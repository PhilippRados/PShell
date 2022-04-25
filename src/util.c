#include "util.h"

void clean_stdin(void) {
  int stdin_copy = dup(STDIN_FILENO);
  tcdrain(stdin_copy);
  tcflush(stdin_copy, TCIFLUSH);
  close(stdin_copy);
}

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

string_array splitString(const char* string_to_split, char delimeter) {
  int start = 0;
  int j = 0;
  char** splitted_strings = (char**)calloc(strlen(string_to_split), sizeof(char*));
  string_array result;
  if (isOnlyDelimeter(string_to_split, delimeter)) {
    splitted_strings[0] = calloc(strlen(string_to_split) + 1, sizeof(char));
    strcpy(splitted_strings[0], string_to_split);
    return (string_array){.len = 1, .values = splitted_strings};
  }

  for (int i = 0;; i++) {
    if (string_to_split[i] == delimeter || string_to_split[i] == '\0') {
      splitted_strings[j] = (char*)calloc(i - start + 1, sizeof(char));
      memcpy(splitted_strings[j], &string_to_split[start], i - start);
      start = i + 1;
      j++;
    }
    if (string_to_split[i] == '\0')
      break;
  }
  result.len = j;
  result.values = splitted_strings;
  return result;
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
  if (strlen(line) > 0 && i >= 0) {
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
  char buf[1];
  char data[64];
  int y, x;
  char cmd[] = "\033[6n";
  coordinates cursor_pos = {.x = -1, .y = -1};
  struct termios oldattr, newattr;

  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);
  newattr.c_cflag &= ~(CREAD);
  tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

  write(STDIN_FILENO, cmd, sizeof(cmd));
  read(STDIN_FILENO, buf, 1);

  if (*buf == '\033') {
    read(STDIN_FILENO, buf, 1);
    if (*buf == '[') {
      read(STDIN_FILENO, buf, 1);
      for (int i = 0; *buf != 'R'; i++) {
        data[i] = *buf;
        read(STDIN_FILENO, buf, 1);
      }
      // check if string matches expected data
      int valid = sscanf(data, "%d;%d", &y, &x);
      if (valid == 2) {
        cursor_pos.x = x;
        cursor_pos.y = y;
      }
    }
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
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

int getWordEndIndex(char* line, int start) {
  int line_end = start;
  for (; line[start] != '\0' && line[start] != ' '; start++)
    line_end++;

  return line_end;
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

void fileDirArray(string_array* filtered, char* current_dir_sub, char* removed_sub) {
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
autocomplete_array fileComp(char* current_word) {
  char cd[PATH_MAX];
  file_string_tuple file_strings = getFileStrings(current_word, strcat(getcwd(cd, sizeof(cd)), "/"));

  char* current_dir_sub = calloc(strlen(file_strings.current_dir) + 2, sizeof(char));
  strncpy(current_dir_sub, file_strings.current_dir,
          strlen(file_strings.current_dir) - getAppendingIndex(file_strings.current_dir, '/'));
  string_array filtered = getAllMatchingFiles(current_dir_sub, file_strings.removed_sub);

  fileDirArray(&filtered, current_dir_sub, file_strings.removed_sub);

  return (autocomplete_array){.array.values = filtered.values,
                              .array.len = filtered.len,
                              .appending_index = strlen(file_strings.removed_sub)};
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
  token_index result = {.start = -1, .end = -1};
  for (int i = 0; i < tokenized_line.len; i++) {
    if (line_index >= tokenized_line.arr[i].start && line_index <= tokenized_line.arr[i].end) {
      result = tokenized_line.arr[i];
      break;
    }
  }
  return result;
}
