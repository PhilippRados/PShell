#include "tab_complete.h"

int maxWidthTerm(int width, int terminal_width) {
  if (width > terminal_width - 2) {
    width = terminal_width - 2;
  }
  return width;
}

void tabRender(string_array possible_tabcomplete, int tab_index, int col_size, int format_width,
               int terminal_width) {
  int j = 0;
  char* complete;

  while (j < possible_tabcomplete.len) {
    printf("\n");
    for (int x = 0; x < col_size; x++) {
      if (j >= possible_tabcomplete.len)
        break;
      complete = shortenIfTooLong(possible_tabcomplete.values[j], terminal_width);

      int diff_len = strlen(complete) - format_width;
      if (tab_index == j) {

        printColor(complete, GREEN, reversed);
        printf("%-*s", diff_len, "");
      } else {
        if (possible_tabcomplete.values[j][strlen(possible_tabcomplete.values[j]) - 1] == '/') {
          printColor(complete, CYAN, bold);
          printf("%-*s", diff_len, "");
        } else {
          printf("%-*s", format_width, complete);
        }
      }
      j++;
      free(complete);
    }
  }
}

void escapeWhitespace(string_array* arr) {
  for (int i = 0; i < arr->len; i++) {
    for (int j = 0; j < strlen(arr->values[i]); j++) {
      if (arr->values[i][j] == ' ') {
        arr->values[i] = realloc(arr->values[i], (strlen(arr->values[i]) + 2) * sizeof(char));
        // insert escape \\ in front of whitespace
        insertCharAtPos(arr->values[i], j, '\\');
        j++;
      }
    }
  }
}
autocomplete_array checkForAutocomplete(char* current_word, enum token current_token,
                                        const string_array PATH_BINS) {
  autocomplete_array possible_autocomplete = {.array.len = 0};

  if (current_token == CMD || current_token == PIPE_CMD || current_token == AMP_CMD) { // autocomplete for commands
    string_array filtered = filterMatching(current_word, PATH_BINS);

    possible_autocomplete = (autocomplete_array){
        .array.values = filtered.values, .array.len = filtered.len, .appending_index = strlen(current_word)};
  } else { // autocomplete for files
    possible_autocomplete = fileComp(current_word);
    escapeWhitespace(&possible_autocomplete.array);
  }

  return possible_autocomplete;
}

void moveCursorIfShifted(render_objects* render_data) {
  if (render_data->cursor_height_diff <= render_data->row_size || render_data->cursor_height_diff == 0) {
    render_data->cursor_pos->y -= render_data->row_size - render_data->cursor_height_diff;
    moveCursor(*render_data->cursor_pos);
  } else {
    moveCursor(*render_data->cursor_pos);
  }
  render_data->cursor_pos->y -= render_data->cursor_row;
}

void renderCompletion(autocomplete_array possible_tabcomplete, int tab_index, render_objects* render_data) {
  render_data->cursor_pos->y += render_data->cursor_row;
  int bottom_line_y =
      render_data->cursor_pos->y - render_data->cursor_row + render_data->line_row_count_with_autocomplete;
  render_data->cursor_height_diff = render_data->terminal_size.y - bottom_line_y;

  moveCursor((coordinates){1000, bottom_line_y}); // have to move cursor to end of
                                                  // line to not cut off in middle
  CLEAR_BELOW_CURSOR;
  tabRender(possible_tabcomplete.array, tab_index, render_data->col_size, render_data->format_width,
            render_data->terminal_size.x);
  moveCursorIfShifted(render_data);
}

bool dontShowMatches(char answer, render_objects* render_data, autocomplete_array possible_tabcomplete) {
  bool result = false;

  if (answer != 'y') {
    result = true;
  } else if (render_data->row_size >= render_data->terminal_size.y) {
    renderCompletion(possible_tabcomplete, -1, render_data);
    printf("\n\n");

    for (int i = 0; i < render_data->line_row_count_with_autocomplete; i++) {
      printf("\n");
    }
    render_data->cursor_pos->y =
        render_data->terminal_size.y + render_data->cursor_row - render_data->line_row_count_with_autocomplete;
    result = true;
  }
  return result;
}

bool tooManyMatches(render_objects* render_data, autocomplete_array possible_tabcomplete) {
  char answer;
  char* prompt_sentence = "\nThe list of possible matches is %d lines. Do you want to print all of them? (y/n) ";
  int bottom_line_y = render_data->cursor_pos->y + render_data->line_row_count_with_autocomplete;

  moveCursor((coordinates){1000, bottom_line_y});

  if (!(render_data->row_size > 10 || render_data->row_size > render_data->terminal_size.y))
    return false;

  printf(prompt_sentence, render_data->row_size);
  answer = getch();

  int prompt_row_count = calculateRowCount(render_data->terminal_size, 0, strlen(prompt_sentence) - 1) + 1;
  int diff = prompt_row_count + bottom_line_y - render_data->terminal_size.y;
  if (diff > 0) {
    render_data->cursor_pos->y -= diff;
  }

  return dontShowMatches(answer, render_data, possible_tabcomplete);
}

bool tabPress(autocomplete_array possible_tabcomplete, int* tab_index, line_data* line_info,
              token_index current_token) {
  if (possible_tabcomplete.array.len == 1) {
    removeSlice(&line_info->line, *line_info->i, current_token.end);
    insertStringAtPos(&line_info->line,
                      &(possible_tabcomplete.array.values[0])[possible_tabcomplete.appending_index],
                      *line_info->i);

    line_info->size = (strlen(line_info->line) + 1) * sizeof(char);
    return true;
  } else if (possible_tabcomplete.array.len > 1) {
    if (*tab_index < possible_tabcomplete.array.len - 1) {
      *tab_index += 1;
    } else {
      *tab_index = 0;
    }
  }
  return false;
}

void shiftTabPress(autocomplete_array possible_tabcomplete, int* tab_index) {
  getch();
  if (getch() == 'Z') { // Shift-Tab
    if (*tab_index > 0) {
      *tab_index -= 1;
    } else {
      *tab_index = possible_tabcomplete.array.len - 1;
    }
  }
}

void enterPress(autocomplete_array possible_tabcomplete, line_data* line_info, int tab_index,
                token_index current_token) {
  removeSlice(&line_info->line, *line_info->i, current_token.end);
  // remove-slice is correct
  // have to count escapes upto line_index
  insertStringAtPos(&line_info->line,
                    &(possible_tabcomplete.array.values[tab_index])[possible_tabcomplete.appending_index],
                    *line_info->i);

  line_info->size = (strlen(line_info->line) + 1) * sizeof(char);
}

tab_completion updateCompletion(autocomplete_array possible_tabcomplete, char* c, line_data* line_info,
                                int* tab_index, token_index current_token) {
  tab_completion result = {.successful = false, .continue_loop = true};
  if (*c == TAB) {
    if (tabPress(possible_tabcomplete, tab_index, line_info, current_token)) {
      result.successful = true;
      result.continue_loop = false;
    }

  } else if (*c == ESCAPE) {
    shiftTabPress(possible_tabcomplete, tab_index);

  } else if (*c == '\n') {
    enterPress(possible_tabcomplete, line_info, *tab_index, current_token);
    result.successful = true;
    result.continue_loop = false;

  } else {
    result.successful = false;
    result.continue_loop = false;
  }

  return result;
}

render_objects initializeRenderObjects(coordinates terminal_size, autocomplete_array possible_tabcomplete,
                                       coordinates* cursor_pos, int cursor_row,
                                       int line_row_count_with_autocomplete) {
  int format_width = maxWidthTerm(getLongestWordInArray(possible_tabcomplete.array), terminal_size.x) + 2;
  int col_size = terminal_size.x / format_width;
  int row_size = ceil(possible_tabcomplete.array.len / (float)col_size);
  int cursor_height_diff = terminal_size.y - cursor_pos->y;

  return (render_objects){
      .format_width = format_width,
      .col_size = col_size,
      .row_size = row_size,
      .cursor_height_diff = cursor_height_diff,
      .cursor_pos = cursor_pos,
      .terminal_size = terminal_size,
      .cursor_row = cursor_row,
      .line_row_count_with_autocomplete = line_row_count_with_autocomplete,
  };
}

autocomplete_array removeDotFiles(autocomplete_array* tabcomp) {
  autocomplete_array new;
  new.array.values = calloc(tabcomp->array.len, sizeof(char*));
  int j = 0;
  for (int i = 0; i < tabcomp->array.len; i++) {
    if (tabcomp->array.values[i][0] != '.') {
      new.array.values[j] = calloc(strlen(tabcomp->array.values[i]) + 1, sizeof(char));
      strcpy(new.array.values[j], tabcomp->array.values[i]);
      j++;
    }
  }
  new.appending_index = tabcomp->appending_index;
  new.array.len = j;
  free_string_array(&tabcomp->array);
  return new;
}

void removeDotFilesIfnecessary(char* current_word, autocomplete_array* possible_tabcomplete) {
  int appending_index;
  char* removed_sub;
  if ((appending_index = getAppendingIndex(current_word, '/')) != -1) {
    removed_sub = &(current_word[strlen(current_word) - getAppendingIndex(current_word, '/')]);
  } else {
    removed_sub = current_word;
  }
  if (removed_sub[0] != '.') {
    *possible_tabcomplete = removeDotFiles(possible_tabcomplete);
  }
}

char* getCurrentWord(char* line, int line_index, token_index current_token) {
  if (current_token.start == -1 && current_token.end == -1) {
    return NULL;
  } else if (current_token.token == WHITESPACE) {
    char* result = calloc(2, sizeof(char));
    strcpy(result, "");
    return result;
  } else {
    char* result = calloc(line_index - current_token.start + 1, sizeof(char));
    strncpy(result, &line[current_token.start], line_index - current_token.start);
    return result;
  }
}

bool tabLoop(line_data* line_info, coordinates* cursor_pos, const string_array PATH_BINS,
             const coordinates terminal_size, token_index current_token) {
  int tab_index = -1;
  char* current_word = getCurrentWord(line_info->line, *line_info->i, current_token);
  removeEscapesString(&current_word);
  autocomplete_array possible_tabcomplete = checkForAutocomplete(current_word, current_token.token, PATH_BINS);
  removeDotFilesIfnecessary(current_word, &possible_tabcomplete);

  render_objects render_data =
      initializeRenderObjects(terminal_size, possible_tabcomplete, cursor_pos, line_info->cursor_row,
                              line_info->line_row_count_with_autocomplete);
  tab_completion completion_result;

  if (possible_tabcomplete.array.len <= 0 || tooManyMatches(&render_data, possible_tabcomplete)) {
    free_string_array(&(possible_tabcomplete.array));
    free(current_word);
    return false;
  }
  do {
    if ((completion_result =
             updateCompletion(possible_tabcomplete, &line_info->c, line_info, &tab_index, current_token))
            .continue_loop) {
      renderCompletion(possible_tabcomplete, tab_index, &render_data);
    }

  } while (completion_result.continue_loop && (line_info->c = getch()));

  free_string_array(&(possible_tabcomplete.array));
  free(current_word);

  return completion_result.successful;
}
