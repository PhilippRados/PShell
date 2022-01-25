#include "util.h"

const int BUFFER = 256;

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

void upArrowPress(int* history_index, char** line, const string_array command_history) {
  if (*history_index < command_history.len) {
    *history_index += 1;
    memset(*line, 0, strlen(*line));
    strcpy(*line, command_history.values[*history_index - 1]);
  };
}

void downArrowPress(int* history_index, char** line, const string_array command_history) {
  if (*history_index > 1) {
    *history_index -= 1;
    memset(*line, 0, strlen(*line));
    strcpy(*line, command_history.values[*history_index - 1]);

  } else if (*history_index > 0) {
    *history_index -= 1;
    memset(*line, 0, strlen(*line));
  };
}

bool typedLetter(char** line, const char c, const int i) {
  bool cursor_moved = false;
  if (strlen(*line) == 0 && (c == 32 || c == TAB)) {
    return false;
  }

  if (c < 0 || c > 127) {
    getch();
  } else if (i == strlen(*line)) {
    (*line)[i] = c;
    cursor_moved = true;
  } else if (insertCharAtPos(*line, i, c)) {
    cursor_moved = true;
  }

  return cursor_moved;
}

void arrowPress(char** line, int* i, int* history_index, const bool autocomplete,
                const char* possible_autocomplete, const string_array command_history, const char value) {
  switch (value) {
  case 'A':
    upArrowPress(history_index, line, command_history);

    *i = strlen(*line);
    break;

  case 'B':
    downArrowPress(history_index, line, command_history);

    *i = strlen(*line);
    break;

  case 'C': { // right-arrow
    if (autocomplete && strncmp(*line, possible_autocomplete, strlen(*line)) == 0) {
      memset(*line, 0, strlen(*line));
      strcpy(*line, possible_autocomplete);
      *i = strlen(*line);
    } else {
      *i = (*i < strlen(*line)) ? (*i) + 1 : *i;
    }
    break;
  }

  case 'D': { // left-arrow
    *i = (*i > 0) ? (*i) - 1 : *i;
    break;
  }
  }
}

bool filterHistoryForMatchingAutoComplete(const string_array concatenated, char* line,
                                          char* possible_autocomplete) {

  for (int i = 0; i < concatenated.len; i++) {
    if (strlen(line) > 0 && (strncmp(line, concatenated.values[i], strlen(line)) == 0)) {
      strcpy(possible_autocomplete, concatenated.values[i]);

      return true;
    }
  }

  return false;
}

void underlineIfValidPath(string_array command_line) {
  string_array copy = copyStringArray(command_line);
  replaceAliases(&copy);
  for (int i = 1; i < command_line.len; i++) {
    if (isDirectory(copy.values[i]) || isFile(copy.values[i])) {
      printf(" ");
      printColor(command_line.values[i], WHITE, underline);
    } else {
      printf(" %s", command_line.values[i]);
    }
  }
  free_string_array(&copy);
}

void render(const char* line, const string_array command_history, const string_array PATH_BINS,
            const bool autocomplete, const char* possible_autocomplete, char* directories) {
  CLEAR_LINE;
  CLEAR_BELOW_CURSOR;
  printf("\r");
  printPrompt(directories, CYAN);

  if (strlen(line) > 0) {
    string_array command_line = splitString(line, ' ');

    isInPath(command_line.values[0], PATH_BINS) ? printColor(command_line.values[0], GREEN, standard)
                                                : printColor(command_line.values[0], RED, bold);
    underlineIfValidPath(command_line);
    free_string_array(&command_line);

    if (autocomplete) {
      printf("%s", &possible_autocomplete[strlen(line)]);
    }
  }
}

void tab(char* line, coordinates* cursor_pos, string_array PATH_BINS, coordinates terminal_size, int* i, char* c) {
  if (strlen(line) <= 0)
    return;

  char temp;
  if ((temp = tabLoop(line, cursor_pos, PATH_BINS, terminal_size, *i)) != 0) {
    *c = temp;
  } else {
    *c = -1;
    *i = getWordEndIndex(line, *i);
  }
}

void ctrlFPress(string_array global_command_history, coordinates terminal_size, coordinates cursor_pos, char* line,
                string_array command_history, int* i) {
  // string_array* ref = concatenated_history_commands;
  string_array concatenated_history_commands = concatenateArrays(global_command_history, command_history);
  char* popup_result = popupFuzzyFinder(concatenated_history_commands, terminal_size, cursor_pos.y);
  if (strcmp(popup_result, "") != 0) {
    strcpy(line, popup_result);
  }
  free(popup_result);
  // free_string_array(ref);
  *i = strlen(line);

  if (cursor_pos.y >= (terminal_size.y * 0.85) - 2) {
    cursor_pos.y = (terminal_size.y * 0.85) - 3;
    moveCursor(cursor_pos);
  } else {
    moveCursor(cursor_pos);
  }
}

bool update(char* c, char** line, int* i, coordinates* cursor_pos, int prompt_len, coordinates terminal_size,
            string_array command_history, bool* autocomplete, int* history_index,
            string_array global_command_history, char* possible_autocomplete,
            string_array all_time_command_history, string_array PATH_BINS) {
  bool loop = true;
  if (*c == TAB) {
    tab(*line, cursor_pos, PATH_BINS, terminal_size, i, c);
  }
  if (*c == BACKSPACE) {
    backspaceLogic(line, i);
  } else if (*c == ESCAPE) {
    getch();
    int value = getch();
    arrowPress(line, i, history_index, *autocomplete, possible_autocomplete, command_history, value);
  } else if (*c == '\n') {
    return false;
  } else if ((int)*c == CONTROL_F) {
    ctrlFPress(global_command_history, terminal_size, *cursor_pos, *line, command_history, i);
  } else if (*c != -1 && typedLetter(line, *c, *i)) {
    (*i)++;
  }
  cursor_pos->x = *i + prompt_len;
  *autocomplete = filterHistoryForMatchingAutoComplete(all_time_command_history, *line, possible_autocomplete);

  return loop;
}

char* readLine(string_array PATH_BINS, char* directories, string_array* command_history,
               const string_array global_command_history) {
  const coordinates terminal_size = getTerminalSize();
  char* c = calloc(1, sizeof(char));
  char* line = calloc(BUFFER, sizeof(char));
  char* possible_autocomplete = calloc(BUFFER, sizeof(char));
  int* i = calloc(1, sizeof(int));
  *i = 0;
  int history_index = 0;
  coordinates* cursor_pos = calloc(1, sizeof(coordinates));
  *cursor_pos = getCursorPos();
  int prompt_len = strlen(directories) + 4;
  bool* autocomplete = calloc(1, sizeof(bool));
  *autocomplete = false;
  string_array all_time_command_history = concatenateArrays(*command_history, global_command_history);
  int loop = true;

  while (loop && (*c = getch())) {
    loop =
        update(c, &line, i, cursor_pos, prompt_len, terminal_size, *command_history, autocomplete, &history_index,
               global_command_history, possible_autocomplete, all_time_command_history, PATH_BINS);
    render(line, *command_history, PATH_BINS, *autocomplete, possible_autocomplete, directories);
    moveCursor(*cursor_pos);
  }
  free(possible_autocomplete);
  free_string_array(&all_time_command_history);
  free(autocomplete);
  free(cursor_pos);
  free(i);
  free(c);

  printf("\n");
  return line;
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
        insertStringAtPos(splitted_line->values[i], home_path, j);
      }
    }
  }
}

int runChildProcess(string_array splitted_line) {
  replaceAliases(&splitted_line);
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
  string_array result = {.len = 0, .values = calloc(512, sizeof(char*))};
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_read = fopen(file_path, "r");
  free(file_path);
  char* buf = calloc(64, sizeof(char));
  int line_len;
  unsigned long buf_size;
  int i = 0;
  int realloc_index = 1;

  if (file_to_read == NULL) {
    return result;
  }

  while ((line_len = getline(&buf, &buf_size, file_to_read)) != -1) {
    if (i >= (realloc_index * 512)) {
      realloc_index++;
      result.values = realloc(result.values, realloc_index * 512 * sizeof(char*));
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

void writeSessionCommandsToGlobalHistoryFile(string_array command_history) {
  string_array history_commands = getAllHistoryCommands();
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_write = fopen(file_path, "a");
  free(file_path);

  if (file_to_write == NULL) {
    logger(string, "error\n");
    return;
  }

  for (int i = 0; i < command_history.len; i++) {
    if (!inArray(command_history.values[i], history_commands)) {
      fprintf(file_to_write, "%s\n", command_history.values[i]);
    }
  }

  fclose(file_to_write);
  free_string_array(&history_commands);
}

int main(int argc, char* argv[]) {
  char* line;
  string_array splitted_line;
  char cd[512];
  string_array command_history = {.len = 0, .values = calloc(HISTORY_SIZE, sizeof(char*))};
  string_array PATH_ARR = splitString(getenv("PATH"), ':');
  string_array all_files_in_dir = getAllFilesInDir(&PATH_ARR);
  free_string_array(&PATH_ARR);
  string_array removed_dots = removeDots(&all_files_in_dir);
  string_array PATH_BINS = removeDuplicates(&removed_dots);
  string_array global_command_history = getAllHistoryCommands();

  char* current_dir = getcwd(cd, sizeof(cd));
  char* last_two_dirs = getLastTwoDirs(current_dir);

  CLEAR_SCREEN;

  while (1) {
    printf("\n");
    printPrompt(last_two_dirs, CYAN);

    line = readLine(PATH_BINS, last_two_dirs, &command_history, global_command_history);
    if (strcmp(line, "q") == 0) {
      free(line);
      break;
    }
    if (strlen(line) > 0) {
      splitted_line = splitString(line, ' ');
      if (strcmp(splitted_line.values[0], "cd") == 0) {
        chdir(splitted_line.values[1]);
        current_dir = getcwd(cd, sizeof(cd));
        free(last_two_dirs);
        last_two_dirs = getLastTwoDirs(current_dir);

        if (command_history.len == 0 || strcmp(command_history.values[0], line) != 0) {
          push(line, &command_history);
        }
        free_string_array(&splitted_line);
      } else {
        pid_t child;
        int wstatus;

        child = runChildProcess(splitted_line);

        if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          exit(EXIT_FAILURE);
        }

        if (command_history.len == 0 || strcmp(command_history.values[0], line) != 0) {
          push(line, &command_history);
        }
      }
      free_string_array(&splitted_line);
    }
    free(line);
  }

  writeSessionCommandsToGlobalHistoryFile(command_history);
  free_string_array(&command_history);
  free_string_array(&PATH_BINS);
  free(last_two_dirs);
  // free_string_array(&global_command_history);
  int chunk_size = (global_command_history.len / 512) + 1;
  int j = 0;
  for (int i = 0; i < chunk_size; i++) {
    for (int k = 0; k < 512; k++) {
      free(global_command_history.values[j]);
      global_command_history.values[j] = NULL;
      j++;
    }
  }
  free(global_command_history.values);

  /* free(splitted_line.values); */
  /* free(line); */
  /* free(PATH_ARR.values); */
}
