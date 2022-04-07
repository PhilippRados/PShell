#include "main.h"

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

char* getLastTwoDirs(char* cwd) {
  int i = 1;
  int last_slash_pos = 0;
  int second_to_last_slash = 0;

  for (; cwd[i] != '\n' && cwd[i] != '\0'; i++) {
    if (cwd[i] == '/') {
      second_to_last_slash = last_slash_pos;
      last_slash_pos = i + 1;
    }
  }
  char* last_two_dirs = (char*)calloc(i - second_to_last_slash + 1, sizeof(char));
  strncpy(last_two_dirs, &cwd[second_to_last_slash], i - second_to_last_slash);

  return last_two_dirs;
}

void stringToLower(char* string) {
  for (int i = 0; i < strlen(string); i++) {
    string[i] = tolower(string[i]);
  }
}

string_array removeDots(string_array* array) {
  int j = 0;
  bool remove_index;
  char* not_allowed_dots[] = {".", "..", "./", "../"};
  string_array no_dots_array;
  no_dots_array.values = calloc(array->len, sizeof(char*));
  no_dots_array.len = 0;

  for (int i = 0; i < array->len; i++) {
    remove_index = false;
    for (int k = 0; k < 4; k++) {
      if (strcmp(array->values[i], not_allowed_dots[k]) == 0) {
        remove_index = true;
      }
    }
    if (!remove_index) {
      no_dots_array.values[j] = calloc(strlen(array->values[i]) + 1, sizeof(char));
      strcpy(no_dots_array.values[j], array->values[i]);
      no_dots_array.len += 1;
      j++;
    }
  }
  free_string_array(array);
  return no_dots_array;
}

char* joinHistoryFilePath(char* home_dir, char* destination_file) {
  char* home_dir_copied = calloc(strlen(home_dir) + strlen(destination_file) + 1, sizeof(char));
  strcpy(home_dir_copied, home_dir);

  char* file_path = strcat(home_dir_copied, destination_file);

  return file_path;
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
      perror("realloc");
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

int tokenCmp(const void* a, const void* b) {
  token_index cast_a = *(token_index*)a;
  token_index cast_b = *(token_index*)b;
  return cast_a.start - cast_b.start;
}

token_index_arr tokenizeLine(char* line) {
  char* copy = calloc(strlen(line) + 1, sizeof(char));
  strcpy(copy, line);

  int retval = 0;
  regex_t re;
  int group_nums = ENUM_LEN;
  regmatch_t rm[group_nums];
  char* line_token_regex = "^[ ]*([_A-Za-z0-9]+)|\\|[ ]*([_A-Za-z0-9]+)|(\\|)|([ ]+)";
  char* only_args = "([^ \t]+)";

  if (regcomp(&re, line_token_regex, REG_EXTENDED) != 0) {
    perror("error in compiling regex\n");
  }
  char* start;
  char* end;

  token_index* result_arr = calloc(strlen(line), sizeof(token_index));

  int j;
  for (j = 0; j < strlen(copy); j++) {
    if ((retval = regexec(&re, copy, group_nums, rm, 0)) == 0) {
      for (int i = 1; i < group_nums; i++) {
        if (rm[i].rm_so != -1) { // only quit when in every group .so == -1
          start = copy + rm[i].rm_so;
          end = copy + rm[i].rm_eo;
          result_arr[j].start = rm[i].rm_so; // [i] i matches group index
          result_arr[j].end = rm[i].rm_eo;
          result_arr[j].token = i; // depending on which group matched assign token
          while (start < end) {
            *start++ = '\t';
          }
          break;
        }
      }
    } else {
      break;
    }
  }
  if (regcomp(&re, only_args, REG_EXTENDED) != 0) {
    perror("error in compiling regex\n");
  }
  for (; j < strlen(copy); j++) {
    if ((retval = regexec(&re, copy, group_nums, rm, 0)) == 0) {
      if (rm[1].rm_so != -1) {
        start = copy + rm[1].rm_so;
        end = copy + rm[1].rm_eo;
        result_arr[j].start = rm[1].rm_so;
        result_arr[j].end = rm[1].rm_eo;
        result_arr[j].token = ARG;
        while (start < end) {
          *start++ = '\t';
        }
      }
    } else {
      break;
    }
  }
  free(copy);

  qsort(result_arr, j, sizeof(token_index), tokenCmp);
  token_index_arr result = {.arr = result_arr, .len = j};

  return result;
}

int isBuiltin(char* command, builtins_array builtins) {
  for (int i = 0; i < builtins.len; i++) {
    if (strcmp(command, builtins.array[i].name) == 0) {
      return i;
    }
  }
  return -1;
}

void replaceAliasesString(char** line) {
  for (int j = 0; j < strlen(*line); j++) {
    if (**line == '~') {
      char* home_path = getenv("HOME");
      char* prior_line = calloc(strlen(*line) + 1, sizeof(char));
      strcpy(prior_line, *line);
      removeCharAtPos(prior_line, j + 1);
      *line = realloc(*line, strlen(home_path) + strlen(*line) + 10);
      strcpy(*line, prior_line);
      insertStringAtPos(line, home_path, j);
      free(prior_line);
    }
  }
}

void printTokenizedLine(char* line, token_index_arr tokenized_line, builtins_array BUILTINS,
                        string_array PATH_BINS) {
  for (int i = 0; i < tokenized_line.len; i++) {
    int token_start = tokenized_line.arr[i].start;
    int token_end = tokenized_line.arr[i].end;
    char* substring = calloc(token_end - token_start + 1, sizeof(char));
    strncpy(substring, &line[token_start], token_end - token_start);

    switch (tokenized_line.arr[i].token) {
    case (PIPE_CMD):
    case (CMD): {
      bool in_path = isInPath(substring, PATH_BINS);
      bool is_builtin = isBuiltin(substring, BUILTINS) != -1 ? true : false;
      in_path || is_builtin ? printColor(substring, GREEN, standard) : printColor(substring, RED, bold);
      break;
    }
    case (ARG): {
      char* copy_sub = calloc(strlen(substring) + 1, sizeof(char));
      strcpy(copy_sub, substring);
      replaceAliasesString(&copy_sub);

      int autocomplete = fileComp(copy_sub).array.len;
      if (autocomplete > 0) {
        printColor(substring, WHITE, underline);
      } else {
        printf("%s", substring);
      }
      free(copy_sub);
      break;
    }
    case (WHITESPACE): {
      printf("%s", substring);
      break;
    }
    case (PIPE): {
      printColor("|", YELLOW, standard);
      break;
    }
    default: {
      perror("invalid input\n");
      break;
    }
    }
    free(substring);
  }
}

void printLine(char* line, builtins_array BUILTINS, string_array PATH_BINS) {
  token_index_arr tokenized_line = tokenizeLine(line);
  printTokenizedLine(line, tokenized_line, BUILTINS, PATH_BINS);
}

bool shiftLineIfOverlap(int current_cursor_height, int terminal_height, int line_row_count_with_autocomplete) {
  if ((current_cursor_height + line_row_count_with_autocomplete) < terminal_height)
    return false;

  for (int i = terminal_height; i < (current_cursor_height + line_row_count_with_autocomplete); i++) {
    printf("\n");
  }
  return true;
}

void render(line_data* line_info, autocomplete_data* autocomplete_info, const string_array command_history,
            const string_array PATH_BINS, char* directories, coordinates* starting_cursor_pos,
            coordinates terminal_size, builtins_array BUILTINS) {
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
    printLine(line_info->line, BUILTINS, PATH_BINS);

    if (autocomplete_info->autocomplete) {
      printf("%s", &autocomplete_info->possible_autocomplete[strlen(line_info->line)]);
    }
  }
  coordinates new_cursor_pos = calculateCursorPos(
      terminal_size, (coordinates){.x = 0, .y = starting_cursor_pos->y}, line_info->prompt_len, *line_info->i);

  moveCursor(new_cursor_pos);
  starting_cursor_pos->x = new_cursor_pos.x;
}

void tab(line_data* line_info, coordinates* cursor_pos, string_array PATH_BINS, coordinates terminal_size,
         autocomplete_data autocomplete_info) {
  if (strlen(line_info->line) <= 0)
    return;

  if (tabLoop(line_info, cursor_pos, PATH_BINS, terminal_size)) {
    // successful completion
    *line_info->i = getWordEndIndex(line_info->line, *line_info->i);
    line_info->c = -1;
  }
}

void ctrlFPress(string_array all_time_command_history, coordinates terminal_size, coordinates* cursor_pos,
                char* possible_autocomplete, line_data* line_info) {
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

bool update(line_data* line_info, autocomplete_data* autocomplete_info, history_data* history_info,
            coordinates terminal_size, string_array PATH_BINS, coordinates* cursor_pos) {

  string_array all_time_command_history =
      concatenateArrays(history_info->sessions_command_history, history_info->global_command_history);
  bool loop = true;

  if (line_info->c == TAB) {
    tab(line_info, cursor_pos, PATH_BINS, terminal_size, *autocomplete_info);
  }
  if (line_info->c == BACKSPACE) {
    backspaceLogic(line_info->line, line_info->i);
  } else if (line_info->c == ESCAPE) {
    arrowPress(line_info, history_info, autocomplete_info);
  } else if (line_info->c == '\n') {
    free_string_array(&all_time_command_history);
    return false;
  } else if ((int)line_info->c == CONTROL_F) {
    ctrlFPress(all_time_command_history, terminal_size, cursor_pos, autocomplete_info->possible_autocomplete,
               line_info);
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

char* readLine(string_array PATH_BINS, char* directories, string_array* command_history,
               const string_array global_command_history, builtins_array BUILTINS) {

  const coordinates terminal_size = getTerminalSize();
  line_data* line_info = lineDataConstructor(strlen(directories));
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();
  history_data* history_info = historyDataConstructor(command_history, global_command_history);
  coordinates* cursor_pos = calloc(1, sizeof(coordinates));
  *cursor_pos = getCursorPos();
  int loop = true;

  while (loop && (line_info->c = getch())) {
    loop = update(line_info, autocomplete_info, history_info, terminal_size, PATH_BINS, cursor_pos);

    render(line_info, autocomplete_info, history_info->sessions_command_history, PATH_BINS, directories,
           cursor_pos, terminal_size, BUILTINS);
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

void printPrompt(const char* dir, color color) {
  printColor(dir, color, bold);
  printf(" ");
  printColor("\u2771 ", GREEN, standard);
}

void pipeOutputToFile(char* filename) {
  int file = open(filename, O_WRONLY | O_CREAT, 0777);

  int file2 = dup2(file, STDOUT_FILENO);
  close(file);
}

void replaceAliases(string_array* splitted_line) {
  for (int i = 0; i < splitted_line->len; i++) {
    for (int j = 0; j < strlen(splitted_line->values[i]); j++) {
      if (splitted_line->values[i][j] == '~') {
        char* home_path = getenv("HOME");
        char* prior_line = calloc(strlen(splitted_line->values[i]) + 1, sizeof(char));
        strcpy(prior_line, splitted_line->values[i]);
        removeCharAtPos(prior_line, j + 1);
        splitted_line->values[i] =
            realloc(splitted_line->values[i], strlen(home_path) + strlen(splitted_line->values[i]) + 10);
        strcpy(splitted_line->values[i], prior_line);
        insertStringAtPos(&(splitted_line->values[i]), home_path, j);
        free(prior_line);
      }
    }
  }
}

int runChildProcess(string_array splitted_line) {
  pid_t pid = fork();

  if (pid == 0) {
    int error = execvp(splitted_line.values[0], splitted_line.values);
    if (error) {
      printf("couldn't find command %s\n", splitted_line.values[0]);
    }
  } else {
    return pid;
  }
  return false;
}

void push(char* line, string_array* command_history) {
  if (command_history->len > 0) {
    for (int i = command_history->len; i > 0; i--) {
      if (i <= HISTORY_SIZE) {
        command_history->values[i] = command_history->values[i - 1];
      }
    }
  }
  (command_history->len <= HISTORY_SIZE) ? command_history->len++ : command_history->len;
  command_history->values[0] = calloc(strlen(line) + 1, sizeof(char));
  strcpy(command_history->values[0], line);
}

bool arrCmp(string_array arr1, string_array arr2) {
  if (arr1.len != arr2.len) {
    return false;
  }
  for (int i = 0; i < arr1.len; i++) {
    if (strcmp(arr1.values[i], arr2.values[i]) != 0) {
      return false;
    }
  }
  return true;
}

string_array getAllHistoryCommands() {
  size_t size = BUFFER * sizeof(char*);
  string_array result = {.len = 0, .values = calloc(BUFFER, sizeof(char*))};
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_read = fopen(file_path, "r");
  free(file_path);
  char* buf = NULL;
  int line_len;
  unsigned long buf_size;
  int i = 0;

  if (file_to_read == NULL) {
    return result;
  }

  while ((line_len = getline(&buf, &buf_size, file_to_read)) != -1) {
    if ((i * sizeof(char*)) >= size) {
      result.values = realloc(result.values, size * 1.5);
      size *= 1.5;
    }

    result.values[i] = calloc(strlen(buf), sizeof(char));
    strncpy(result.values[i], buf, strlen(buf) - 1);
    i++;
  }
  result.len = i;

  free(buf);
  fclose(file_to_read);
  return result;
}

void writeSessionCommandsToGlobalHistoryFile(string_array command_history, string_array global_history) {
  // string_array history_commands = getAllHistoryCommands();
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_write = fopen(file_path, "a");
  free(file_path);

  if (file_to_write == NULL) {
    logger(string, "error\n");
    return;
  }

  for (int i = 0; i < command_history.len; i++) {
    if (!inArray(command_history.values[i], global_history)) {
      fprintf(file_to_write, "%s\n", command_history.values[i]);
    }
  }

  fclose(file_to_write);
  free_string_array(&global_history);
}

void cd(string_array splitted_line, char* current_dir, char* last_two_dirs, char* dir) {
  if (splitted_line.len == 2) {
    if (chdir(splitted_line.values[1]) == -1) {
      printf("cd: %s: No such file or directory\n", splitted_line.values[1]);
    }
  } else if (splitted_line.len > 2) {
    printf("Too many arguments\n");
  } else {
    printf("Usage: cd [path]\n");
  }
}

bool callBuiltin(string_array splitted_line, function_by_name builtins[], int function_index) {
  if (strcmp(splitted_line.values[0], "exit") == 0) {
    return false;
  }
  (*builtins[function_index].func)(splitted_line);
  return true;
}

void pushToCommandHistory(char* line, string_array* command_history) {
  if (command_history->len == 0 || strcmp(command_history->values[0], line) != 0) {
    push(line, command_history);
  }
}

#ifndef TEST

int main(int argc, char* argv[]) {
  char* line;
  string_array splitted_line;
  char dir[PATH_MAX];
  string_array command_history = {.len = 0, .values = calloc(HISTORY_SIZE, sizeof(char*))};
  string_array PATH_ARR = splitString(getenv("PATH"), ':');
  string_array all_files_in_dir = getAllFilesInDir(&PATH_ARR);
  free_string_array(&PATH_ARR);
  string_array removed_dots = removeDots(&all_files_in_dir);
  string_array PATH_BINS = removeDuplicates(&removed_dots);
  string_array global_command_history = getAllHistoryCommands();

  char* current_dir = getcwd(dir, sizeof(dir));
  char* last_two_dirs = getLastTwoDirs(current_dir);
  function_by_name builtin_funcs[] = {
      {"exit", NULL},
      {"cd", cd},
  };
  builtins_array BUILTINS = {
      .array = builtin_funcs,
      .len = sizeof(builtin_funcs) / sizeof(builtin_funcs[0]),
  };

  CLEAR_SCREEN;

  while (1) {
    printf("\n");
    printPrompt(last_two_dirs, CYAN);

    line = readLine(PATH_BINS, last_two_dirs, &command_history, global_command_history, BUILTINS);
    line = removeMultipleWhitespaces(line);
    if (strlen(line) > 0) {
      splitted_line = splitString(line, ' ');
      replaceAliases(&splitted_line);
      int builtin_index;
      if ((builtin_index = isBuiltin(splitted_line.values[0], BUILTINS)) != -1) {
        if (!callBuiltin(splitted_line, BUILTINS.array, builtin_index)) {
          free_string_array(&splitted_line);
          free(line);
          break;
        }
        current_dir = getcwd(dir, sizeof(dir));
        free(last_two_dirs);
        last_two_dirs = getLastTwoDirs(current_dir);

        pushToCommandHistory(line, &command_history);

      } else {
        pid_t child;
        int wstatus;

        child = runChildProcess(splitted_line);

        if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          exit(EXIT_FAILURE);
        }

        pushToCommandHistory(line, &command_history);
      }
      free_string_array(&splitted_line);
    }
    free(line);
  }

  writeSessionCommandsToGlobalHistoryFile(command_history, global_command_history);
  free_string_array(&command_history);
  free_string_array(&PATH_BINS);
  free(last_two_dirs);
}

#endif /* !TEST */
