#include "readline.h"

line_data* lineDataConstructor(int directory_len) {
  line_data* line_info = calloc(1, sizeof(line_data));
  *line_info = (line_data){
      .line = calloc(BUFFER, sizeof(char)),
      .i = calloc(1, sizeof(int)),
      .prompt_len = directory_len + 4,
      .line_row_count_with_autocomplete = 0,
      .cursor_row = 0,
      .size = BUFFER * sizeof(char),
  };
  *line_info->i = 0;

  return line_info;
}

autocomplete_data* autocompleteDataConstructor() {
  autocomplete_data* autocomplete_info = calloc(1, sizeof(autocomplete_data));
  *autocomplete_info = (autocomplete_data){
      .possible_autocomplete = calloc(BUFFER, sizeof(char)),
      .autocomplete = false,
      .size = BUFFER * sizeof(char),
  };

  return autocomplete_info;
}

history_data* historyDataConstructor(string_array* command_history, string_array global_command_history) {
  history_data* history_info = calloc(1, sizeof(history_data));
  *history_info = (history_data){
      .history_index = 0,
      .sessions_command_history = *command_history,
      .global_command_history = global_command_history,
  };

  return history_info;
}

string_array concatenateArrays(const string_array one, const string_array two) {
  if (one.len == 0 && two.len == 0)
    return (string_array){.len = 0, .values = NULL};
  string_array concatenated = {.values = calloc((one.len + two.len), sizeof(char*))};
  int i = 0;

  for (int k = 0; k < one.len; k++) {
    concatenated.values[i] = calloc(strlen(one.values[k]) + 1, sizeof(char));
    strcpy(concatenated.values[i], one.values[k]);
    i++;
  }
  for (int j = 0; j < two.len; j++) {
    concatenated.values[i] = calloc(strlen(two.values[j]) + 1, sizeof(char));
    strcpy(concatenated.values[i], two.values[j]);
    i++;
  }
  concatenated.len = i;

  return concatenated;
}

bool wildcardInCompletion(token_index_arr token, int line_index) {
  int current_token_index = 0;
  for (int i = 0; i < token.len; i++) {
    if (line_index >= token.arr[i].start && line_index <= token.arr[i].end) {
      current_token_index = i;
    }
  }
  for (int j = current_token_index; j > 0; j--) {
    if (token.arr[j].token == WHITESPACE) {
      break;
    } else if (token.arr[j].token == ASTERISK || token.arr[j].token == QUESTION) {
      return true;
    }
  }
  return false;
}

int firstNonWhitespaceToken(token_index_arr token_line) {
  for (int i = 0; i < token_line.len; i++) {
    if (token_line.arr[i].token != WHITESPACE) {
      return token_line.arr[i].start;
    }
  }
  return INT_MAX;
}

bool tabCompBeforeFirstWord(token_index_arr tokenized_line, int line_index) {
  return tokenized_line.len > 0 && line_index <= firstNonWhitespaceToken(tokenized_line);
}

void tab(line_data* line_info, coordinates* cursor_pos, string_array PATH_BINS, coordinates terminal_size) {
  if (strlen(line_info->line) <= 0)
    return;

  token_index_arr tokenized_line = tokenizeLine(line_info->line);

  /* this should also get token when at end of word */
  token_index current_token = getCurrentToken(*line_info->i, tokenized_line);
  if (current_token.token != CMD && wildcardInCompletion(tokenized_line, *line_info->i)) {
    return;
  }

  if (!tabCompBeforeFirstWord(tokenized_line, *line_info->i) &&
      tabLoop(line_info, cursor_pos, PATH_BINS, terminal_size, current_token)) {
    // successful completion
    free(tokenized_line.arr);
    tokenized_line = tokenizeLine(line_info->line);
    current_token =
        getCurrentToken(*line_info->i + 1, tokenized_line); /* doesnt recognize token when at end of word*/
    *line_info->i = current_token.end;
    line_info->c = -1;
  }
  free(tokenized_line.arr);
}

void upArrowPress(char* line, history_data* history_info) {
  if (history_info->history_index < history_info->sessions_command_history.len) {
    history_info->history_index += 1;
    memset(line, 0, strlen(line));
    strcpy(line, history_info->sessions_command_history.values[history_info->history_index - 1]);
  };
}

void downArrowPress(char* line, history_data* history_info) {
  if (history_info->history_index > 1) {
    history_info->history_index -= 1;
    memset(line, 0, strlen(line));
    strcpy(line, history_info->sessions_command_history.values[history_info->history_index - 1]);

  } else if (history_info->history_index > 0) {
    history_info->history_index -= 1;
    memset(line, 0, strlen(line));
  };
}

bool typedLetter(line_data* line_info) {
  bool cursor_moved = false;
  if ((line_info->c < 27 || line_info->c > 127) || (strlen(line_info->line) == 0 && line_info->c == TAB)) {
    return false;
  } else if ((strlen(line_info->line) * sizeof(char) + 1) >= line_info->size) {
    char* tmp;
    if ((tmp = realloc(line_info->line, 1.5 * line_info->size)) == NULL) {
      perror("psh:");
    } else {
      line_info->line = tmp;
      line_info->size *= 1.5;
    }
  }

  if (*line_info->i == strlen(line_info->line)) {
    (line_info->line)[*line_info->i] = line_info->c;
    (line_info->line)[*line_info->i + 1] = '\0';
    cursor_moved = true;
  } else if (insertCharAtPos(line_info->line, *line_info->i, line_info->c)) {
    cursor_moved = true;
  }

  return cursor_moved;
}

void arrowPress(line_data* line_info, history_data* history_info, autocomplete_data* autocomplete_info) {
  getch();
  int value = getch();
  switch (value) {
  case 'A':
    upArrowPress(line_info->line, history_info);

    *line_info->i = strlen(line_info->line);
    break;

  case 'B':
    downArrowPress(line_info->line, history_info);

    *line_info->i = strlen(line_info->line);
    break;

  case 'C': { // right-arrow
    if (autocomplete_info->autocomplete &&
        strncmp(line_info->line, autocomplete_info->possible_autocomplete, strlen(line_info->line)) == 0) {
      memset(line_info->line, 0, strlen(line_info->line));
      if (((strlen(autocomplete_info->possible_autocomplete) + 1) * sizeof(char)) >= line_info->size) {
        line_info->line =
            realloc(line_info->line, (strlen(autocomplete_info->possible_autocomplete) + 1) * sizeof(char));
        line_info->size = (strlen(autocomplete_info->possible_autocomplete) + 1) * sizeof(char);
      }
      strcpy(line_info->line, autocomplete_info->possible_autocomplete);
      *line_info->i = strlen(line_info->line);
    } else {
      *line_info->i = (*line_info->i < strlen(line_info->line)) ? *line_info->i + 1 : *line_info->i;
    }
    break;
  }

  case 'D': { // left-arrow
    *line_info->i = (*line_info->i > 0) ? (*line_info->i) - 1 : *line_info->i;
    break;
  }
  }
}

void ctrlFPress(string_array all_time_command_history, coordinates terminal_size, coordinates* cursor_pos,
                line_data* line_info) {
  fuzzy_result popup_result = popupFuzzyFinder(all_time_command_history, terminal_size, cursor_pos->y,
                                               line_info->line_row_count_with_autocomplete);

  if (strcmp(popup_result.line, "") != 0) {
    if (((strlen(popup_result.line) + 1) * sizeof(char)) >= line_info->size) {
      line_info->line = realloc(line_info->line, (strlen(popup_result.line) + 1) * sizeof(char));
      line_info->size = (strlen(popup_result.line) + 1) * sizeof(char);
    }
    strcpy(line_info->line, popup_result.line);
    *line_info->i = strlen(line_info->line);
  }
  free(popup_result.line);

  if (popup_result.shifted) {
    cursor_pos->y = (terminal_size.y * 0.85) - 3 - line_info->line_row_count_with_autocomplete;
    moveCursor(*cursor_pos);
  } else {
    moveCursor(*cursor_pos);
  }
}

bool filterHistoryForMatchingAutoComplete(const string_array all_time_commands, char* line,
                                          autocomplete_data* autocomplete_info) {

  for (int i = 0; i < all_time_commands.len; i++) {
    if (strlen(line) > 0 && (strncmp(line, all_time_commands.values[i], strlen(line)) == 0)) {
      if (strlen(all_time_commands.values[i]) >= autocomplete_info->size) {
        autocomplete_info->possible_autocomplete = realloc(
            autocomplete_info->possible_autocomplete, (strlen(all_time_commands.values[i]) + 1) * sizeof(char));
        autocomplete_info->size = strlen(all_time_commands.values[i]) + 1;
      }
      strcpy(autocomplete_info->possible_autocomplete, all_time_commands.values[i]);

      return true;
    }
  }

  return false;
}

bool update(line_data* line_info, autocomplete_data* autocomplete_info, history_data* history_info,
            coordinates terminal_size, string_array PATH_BINS, coordinates* cursor_pos) {

  string_array all_time_command_history =
      concatenateArrays(history_info->sessions_command_history, history_info->global_command_history);
  bool loop = true;

  if (line_info->c == TAB) {
    tab(line_info, cursor_pos, PATH_BINS, terminal_size);
  }
  if (line_info->c == BACKSPACE) {
    backspaceLogic(line_info->line, line_info->i);
  } else if (line_info->c == ESCAPE) {
    arrowPress(line_info, history_info, autocomplete_info);
  } else if (line_info->c == '\n') {
    free_string_array(&all_time_command_history);
    return false;
  } else if ((int)line_info->c == CONTROL_F) {
    ctrlFPress(all_time_command_history, terminal_size, cursor_pos, line_info);
  } else if (line_info->c != -1 && typedLetter(line_info)) {
    (*line_info->i)++;
  }
  autocomplete_info->autocomplete =
      filterHistoryForMatchingAutoComplete(all_time_command_history, line_info->line, autocomplete_info);
  int line_len = (autocomplete_info->autocomplete) ? strlen(autocomplete_info->possible_autocomplete)
                                                   : strlen(line_info->line);
  line_info->line_row_count_with_autocomplete = calculateRowCount(terminal_size, line_info->prompt_len, line_len);
  line_info->cursor_row = calculateRowCount(terminal_size, line_info->prompt_len, *line_info->i);

  free_string_array(&all_time_command_history);

  return loop;
}

bool shiftLineIfOverlap(int current_cursor_height, int terminal_height, int line_row_count_with_autocomplete) {
  if ((current_cursor_height + line_row_count_with_autocomplete) < terminal_height)
    return false;

  for (int i = terminal_height; i < (current_cursor_height + line_row_count_with_autocomplete); i++) {
    printf("\n");
  }
  return true;
}

void stringToLower(char* string) {
  for (int i = 0; i < strlen(string); i++) {
    string[i] = tolower(string[i]);
  }
}

bool isInPath(char* line, string_array PATH_BINS) {
  bool result = false;
  char* line_copy = calloc(strlen(line) + 1, sizeof(char));
  strcpy(line_copy, line);
  stringToLower(line_copy);

  char* bin_copy;
  for (int i = 0; i < PATH_BINS.len; i++) {
    bin_copy = calloc(strlen(PATH_BINS.values[i]) + 1, sizeof(char));
    strcpy(bin_copy, PATH_BINS.values[i]);
    stringToLower(bin_copy);

    if (strcmp(bin_copy, line_copy) == 0) {
      result = true;
    }
    free(bin_copy);
  }
  free(line_copy);
  return result;
}

void printTokenizedLine(line_data line_info, token_index_arr tokenized_line, builtins_array BUILTINS,
                        string_array PATH_BINS) {
  for (int i = 0; i < tokenized_line.len; i++) {
    int token_start = tokenized_line.arr[i].start;
    int token_end = tokenized_line.arr[i].end;
    char* substring = calloc(token_end - token_start + 1, sizeof(char));
    strncpy(substring, &line_info.line[token_start], token_end - token_start);

    switch (tokenized_line.arr[i].token) {
    case (AMP_CMD):
    case (PIPE_CMD):
    case (CMD): {
      bool in_path = isInPath(substring, PATH_BINS);
      bool is_builtin = isBuiltin(substring, BUILTINS) != -1 ? true : false;
      bool is_exec = isExec(substring);
      in_path || is_builtin || is_exec ? printColor(substring, GREEN, standard) : printColor(substring, RED, bold);
      break;
    }
    case (ARG): {
      char* copy_sub = calloc(strlen(substring) + 1, sizeof(char));
      strcpy(copy_sub, substring);
      replaceAliasesString(&copy_sub);
      removeEscapesString(&copy_sub);

      autocomplete_array autocomplete = fileComp(copy_sub);
      if (autocomplete.array.len > 0 && !wildcardInCompletion(tokenized_line, *line_info.i)) {
        printColor(substring, WHITE, underline);
      } else if (substring[0] == '\'' && substring[strlen(substring) - 1] == '\'') {
        printColor(substring, YELLOW, standard);
      } else {
        printf("%s", substring);
      }
      free(copy_sub);
      free_string_array(&(autocomplete.array));
      break;
    }
    case (WHITESPACE): {
      printf("%s", substring);
      break;
    }
    case (GREAT):
    case (GREATGREAT):
    case (LESS):
    case (AMP_GREAT):
    case (AMP_GREATGREAT):
    case (PIPE):
    case (AMPAMP): {
      printColor(substring, YELLOW, standard);
      break;
    }
    case (QUESTION):
    case (ASTERISK): {
      printColor(substring, BLUE, bold);
      break;
    }
    default: {
      fprintf(stderr, "psh: invalid input\n");
      break;
    }
    }
    free(substring);
  }
}

void printLine(line_data line_info, builtins_array BUILTINS, string_array PATH_BINS) {
  token_index_arr tokenized_line = tokenizeLine(line_info.line);
  printTokenizedLine(line_info, tokenized_line, BUILTINS, PATH_BINS);
  free(tokenized_line.arr);
}

void render(line_data* line_info, autocomplete_data* autocomplete_info, const string_array PATH_BINS,
            char* directories, coordinates* starting_cursor_pos, coordinates terminal_size,
            builtins_array BUILTINS) {
  if (shiftLineIfOverlap(starting_cursor_pos->y, terminal_size.y, line_info->line_row_count_with_autocomplete)) {
    starting_cursor_pos->y -=
        (starting_cursor_pos->y + line_info->line_row_count_with_autocomplete) - terminal_size.y;
  }
  moveCursor(*starting_cursor_pos);

  CLEAR_LINE;
  CLEAR_BELOW_CURSOR;
  printf("\r");
  printPrompt(directories, CYAN);

  if (strlen(line_info->line) > 0) {
    printLine(*line_info, BUILTINS, PATH_BINS);

    if (autocomplete_info->autocomplete) {
      printf("%s", &autocomplete_info->possible_autocomplete[strlen(line_info->line)]);
    }
  }
  coordinates new_cursor_pos = calculateCursorPos(
      terminal_size, (coordinates){.x = 0, .y = starting_cursor_pos->y}, line_info->prompt_len, *line_info->i);

  moveCursor(new_cursor_pos);
  starting_cursor_pos->x = new_cursor_pos.x;
}

char* readLine(string_array PATH_BINS, char* directories, string_array* command_history,
               const string_array global_command_history, builtins_array BUILTINS) {

  const coordinates terminal_size = getTerminalSize();
  line_data* line_info = lineDataConstructor(strlen(directories));
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();
  history_data* history_info = historyDataConstructor(command_history, global_command_history);
  coordinates* cursor_pos = calloc(1, sizeof(coordinates));
  *cursor_pos = getCursorPos();
  int loop = true;

  while (loop && (line_info->c = getch()) != -1) {
    loop = update(line_info, autocomplete_info, history_info, terminal_size, PATH_BINS, cursor_pos);

    render(line_info, autocomplete_info, PATH_BINS, directories, cursor_pos, terminal_size, BUILTINS);
  }

  moveCursor((coordinates){
      1000, cursor_pos->y + calculateRowCount(terminal_size, line_info->prompt_len, strlen(line_info->line))});

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free(history_info);
  free(cursor_pos);
  char* result = calloc(strlen(line_info->line) + 1, sizeof(char));
  strcpy(result, line_info->line);
  free(line_info->line);
  free(line_info->i);
  free(line_info);

  printf("\n");
  return result;
}
