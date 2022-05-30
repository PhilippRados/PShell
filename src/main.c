#include "main.h"

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

char* joinFilePath(char* home_dir, char* destination_file) {
  char* home_dir_copied = calloc(strlen(home_dir) + strlen(destination_file) + 1, sizeof(char));
  strcpy(home_dir_copied, home_dir);

  char* file_path = strcat(home_dir_copied, destination_file);

  return file_path;
}

void removeEscapesArr(string_array* splitted) {
  for (int i = 0; i < splitted->len; i++) {
    removeEscapesString(&splitted->values[i]);
  }
}

void pipeOutputToFile(char* filename) {
  int file = open(filename, O_WRONLY | O_CREAT, 0777);

  int file2 = dup2(file, STDOUT_FILENO);
  close(file);
}

wildcard_groups_arr groupWildcards(char* line, token_index_arr token) {
  wildcard_groups* result = calloc(token.len, sizeof(wildcard_groups));
  int wildcard_index = 0;

  for (int i = 0; i < token.len;) {
    if (token.arr[i].token == ASTERISK || token.arr[i].token == QUESTION) {
      int start = token.arr[i - 1].token == ARG ? token.arr[i - 1].start : token.arr[i].start;
      int j = i;

      for (; (j + 1) < token.len && (token.arr[j + 1].token == ARG || token.arr[j + 1].token == ASTERISK ||
                                     token.arr[j + 1].token == QUESTION);
           j++)
        ;
      int end_index = token.arr[j].end;

      result[wildcard_index].wildcard_arg = calloc(end_index - start + 1, sizeof(char));
      strncpy(result[wildcard_index].wildcard_arg, &line[start], end_index - start);
      result[wildcard_index].start = start;
      result[wildcard_index].end = start + strlen(result[wildcard_index].wildcard_arg);

      wildcard_index++;
      i = j + 1;
    }
    i++;
  }
  return (wildcard_groups_arr){.arr = result, .len = wildcard_index};
}

bool isLastRedirectionInLine(char* line, int current_pos) {
  for (int i = 0; i < strlen(line); i++) {
    if (line[i] == '*' || line[i] == '?')
      return false;
  }
  return true;
}

char* createRegex(char* regex, char* start, char* end) {
  char* regex_start = regex;
  *regex++ = '^';
  while (start < end) {
    if (*start == '*') {
      *regex++ = '.';
      *regex++ = '*';
      start++;
    } else if (*start == '?') {
      *regex++ = '.';
      start++;
    } else if (*start == '.') {
      *regex++ = '\\';
      *regex++ = '.';
      start++;
    } else {
      *regex++ = *start++;
    }
  }
  *regex++ = '$';

  return regex_start;
}

int calculateEndIndex(wildcard_groups_arr wildcard_groups, int j, int i) {
  int end_index = j + 1;

  for (; end_index < strlen(wildcard_groups.arr[i].wildcard_arg) &&
         wildcard_groups.arr[i].wildcard_arg[end_index] != '/' &&
         wildcard_groups.arr[i].wildcard_arg[end_index] != ' ';
       end_index++)
    ;

  return end_index;
}

void insertIfMatch(wildcard_groups_arr* wildcard_groups, char* prefix, DIR* current_dir, regex_t* re,
                   int concat_index, bool is_dotfile, int i, int j) {
  struct dirent* dir;
  int start_index = 0;

  while ((dir = readdir(current_dir)) != NULL) {
    if (regexec(re, dir->d_name, 0, NULL, 0) == 0) {
      if (dir->d_name[0] == '.' && !is_dotfile) {
        continue;
      }
      for (int j = 0; j < strlen(dir->d_name); j++) {
        if (dir->d_name[j] == ' ') {
          // insert escape \\ in front of whitespace
          insertCharAtPos(dir->d_name, j, '\\');
          j++;
        }
      }
      char* prev_copy = calloc(strlen(prefix) + strlen(dir->d_name) + 1, sizeof(char));
      strcpy(prev_copy, prefix);
      char* match = strcat(&prev_copy[concat_index], dir->d_name);

      if (strlen(wildcard_groups->arr[i].wildcard_arg) == 0) {
        wildcard_groups->arr[i].wildcard_arg =
            realloc(wildcard_groups->arr[i].wildcard_arg, (strlen(match) + 1) * sizeof(char));
        strcpy(wildcard_groups->arr[i].wildcard_arg, match);
      } else {
        insertStringAtPos(&wildcard_groups->arr[i].wildcard_arg, match, start_index);
      }

      if (isLastRedirectionInLine(wildcard_groups->arr[i].wildcard_arg, j)) {
        start_index = strlen(wildcard_groups->arr[i].wildcard_arg);
        wildcard_groups->arr[i].wildcard_arg =
            realloc(wildcard_groups->arr[i].wildcard_arg,
                    (strlen(wildcard_groups->arr[i].wildcard_arg) + 2) * sizeof(char));
        insertCharAtPos(wildcard_groups->arr[i].wildcard_arg, start_index, ' ');
        start_index++;
      }
      free(prev_copy);
    }
  }
}

int calculateConcatIndex(char* prefix) {
  int concat_index = 0;
  for (; concat_index < strlen(prefix) && prefix[concat_index] != '/'; concat_index++)
    ;
  concat_index += (prefix[0] == '/') ? 0 : 1;

  return concat_index;
}

int calculatePrefixEnd(wildcard_groups_arr wildcard_groups, int j, int i) {
  int prefix_end = j;
  for (; prefix_end > 0 && wildcard_groups.arr[i].wildcard_arg[prefix_end - 1] != '/'; prefix_end--)
    ;
  return prefix_end;
}

void getPrefix(wildcard_groups_arr wildcard_groups, char* prefix, int i, int prefix_end) {
  if (wildcard_groups.arr[i].wildcard_arg[0] == '/') {
    strncpy(prefix, wildcard_groups.arr[i].wildcard_arg, prefix_end);
  } else {
    strcpy(prefix, "./");
    strncpy(&prefix[2], wildcard_groups.arr[i].wildcard_arg, prefix_end);
  }
}

wildcard_groups_arr expandWildcardgroups(wildcard_groups_arr wildcard_groups) {
  for (int i = 0; i < wildcard_groups.len; i++) {
    for (int j = 0; j < strlen(wildcard_groups.arr[i].wildcard_arg); j++) {
      if (wildcard_groups.arr[i].wildcard_arg[j] == '*' || wildcard_groups.arr[i].wildcard_arg[j] == '?') {
        int prefix_end = calculatePrefixEnd(wildcard_groups, j, i);
        char* prefix = calloc(prefix_end + 3, sizeof(char));
        getPrefix(wildcard_groups, prefix, i, prefix_end);

        char* start = &wildcard_groups.arr[i].wildcard_arg[prefix_end];
        bool is_dotfile = start[0] == '.' ? true : false;

        int end_index = calculateEndIndex(wildcard_groups, j, i);
        char* end = &wildcard_groups.arr[i].wildcard_arg[end_index];

        char* regex = calloc(((end - start) * 2) + 3, sizeof(char));
        regex = createRegex(regex, start, end);
        regex_t re;
        if (regcomp(&re, regex, REG_EXTENDED) != 0) {
          perror("regex");
        }

        removeSlice(&wildcard_groups.arr[i].wildcard_arg, 0, end_index);
        int concat_index = calculateConcatIndex(prefix);

        DIR* current_dir = opendir(prefix);
        if (current_dir == NULL) {
          // when wrong dir make wildcard empty so that foundAllWildcards is false
          fprintf(stderr, "psh: couldn't open directory: %s\n", prefix);
          strcpy(wildcard_groups.arr[0].wildcard_arg, "");
          return wildcard_groups;
        }

        insertIfMatch(&wildcard_groups, prefix, current_dir, &re, concat_index, is_dotfile, i, j);

        free(regex);
        free(prefix);
        regfree(&re);
        closedir(current_dir);
      }
    }
  }
  return wildcard_groups;
}

void replaceLineWithWildcards(char** line, wildcard_groups_arr wildcard_matches) {
  int verschoben_len = 0;
  for (int i = 0; i < wildcard_matches.len; i++) {
    removeSlice(line, wildcard_matches.arr[i].start + verschoben_len,
                wildcard_matches.arr[i].end + verschoben_len);
    insertStringAtPos(line, wildcard_matches.arr[i].wildcard_arg, wildcard_matches.arr[i].start + verschoben_len);
    verschoben_len += strlen(wildcard_matches.arr[i].wildcard_arg) -
                      (wildcard_matches.arr[i].end - wildcard_matches.arr[i].start);
  }
}

int runChildProcess(string_array splitted_line) {
  pid_t pid = fork();

  if (pid == 0) {
    int error = execvp(splitted_line.values[0], splitted_line.values);
    if (error) {
      fprintf(stderr, "couldn't find command %s\n", splitted_line.values[0]);
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
  char* file_path = joinFilePath(getenv("HOME"), "/.psh_history");
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

void writeCommandToGlobalHistory(char* cmd, string_array global_history) {
  char* file_path = joinFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_write = fopen(file_path, "a");
  free(file_path);

  if (file_to_write == NULL) {
    fprintf(stderr, "psh: can't open ~/.psh_history\n");
    return;
  }

  if (!inArray(cmd, global_history)) {
    fprintf(file_to_write, "%s\n", cmd);
  }

  fclose(file_to_write);
}

void cd(string_array splitted_line, char* current_dir, char* last_two_dirs, char* dir) {
  if (splitted_line.len == 2) {
    if (chdir(splitted_line.values[1]) == -1) {
      fprintf(stderr, "cd: %s: no such file or directory\n", splitted_line.values[1]);
    }
  } else if (splitted_line.len > 2) {
    fprintf(stderr, "cd: too many arguments\n");
  } else {
    printf("usage: cd [path]\n");
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

void removeWhitespaceTokens(token_index_arr* tokenized_line) {
  for (int i = 0; i < tokenized_line->len;) {
    if (tokenized_line->arr[i].token == WHITESPACE) {
      for (int j = i; j < tokenized_line->len - 1; j++) {
        tokenized_line->arr[j] = tokenized_line->arr[j + 1];
      }
      tokenized_line->len--;
    } else {
      i++;
    }
  }
}
char* convertTokenToString(token_index_arr tokenized_line) {
  char* result = calloc(tokenized_line.len * 2 + 1, sizeof(char));
  int string_index = 0;
  for (int i = 0; i < tokenized_line.len; i++) {
    // ignoring wildcards since they can be anywhere
    if (tokenized_line.arr[i].token != ASTERISK && tokenized_line.arr[i].token != QUESTION) {
      sprintf(&result[string_index], "%d", tokenized_line.arr[i].token);
      tokenized_line.arr[i].token >= 10 ? string_index += 2 : string_index++;
    }
  }
  return result;
}

bool isValidSyntax(token_index_arr tokenized_line) {
  char* string_rep = convertTokenToString(tokenized_line);
  bool result = false;
  regex_t re;
  regmatch_t rm[1];
  // nums represent token enum values from types.h
  char* valid_syntax = "^((6|7|8|9|10)14)*(1((6|7|8|9|10)14)*(14)*((6|7|8|9|10)14)*((3((6|7|8|9|10)14)*2)(("
                       "6|7|8|9|10)14)*(14)*((6|7|8|9|10)14)*|((4((6|7|8|9|10)14)*5)|(4((6|7|8|9|10)14)+))"
                       "((6|7|8|9|10)14)*(14)*((6|7|8|9|10)14)*)*)?";

  if (regcomp(&re, valid_syntax, REG_EXTENDED) != 0) {
    perror("psh:");
  }
  if (regexec(&re, string_rep, 1, rm, 0) == 0 && rm->rm_eo - rm->rm_so == strlen(string_rep)) {
    result = true;
  } else {
    result = false;
  }
  regfree(&re);

  free(string_rep);
  return result;
}

string_array_token splitLineIntoSimpleCommands(char* line, token_index_arr tokenized_line) {
  char** line_arr = calloc(tokenized_line.len, sizeof(char*));
  enum token* token_arr = calloc(tokenized_line.len, sizeof(enum token));
  int j = 0;
  bool found_split = true;
  int start = 0;
  int i;
  token_arr[0] = CMD;
  for (i = 0; i < tokenized_line.len; i++) {
    if (found_split) {
      start = tokenized_line.arr[i].start;
      found_split = false;
    }
    if (tokenized_line.arr[i].token == PIPE) {
      int end = tokenized_line.arr[i].start;
      line_arr[j] = calloc(end - start + 1, sizeof(char));
      strncpy(line_arr[j], &line[start], end - start);
      token_arr[j + 1] = PIPE_CMD;
      j++;
      found_split = true;
    } else if (tokenized_line.arr[i].token == AMPAMP) {
      int end = tokenized_line.arr[i].start;
      line_arr[j] = calloc(end - start + 1, sizeof(char));
      strncpy(line_arr[j], &line[start], end - start);
      token_arr[j + 1] = AMP_CMD;
      j++;
      found_split = true;
    }
  }
  int end = strlen(line);
  line_arr[j] = calloc(end - start + 1, sizeof(char));
  strncpy(line_arr[j], &line[start], end - start);

  string_array_token result = {.values = line_arr, .len = j + 1, .token_arr = token_arr};
  return result;
}

string_array splitByTokens(char* line, token_index_arr token) {
  char** line_arr = calloc(token.len + 1, sizeof(char*));

  int start;
  int end;
  for (int i = 0; i < token.len; i++) {
    if (token.arr[i].token == ARG && line[token.arr[i].start] == '\'' && line[token.arr[i].end - 1] == '\'') {
      start = token.arr[i].start + 1;
      end = token.arr[i].end - 1;
    } else {
      start = token.arr[i].start;
      end = token.arr[i].end;
    }
    line_arr[i] = calloc(end - start + 1, sizeof(char));
    strncpy(line_arr[i], &line[start], end - start);
  }
  line_arr[token.len] = NULL;
  string_array result = {.values = line_arr, .len = token.len};
  // free(token.arr);
  return result;
}

file_redirection_data parseForRedirectionFiles(string_array simple_command, token_index_arr token) {
  char* output_name = NULL;
  char* input_name = NULL;
  char* error_name = NULL;
  char* merge_name = NULL;
  bool output_append = false;
  bool error_append = false;
  bool merge_append = false;
  bool found_output = false;
  bool found_input = false;
  bool found_stderr = false;
  bool found_merge = false;

  for (int j = token.len - 2; j >= 0; j--) {
    if (!found_input && token.arr[j].token == LESS) {
      input_name = calloc(strlen(simple_command.values[j + 1]) + 1, sizeof(char));
      strcpy(input_name, simple_command.values[j + 1]);
      removeEscapesString(&input_name);
      found_input = true;
    }
    if (!found_stderr && !found_output && !found_merge &&
        (token.arr[j].token == AMP_GREAT || token.arr[j].token == AMP_GREATGREAT)) {
      merge_name = calloc(strlen(simple_command.values[j + 1]) + 1, sizeof(char));
      strcpy(merge_name, simple_command.values[j + 1]);
      removeEscapesString(&merge_name);
      found_stderr = true;
      found_output = true;
      found_merge = true;
      merge_append = token.arr[j].token == AMP_GREATGREAT ? true : false;
    }
    if (token.arr[j].token == GREAT || token.arr[j].token == GREATGREAT) {
      if (!found_stderr && simple_command.values[j][0] == '2') {
        error_name = calloc(strlen(simple_command.values[j + 1]) + 1, sizeof(char));
        strcpy(error_name, simple_command.values[j + 1]);
        removeEscapesString(&error_name);
        found_stderr = true;
        error_append = token.arr[j].token == GREATGREAT ? true : false;
      }
      if (!found_output && simple_command.values[j][0] != '2') {
        output_name = calloc(strlen(simple_command.values[j + 1]) + 1, sizeof(char));
        strcpy(output_name, simple_command.values[j + 1]);
        removeEscapesString(&output_name);
        found_output = true;
        output_append = token.arr[j].token == GREATGREAT ? true : false;
      }
    }
  }

  return (file_redirection_data){.output_filename = output_name,
                                 .input_filename = input_name,
                                 .output_append = output_append,
                                 .stderr_filename = error_name,
                                 .stderr_append = error_append,
                                 .merge_filename = merge_name,
                                 .merge_append = merge_append};
}

bool fileExists(char* name) {
  if (access(name, 0) == 0) {
    return true;
  } else {
    return false;
  }
}

void removeArrayElement(string_array* splitted, int pos) {
  for (int i = pos; i < splitted->len; i++) {
    splitted->values[i] = splitted->values[i + 1];
  }
  splitted->len--;
}

void stripRedirections(string_array* splitted_line, token_index_arr token) {
  int j = 0;
  int len = splitted_line->len;
  for (int i = 0; i < len;) {
    if (token.arr[i].token >= GREATGREAT && token.arr[i].token <= AMP_GREATGREAT) {
      removeArrayElement(splitted_line, j); // removes redirection
      removeArrayElement(splitted_line, j); // removes filename
      i += 2;
    } else {
      j++;
      i++;
    }
  }
}

void replaceAliases(char** line, int len) {
  for (int i = 0; i < len; i++) {
    replaceAliasesString(&line[i]);
  }
}

void free_wildcard_groups(wildcard_groups_arr arr) {
  for (int i = 0; i < arr.len; i++) {
    free(arr.arr[i].wildcard_arg);
  }
  free(arr.arr);
}

bool foundAllWildcards(wildcard_groups_arr arr) {
  for (int i = 0; i < arr.len; i++) {
    if (strcmp(arr.arr[i].wildcard_arg, "") == 0) {
      free_wildcard_groups(arr);
      return false;
    }
  }
  free_wildcard_groups(arr);
  return true;
}

bool replaceWildcards(char** line) {
  token_index_arr token = tokenizeLine(*line);
  wildcard_groups_arr groups = groupWildcards(*line, token);
  wildcard_groups_arr wildcard_matches = expandWildcardgroups(groups);
  replaceLineWithWildcards(line, wildcard_matches);

  free(token.arr);

  return foundAllWildcards(wildcard_matches);
}

void free_string_array_token(string_array_token simple_commands) {
  for (int i = 0; i < simple_commands.len; i++) {
    free(simple_commands.values[i]);
  }
  free(simple_commands.values);
  free(simple_commands.token_arr);
}

void free_file_info(file_redirection_data file_info) {
  free(file_info.input_filename);
  free(file_info.output_filename);
  free(file_info.stderr_filename);
  free(file_info.merge_filename);
}

void resetIO(int* tmpin, int* tmpout, int* tmperr) {
  dup2(*tmpin, 0);
  dup2(*tmpout, 1);
  dup2(*tmperr, 2);
  close(*tmpin);
  close(*tmpout);
  close(*tmperr);
  *tmpin = dup(0);
  *tmpout = dup(1);
  *tmperr = dup(2);
}

void outputRedirection(file_redirection_data file_info, int pd[2], int* fdout, int* fdin, int tmpout, int i,
                       string_array_token simple_commands_arr) {
  if (file_info.output_filename != NULL) {
    *fdout = file_info.output_append ? open(file_info.output_filename, O_RDWR | O_CREAT | O_APPEND, 0666)
                                     : open(file_info.output_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
  } else if (i < simple_commands_arr.len - 1 && simple_commands_arr.token_arr[i + 1] == PIPE_CMD) {
    pipe(pd);
    *fdout = pd[1];
    *fdin = pd[0];
  } else {
    *fdout = dup(tmpout);
  }
}

void errorRedirection(file_redirection_data file_info, int* fderr, int tmperr) {
  if (file_info.stderr_filename != NULL) {
    *fderr = file_info.stderr_append ? open(file_info.stderr_filename, O_RDWR | O_CREAT | O_APPEND, 0666)
                                     : open(file_info.stderr_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
  } else {
    *fderr = dup(tmperr);
  }
}

void mergeRedirection(file_redirection_data file_info, int* fdout) {
  *fdout = file_info.merge_append ? open(file_info.merge_filename, O_RDWR | O_CREAT | O_APPEND, 0666)
                                  : open(file_info.merge_filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
  dup2(*fdout, STDOUT_FILENO);
  dup2(*fdout, STDERR_FILENO);
  close(*fdout);
}

string_array getPathBins() {
  string_array PATH_ARR = splitString(getenv("PATH"), ':');
  string_array all_files_in_dir = getAllFilesInDir(&PATH_ARR);
  free_string_array(&PATH_ARR);
  string_array removed_dots = removeDots(&all_files_in_dir);

  return removeDuplicates(&removed_dots);
}

bool wildcardLogic(string_array_token simple_commands_arr, int* fdout, int* fderr, int tmpout, int tmperr, int i) {
  if (strchr(simple_commands_arr.values[i], '*') != NULL || strchr(simple_commands_arr.values[i], '?') != NULL) {
    if (!replaceWildcards(&simple_commands_arr.values[i])) {
      *fdout = dup(tmpout);
      *fderr = dup(tmperr);
      dup2(*fderr, STDERR_FILENO);
      close(*fderr);
      dup2(*fdout, STDOUT_FILENO);
      close(*fdout);
      fprintf(stderr, "psh: no wildcard matches found\n");
      return false;
    }
  }
  return true;
}

bool foundBuiltin(string_array splitted_line, builtins_array BUILTINS, int* builtin_index) {
  return (splitted_line.len > 0 && (*builtin_index = isBuiltin(splitted_line.values[0], BUILTINS)) != -1) ? true
                                                                                                          : false;
}

char* parseVarName(char* buf) {
  char* result = calloc(strlen(buf) + 1, sizeof(char));
  for (int i = 0; i < strlen(buf) && buf[i] != '='; i++) {
    result[i] = buf[i];
  }
  return result;
}

env_var_arr parseRcFileForEnv() {
  char* file_path = joinFilePath(getenv("HOME"), "/.pshrc");
  FILE* file_to_read = fopen(file_path, "r");
  free(file_path);

  if (file_to_read == NULL) {
    fprintf(stderr, "psh: couldn't open rc file");
    exit(0);
  }
  char* buf;
  size_t buf_size;
  size_t line_len;
  char** var_names = calloc(64, sizeof(char*));
  char** values = calloc(64, sizeof(char*));

  int i = 0;
  for (; (line_len = getline(&buf, &buf_size, file_to_read)) != -1; i++) {
    var_names[i] = parseVarName(buf);
    values[i] = calloc(strlen(buf), sizeof(char));
    strncpy(values[i], &buf[strlen(var_names[i]) + 2], strlen(buf) - (strlen(var_names[i]) + 4));
  }

  return (env_var_arr){.len = i, .var_names = var_names, .values = values};
}

void setRcVars(env_var_arr envs) {
  for (int i = 0; i < envs.len; i++) {
    if (envs.values[i][strlen(envs.values[i]) - 1] == '$') {
      // $ Means concat with already existant VAR
      envs.values[i][strlen(envs.values[i]) - 1] = '\0';
      insertCharAtPos(envs.values[i], 0, ':');
      setenv(envs.var_names[i], joinFilePath(getenv(envs.var_names[i]), envs.values[i]), 1);
    } else {
      // can overwrite VAR
      setenv(envs.var_names[i], envs.values[i], 1);
    }
  }
}

volatile sig_atomic_t ctrlc_hit = false;
void ctrlCHandler(int sig) { ctrlc_hit = true; }

#ifndef TEST

int main(int argc, char* argv[]) {
  char* line;
  char dir[PATH_MAX];
  bool loop = true;
  env_var_arr ENV = parseRcFileForEnv();
  setRcVars(ENV);
  string_array command_history = {.len = 0, .values = calloc(HISTORY_SIZE, sizeof(char*))};
  string_array PATH_BINS = getPathBins();
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

  signal(SIGHUP, SIG_DFL); // Stops process when terminal is closed
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));

  while (loop) {
    ctrlc_hit = false;
    printf("\n");
    printPrompt(last_two_dirs, CYAN);

    // change signal flag to exit even in when blocking func
    sa.sa_flags = 0;
    sa.sa_handler = ctrlCHandler;
    sigaction(SIGINT, &sa, NULL);

    line = readLine(PATH_BINS, last_two_dirs, &command_history, global_command_history, BUILTINS);
    if (ctrlc_hit) {
      free(line);
      continue;
    }
    // change signal flag to default behaviour for child process
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = ctrlCHandler;
    sigaction(SIGINT, &sa, NULL);

    token_index_arr tokenized_line = tokenizeLine(line);
    removeWhitespaceTokens(&tokenized_line);

    if (tokenized_line.len > 0 && isValidSyntax(tokenized_line)) {
      string_array_token simple_commands_arr = splitLineIntoSimpleCommands(line, tokenized_line);
      replaceAliases(simple_commands_arr.values, simple_commands_arr.len);

      int pd[2];
      int tmpin = dup(0);
      int tmpout = dup(1);
      int tmperr = dup(2);
      int fdout;
      int fderr;
      pid_t pid;
      int fdin = open(0, O_RDONLY);

      for (int i = 0; i < simple_commands_arr.len; i++) {
        if (!wildcardLogic(simple_commands_arr, &fdout, &fderr, tmpout, tmperr, i)) {
          continue;
        }
        token_index_arr token = tokenizeLine(simple_commands_arr.values[i]);
        removeWhitespaceTokens(&token);
        string_array splitted_line = splitByTokens(simple_commands_arr.values[i], token);
        file_redirection_data file_info = parseForRedirectionFiles(splitted_line, token);
        stripRedirections(&splitted_line, token);
        free(token.arr);
        removeEscapesArr(&splitted_line);
        int builtin_index;

        if (file_info.input_filename != NULL) {
          if (fileExists(file_info.input_filename)) {
            fdin = open(file_info.input_filename, O_RDONLY);
          } else {
            printf("no such file %s\n", file_info.input_filename);
            free_string_array(&splitted_line);
            free_file_info(file_info);
            continue;
          }
        } else if (simple_commands_arr.token_arr[i] == AMP_CMD) {
          resetIO(&tmpin, &tmpout, &tmperr);
        }

        if (foundBuiltin(splitted_line, BUILTINS, &builtin_index)) {
          if (!callBuiltin(splitted_line, BUILTINS.array, builtin_index)) {
            free_string_array(&splitted_line);
            free_file_info(file_info);
            loop = false;
            break;
          }
          current_dir = getcwd(dir, sizeof(dir));
          free(last_two_dirs);
          last_two_dirs = getLastTwoDirs(current_dir);

          pushToCommandHistory(line, &command_history);

        } else {
          pid_t child;
          int w_status;

          dup2(fdin, STDIN_FILENO);
          close(fdin);
          if (file_info.merge_filename != NULL) {
            mergeRedirection(file_info, &fdout);
          } else {
            outputRedirection(file_info, pd, &fdout, &fdin, tmpout, i, simple_commands_arr);
            errorRedirection(file_info, &fderr, tmperr);

            dup2(fderr, STDERR_FILENO);
            close(fderr);
            dup2(fdout, STDOUT_FILENO);
            close(fdout);
          }

          if (splitted_line.len > 0) {
            if ((pid = fork()) == 0) {
              int error = execvp(splitted_line.values[0], splitted_line.values);
              if (error) {
                fprintf(stderr, "psh: couldn't find command %s\n", splitted_line.values[0]);
              }
            }

            if (waitpid(pid, &w_status, WUNTRACED | WCONTINUED) == -1) {
              exit(EXIT_FAILURE);
            }
          }
        }
        free_string_array(&splitted_line);
        free_file_info(file_info);
      }
      dup2(tmpin, 0);
      dup2(tmpout, 1);
      dup2(tmperr, 2);
      close(tmpin);
      close(tmpout);
      close(tmperr);
      free_string_array_token(simple_commands_arr);
    } else {
      if (!isOnlyDelimeter(line, ' ')) {
        fprintf(stderr, "psh: syntax error\n");
      }
    }
    writeCommandToGlobalHistory(line, global_command_history);
    pushToCommandHistory(line, &command_history);
    free(tokenized_line.arr);
    free(line);
  }
  free_string_array(&global_command_history);
  free_string_array(&command_history);
  free_string_array(&PATH_BINS);
  free(last_two_dirs);
}

#endif /* !TEST */
