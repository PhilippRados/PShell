#include "util.h"
#include <regex.h>

int getch() {
  struct termios oldattr, newattr;
  int ch;
  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
  return ch;
}

void printColor(const char* string, color color, enum color_decorations color_decorations) {
  char command[13];

  sprintf(command, "%c[%d;%d;%dm", 0x1B, color_decorations, color.fg, color.bg);
  printf("%s", command);
  printf("%s", string);

  sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
  printf("%s", command);
}

bool isOnlyDelimeter(const char* string, char delimeter) {
  for (int i = 0; i < strlen(string); i++) {
    if (string[i] != delimeter) {
      return false;
    }
  }
  return true;
}

char* removeMultipleWhitespaces(char* string) {
  int i = 0;
  for (; i < strlen(string) && string[i] == ' '; i++)
    ;
  char* new_string = calloc(strlen(string) + 1, sizeof(char));
  strcpy(new_string, &string[i]);
  free(string);
  bool is_first = true;
  for (int j = 0; j < strlen(new_string);) {
    if (new_string[j] == ' ' && is_first) {
      is_first = false;
      j++;
    } else if (new_string[j] == ' ' && !is_first) {
      new_string = removeCharAtPos(new_string, j);
    } else {
      is_first = true;
      j++;
    }
  }
  return new_string;
}

void free_string_array(string_array* arr) {
  if (arr->values == NULL)
    return;
  for (int i = 0; i < arr->len; i++) {
    free(arr->values[i]);
    arr->values[i] = NULL;
  }
  free(arr->values);
  arr->values = NULL;
}

void moveCursor(coordinates new_pos) { printf("\033[%d;%dH", new_pos.y, new_pos.x); }

coordinates getTerminalSize() {
  coordinates size;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  size.x = w.ws_col;
  size.y = w.ws_row;

  return size;
}

bool inArray(char* value, string_array array) {
  for (int i = 0; i < array.len; i++) {
    if (strcmp(value, array.values[i]) == 0) {
      return true;
    }
  }
  return false;
}

string_array removeDuplicates(string_array* matching_commands) {
  int j = 0;
  string_array no_dup_array;
  no_dup_array.values = calloc(matching_commands->len, sizeof(char*));
  no_dup_array.len = 0;

  for (int i = 0; i < matching_commands->len; i++) {
    if (!inArray(matching_commands->values[i], no_dup_array)) {
      no_dup_array.values[j] = calloc(strlen(matching_commands->values[i]) + 1, sizeof(char));
      strcpy(no_dup_array.values[j], matching_commands->values[i]);
      no_dup_array.len += 1;
      j++;
    }
  }
  free_string_array(matching_commands);

  return no_dup_array;
}

char* removeCharAtPos(char* line, int x_pos) {
  for (int i = x_pos - 1; i < strlen(line); i++) {
    line[i] = line[i + 1];
  }
  return line;
}

void backspaceLogic(char* line, int* i) {
  if (strlen(line) > 0 && *i >= 0) {
    line = removeCharAtPos(line, *i);

    if (*i > 0) {
      *i -= 1;
    }
  }
}

void logger(enum logger_type type, void* message) {
  FILE* logfile = fopen("log.txt", "a");

  switch (type) {
  case integer: {
    fprintf(logfile, "%d", *((int*)message));
    break;
  }
  case string: {
    fprintf(logfile, "%s", (char*)message);
    break;
  }
  case character: {
    fprintf(logfile, "%c", *(char*)message);
    break;
  }
  default: {
    break;
  }
  }
  fclose(logfile);
}

coordinates getCursorPos() {
  char buf[30] = {0};
  int ret, i, pow;
  char ch;
  coordinates cursor_pos = {.x = 0, .y = 0};

  struct termios term, restore;

  tcgetattr(0, &term);
  tcgetattr(0, &restore);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &term);

  write(1, "\033[6n", 4);

  for (i = 0, ch = 0; ch != 'R'; i++) {
    ret = read(0, &ch, 1);
    if (!ret) {
      tcsetattr(0, TCSANOW, &restore);
      fprintf(stderr, "psh: error parsing cursor-position\n");
      return cursor_pos;
    }
    buf[i] = ch;
  }

  if (i < 2) {
    tcsetattr(0, TCSANOW, &restore);
    return cursor_pos;
  }

  for (i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
    cursor_pos.x += (buf[i] - '0') * pow;

  for (i--, pow = 1; buf[i] != '['; i--, pow *= 10)
    cursor_pos.y += (buf[i] - '0') * pow;

  tcsetattr(0, TCSANOW, &restore);
  return cursor_pos;
}

string_array getAllFilesInDir(string_array* directory_array) {
  struct dirent* file;
  string_array all_path_files;
  char** files = calloc(BUFFER, sizeof(char*));
  int j = 0;
  size_t size = BUFFER * sizeof(char*);

  for (int i = 0; i < directory_array->len; i++) {
    if (isDirectory(directory_array->values[i])) {
      DIR* dr = opendir(directory_array->values[i]);

      while ((file = readdir(dr)) != NULL) {
        if ((j * sizeof(char*)) >= size) {
          char** tmp;
          if ((tmp = realloc(files, size * 1.5)) == NULL) {
            perror("realloc");
          } else {
            files = tmp;
            size *= 1.5;
          }
        }
        files[j] = (char*)calloc(strlen(file->d_name) + 1, sizeof(char));
        strcpy(files[j], file->d_name);
        j++;
      }
      closedir(dr);
    }
  }
  all_path_files.values = files;
  all_path_files.len = j;

  // free_string_array(directory_array);
  return all_path_files;
}

string_array filterMatching(char* line, const string_array PATH_BINS) {
  int buf_size = 64;
  size_t size = buf_size * sizeof(char*);
  char** matching_binaries = calloc(buf_size, sizeof(char*));
  string_array result;
  int j = 0;

  for (int i = 0; i < PATH_BINS.len; i++) {
    if (strncmp(PATH_BINS.values[i], line, strlen(line)) == 0) {
      if ((j * sizeof(char*)) >= size) {
        char** tmp;
        if ((tmp = realloc(matching_binaries, size * 1.5)) == NULL) {
          perror("realloc");
        } else {
          matching_binaries = tmp;
          size *= 1.5;
        }
      }
      matching_binaries[j] = calloc(strlen(PATH_BINS.values[i]) + 1, sizeof(char));
      strcpy(matching_binaries[j], PATH_BINS.values[i]);
      j++;
    }
  }
  result.values = matching_binaries;
  result.len = j;

  return result;
}

string_array getAllMatchingFiles(char* current_dir_sub, char* removed_sub) {
  char* temp_sub = calloc(strlen(current_dir_sub) + 1, sizeof(char));
  strcpy(temp_sub, current_dir_sub);

  string_array current_dir_array = {.len = 1, .values = &temp_sub};
  string_array all_files_in_dir = getAllFilesInDir(&current_dir_array);
  string_array filtered = filterMatching(removed_sub, all_files_in_dir);

  free_string_array(&all_files_in_dir);
  free(temp_sub);

  return filtered;
}

// doesnt allocate necessary memory
bool insertCharAtPos(char* line, int index, char c) {
  int len = strlen(line);
  if (index >= 0 && index <= strlen(line)) {
    for (int i = strlen(line) - 1; i >= index; i--) {
      line[i + 1] = line[i];
    }
    line[index] = c;
    line[len + 1] = '\0';
  } else {
    return false;
  }
  return true;
}

void insertStringAtPos(char** line, char* insert_string, int position) {
  if (strcmp(insert_string, "") == 0)
    return;
  char* tmp;
  if ((tmp = realloc(*line, (strlen(*line) + strlen(insert_string) + 2) * sizeof(char))) == NULL) {
    perror("realloc");
  } else {
    *line = tmp;
  }
  insertCharAtPos(*line, position, '%');
  insertCharAtPos(*line, position + 1, 's');

  char* new_line = calloc(strlen(*line) + strlen(insert_string) + 1, sizeof(char));
  sprintf(new_line, *line, insert_string);
  strcpy(*line, new_line);
  free(new_line);
}

int isDirectory(const char* path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;

  return S_ISDIR(statbuf.st_mode);
}

string_array copyStringArray(string_array arr) {
  string_array copy = {.len = arr.len, .values = calloc(arr.len, sizeof(char*))};
  for (int i = 0; i < arr.len; i++) {
    copy.values[i] = calloc(strlen(arr.values[i]) + 1, sizeof(char));
    strcpy(copy.values[i], arr.values[i]);
  }

  return copy;
}

file_string_tuple getFileStrings(char* current_word, char* current_path) {
  char* current_dir;
  char* removed_sub;

  switch (current_word[0]) {
  case '/': {
    current_dir = current_word;
    if (strlen(current_dir) == 1) {
      removed_sub = "";
    } else {
      removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir, '/')]); // c_e
    }
    break;
  }
  case '~': {
    char* home_path = getenv("HOME"); // Users/username
    strcpy(current_path, home_path);

    current_dir = strcat(current_path, &(current_word[1])); // Users/username/documents
    removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir, '/')]); // documents
    break;
  }
  default: {
    current_dir = strcat(current_path, current_word); // documents/coding/c_e
    removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir, '/')]); // c_e
    break;
  }
  }

  return (file_string_tuple){.removed_sub = removed_sub, .current_dir = current_dir};
}

int getAppendingIndex(char* line, char delimeter) {
  int j = 0;
  for (int i = strlen(line) - 1; i >= 0; i--) {
    if (line[i] == delimeter)
      return j;
    j++;
  }
  return -1;
}

void fileDirArray(string_array* filtered, char* current_dir_sub) {
  char* current_dir_sub_copy =
      calloc(strlen(current_dir_sub) + getLongestWordInArray(*filtered) + 2, sizeof(char));
  char* temp;
  char copy[strlen(current_dir_sub) + getLongestWordInArray(*filtered) + 2];
  strcat(current_dir_sub, "/");

  for (int i = 0; i < filtered->len; i++) {
    strcpy(current_dir_sub_copy, current_dir_sub);

    temp = strcpy(copy, strcat(current_dir_sub_copy, filtered->values[i]));

    if (isDirectory(temp)) {
      filtered->values[i] = realloc(filtered->values[i], strlen(filtered->values[i]) + 2);
      filtered->values[i][strlen(filtered->values[i]) + 1] = '\0';
      filtered->values[i][strlen(filtered->values[i])] = '/';
    }
    memset(copy, 0, strlen(copy));
    memset(temp, 0, strlen(temp));
    memset(current_dir_sub_copy, 0, strlen(current_dir_sub_copy));
  }
  free(current_dir_sub_copy);
  free(current_dir_sub);
}

int getCurrentWordPosInLine(string_array command_line, char* word) {
  for (int i = 0; i < command_line.len; i++) {
    if (strncmp(command_line.values[i], word, strlen(word)) == 0) {
      return i;
    }
  }

  return -1;
}

int countWhitespace(char* string) {
  int result = 0;
  for (int i = 0; i < strlen(string); i++) {
    if (string[i] == ' ')
      result++;
  }
  return result;
}

autocomplete_array fileComp(char* current_word) {
  char cd[PATH_MAX];
  file_string_tuple file_strings = getFileStrings(current_word, strcat(getcwd(cd, sizeof(cd)), "/"));

  char* current_dir_sub = calloc(strlen(file_strings.current_dir) + 2, sizeof(char));
  strncpy(current_dir_sub, file_strings.current_dir,
          strlen(file_strings.current_dir) - getAppendingIndex(file_strings.current_dir, '/'));
  string_array filtered = getAllMatchingFiles(current_dir_sub, file_strings.removed_sub);

  fileDirArray(&filtered, current_dir_sub);
  // have to take escapes into account which offset the original appending_index
  int offset = countWhitespace(file_strings.removed_sub);

  return (autocomplete_array){.array.values = filtered.values,
                              .array.len = filtered.len,
                              .appending_index = strlen(file_strings.removed_sub) + offset};
}

coordinates calculateCursorPos(coordinates terminal_size, coordinates cursor_pos, int prompt_len, int i) {
  int line_pos = i + prompt_len;

  if (line_pos < terminal_size.x) {
    return (coordinates){.x = line_pos, .y = cursor_pos.y};
  } else if (line_pos % terminal_size.x == 0) {
    return (coordinates){.x = terminal_size.x, .y = cursor_pos.y + ((line_pos - 1) / terminal_size.x)};
  } else {
    return (coordinates){.x = line_pos % terminal_size.x, .y = cursor_pos.y + (line_pos / terminal_size.x)};
  }
}

int calculateRowCount(coordinates terminal_size, int prompt_len, int i) {
  return calculateCursorPos(terminal_size, (coordinates){0, 0}, prompt_len, i).y;
}

char* shortenIfTooLong(char* word, int terminal_width) {
  char* result = calloc(strlen(word) + 1, sizeof(char));
  strcpy(result, word);
  if (strlen(word) > terminal_width - 2) {
    result[terminal_width - 2] = '\0';
    for (int j = terminal_width - 3; j > (terminal_width - 6); j--) {
      result[j] = '.';
    }
  }
  return result;
}

int firstNonDelimeterIndex(string_array splitted_line) {
  for (int i = 0; i < splitted_line.len; i++) {
    if (strcmp(splitted_line.values[i], "") != 0) {
      return i;
    }
  }
  return 0;
}

int getLongestWordInArray(const string_array array) {
  int longest = 0;
  int current_len = 0;

  for (int i = 0; i < array.len; i++) {
    current_len = strlen(array.values[i]);
    if (current_len > longest) {
      longest = current_len;
    }
  }

  return longest;
}

bool isExec(char* file) {
  if (access(file, F_OK | X_OK) == 0 && !isDirectory(file)) {
    return true;
  }
  return false;
}

token_index getCurrentToken(int line_index, token_index_arr tokenized_line) {
  // when current index greater than existant line len take last element
  token_index result = {.start = tokenized_line.arr[tokenized_line.len - 1].start,
                        .end = tokenized_line.arr[tokenized_line.len - 1].end};
  for (int i = 0; i < tokenized_line.len; i++) {
    if (line_index >= tokenized_line.arr[i].start && line_index <= tokenized_line.arr[i].end) {
      result = tokenized_line.arr[i];
      break;
    }
  }
  return result;
}

void removeEscapesString(char** string) {
  for (int j = 0; j < strlen(*string); j++) {
    if ((*string)[j] == '\\') {
      *string = removeCharAtPos(*string, j + 1);
    }
  }
}

void removeSlice(char** line, int start, int end) {
  for (int i = start; i < end; i++) {
    *line = removeCharAtPos(*line, start + 1);
  }
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
  regmatch_t rm[ENUM_LEN];
  char* quoted_args = "(\'[^\']*\')";
  char* whitespace = "(\\\\[ ])|([ ]+)";
  char* filenames = "([12]?>{2}|[12]?>|<|&>|&>>)[\t]*([^\n\t><]+)";
  char* redirection = "([12]?>{2})|([12]?>)|(<)|(&>)|(&>>)";
  char* line_token = "^[\n\t]*([^\n\t\f|&&]+)|\\|[\t\n]*([^\n\t\f|&&]+)|(\\|)|(&&)|&&["
                     "\t\n]*([^\n\t\f|&&]+)";
  char* wildcards = "(\\*)|(\\?)";
  char* only_args = "([^\t\n\f]+)";
  char* regexes[] = {quoted_args, whitespace, filenames, redirection, line_token, wildcards, only_args};
  regex_loop_struct regex_info[] = {{.fill_char = '\n', .loop_start = 1, .token_index_inc = 13}, // quoted_args
                                    {.fill_char = '\t', .loop_start = 1, .token_index_inc = 9},  // whitespace
                                    {.fill_char = '\n', .loop_start = 2, .token_index_inc = 12}, // filenames(args)
                                    {.fill_char = '\n', .loop_start = 1, .token_index_inc = 5},  // redirection
                                    {.fill_char = '\f', .loop_start = 1, .token_index_inc = 0},  // line-token
                                    {.fill_char = '\t', .loop_start = 1, .token_index_inc = 11}, // wildcards
                                    {.fill_char = '\t', .loop_start = 1, .token_index_inc = 13}}; // args
  char* start;
  char* end;
  int j = 0;

  token_index* result_arr = calloc(strlen(line), sizeof(token_index));

  for (int k = 0; k < (sizeof(regexes) / sizeof(regexes[0])); k++) {
    if (regcomp(&re, regexes[k], REG_EXTENDED) != 0) {
      perror("Error in compiling regex\n");
    }
    if (k == 2) {
      // change back escaped whitespaces
      for (int l = 0; l < strlen(copy); l++) {
        if (copy[l] == '\f') {
          removeCharAtPos(copy, l + 1);
          removeCharAtPos(copy, l + 1);
          insertStringAtPos(&copy, "\\ ", l);
        }
      }
    }
    for (; j < strlen(copy); j++) {
      if ((retval = regexec(&re, copy, ENUM_LEN, rm, 0)) == 0) {
        for (int i = regex_info[k].loop_start; i < ENUM_LEN; i++) {
          if (rm[i].rm_so != -1) {
            start = copy + rm[i].rm_so;
            end = copy + rm[i].rm_eo;
            if (k == 1 && i == 1) {
              // matched escape-whitespace
              while (start < end) {
                *start++ = '\f'; // overwrite with random char to recognize in next regex and change back
              }
              j--;
            } else {
              result_arr[j].start = rm[i].rm_so;
              result_arr[j].end = rm[i].rm_eo;
              result_arr[j].token = i + regex_info[k].token_index_inc;
              while (start < end) {
                *start++ = regex_info[k].fill_char; // have to overwrite matches so they dont match again
              }
            }
            break;
          }
        }
      } else {
        break;
      }
    }
    regfree(&re);
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
    if ((*line)[j] == '~') {
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

void printPrompt(const char* dir, color color) {
  printColor(dir, color, bold);
  printf(" ");
  printColor("\u2771 ", GREEN, standard);
}
