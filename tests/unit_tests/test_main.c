#include "../../src/main.h"
#include <criterion/criterion.h>

// Unit Tests
Test(Parsing_directory, get_last_two_dirs) {
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell");

  cr_expect(strcmp(last_two_dirs, "c/pshell") == 0);
}

Test(Concatenating_command_history, check_concat_len) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};

  char* three = "three";
  char* four = "four";
  char* addr_two[] = {three, four};

  string_array arr1 = {.len = 2, .values = addr_one};

  string_array arr2 = {.len = 2, .values = addr_two};
  string_array result = concatenateArrays(arr1, arr2);

  cr_expect(result.len == 4);
}

Test(Concatenating_command_history, check_concat_elements) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};

  char* three = "three";
  char* four = "four";
  char* addr_two[] = {three, four};

  string_array arr1 = {.len = 2, .values = addr_one};

  string_array arr2 = {.len = 2, .values = addr_two};
  string_array result = concatenateArrays(arr1, arr2);
  char* concat[] = {one, two, three, four};
  string_array correct = {.values = concat, .len = 4};

  bool works = true;
  for (int i = 0; i < correct.len; i++) {
    if (strcmp(correct.values[i], result.values[i]) != 0) {
      works = false;
    }
  }
  cr_expect(works == true);
}

Test(insertCharAtPos, see_if_string_reference_changes) {
  char line[24] = "uwe tested";

  insertCharAtPos(line, 3, 'i');
  cr_expect(strcmp(line, "uwei tested") == 0);
}

Test(removeDots, removes_string_when_has_dot) {
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1, 1);
  strcpy(addr_one[0], "one");
  addr_one[1] = calloc(strlen("two") + 1, 1);
  strcpy(addr_one[1], "two");
  addr_one[2] = calloc(strlen("../") + 1, 1);
  strcpy(addr_one[2], "../");
  addr_one[3] = calloc(strlen("four") + 1, 1);
  strcpy(addr_one[3], "four");

  string_array arr1 = {.len = 4, .values = addr_one};

  string_array result = removeDots(&arr1);

  cr_expect(strcmp(result.values[0], "one") == 0);
  cr_expect(strcmp(result.values[1], "two") == 0);
  cr_expect(strcmp(result.values[2], "four") == 0);
}

Test(removeDots, array_stays_same_when_no_dot) {
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1, 1);
  strcpy(addr_one[0], "one");
  addr_one[1] = calloc(strlen("two") + 1, 1);
  strcpy(addr_one[1], "two");
  addr_one[2] = calloc(strlen("three") + 1, 1);
  strcpy(addr_one[2], "three");
  addr_one[3] = calloc(strlen("four") + 1, 1);
  strcpy(addr_one[3], "four");

  string_array arr1 = {.len = 4, .values = addr_one};

  string_array result = removeDots(&arr1);

  cr_expect(strcmp(result.values[0], "one") == 0);
  cr_expect(strcmp(result.values[1], "two") == 0);
  cr_expect(strcmp(result.values[2], "three") == 0);
  cr_expect(strcmp(result.values[3], "four") == 0);
  free_string_array(&result);
}

Test(stringToLower, converting_string_lowercase_in_place) {
  char* string = calloc(strlen("halle") + 1, sizeof(char));
  strcpy(string, "HaLLe");

  stringToLower(string);

  cr_expect(strcmp(string, "halle") == 0);
  free(string);
}

// big update tests
Test(update, writing_normal_commands_works) {
  line_data* line_info = lineDataConstructor(4);
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();
  line_info->c = 'l';

  string_array arr1 = {.len = 0};
  string_array global_command_history = arr1;
  string_array* sessions_command_history = &arr1;
  history_data* history_info = historyDataConstructor(sessions_command_history, global_command_history);
  coordinates* cursor_pos;

  bool result = update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL},
                       cursor_pos);

  cr_expect(result == true);
  cr_expect(strcmp(line_info->line, "l") == 0);
  cr_expect(*line_info->i == 1);
  cr_expect(autocomplete_info->autocomplete == false);

  line_info->c = 's';
  bool result2 = update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL},
                        cursor_pos);
  cr_expect(result == true);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 2);
  cr_expect(autocomplete_info->autocomplete == false);

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free(history_info);
  free(line_info->line);
  free(line_info->i);
  free(line_info);
}

Test(update, moving_cursor_with_arrow_keys) {
  line_data* line_info = lineDataConstructor(8);
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();

  string_array arr1 = {.len = 0};
  string_array global_command_history = arr1;
  string_array* sessions_command_history = &arr1;
  history_data* history_info = historyDataConstructor(sessions_command_history, global_command_history);
  coordinates* cursor_pos;

  // type something to test
  line_info->c = 'l';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 's';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);

  int fds[2];
  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }
  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "ZDZDZDZC", sizeof("ZDZDZDZC")); // emulate horizontal-arrow-press
  close(fds[1]);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 1);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 0);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 0);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 1);

  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "les") == 0);
  cr_expect(*line_info->i == 2);

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free(history_info);
  free(line_info->line);
  free(line_info->i);
  free(line_info);
}

Test(update, moving_through_history_with_cursor) {
  line_data* line_info = lineDataConstructor(8);
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();

  char** addr_one = calloc(2, sizeof(char*));
  addr_one[0] = calloc(strlen("yeyeye") + 1, 1);
  strcpy(addr_one[0], "yeyeye");
  addr_one[1] = calloc(strlen("test") + 1, 1);
  strcpy(addr_one[1], "test");
  string_array global_command_history = (string_array){.len = 0};
  string_array* sessions_command_history = calloc(1, sizeof(string_array));
  sessions_command_history->values = addr_one;
  sessions_command_history->len = 2;
  history_data* history_info = historyDataConstructor(sessions_command_history, global_command_history);
  coordinates* cursor_pos;

  // type something to test
  line_info->c = 'l';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 's';
  cr_expect(history_info->history_index == 0);

  line_info->c = ESCAPE;
  int fds[2];
  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }
  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "ZAZAZAZBZB", sizeof("ZAZAZAZBZB")); // emulate key-press_sequence
  close(fds[1]);

  bool result = update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL},
                       cursor_pos);
  cr_expect(result == true);
  cr_expect(strcmp(line_info->line, "yeyeye") == 0);
  cr_expect(*line_info->i == strlen("yeyeye"));
  cr_expect(history_info->history_index == 1);

  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(history_info->history_index == 2);

  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(history_info->history_index == 2);

  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "yeyeye") == 0);
  cr_expect(*line_info->i == strlen("yeyeye"));
  cr_expect(history_info->history_index == 1);

  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "") == 0);
  cr_expect(*line_info->i == strlen(""));
  cr_expect(history_info->history_index == 0);

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free_string_array(&history_info->sessions_command_history);
  free(history_info);
  free(line_info->line);
  free(line_info->i);
  free(line_info);
}

Test(update, moving_cursor_to_endofline_when_matching_complete_from_current_session) {
  line_data* line_info = lineDataConstructor(8);
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();

  string_array global_command_history = (string_array){.len = 0};
  string_array* sessions_command_history = calloc(1, sizeof(string_array));
  char** addr_one = calloc(2, sizeof(char*));
  addr_one[0] = calloc(strlen("yeyeye") + 1, 1);
  strcpy(addr_one[0], "yeyeye");
  addr_one[1] = calloc(strlen("test") + 1, 1);
  strcpy(addr_one[1], "test");
  sessions_command_history->values = addr_one;
  sessions_command_history->len = 2;
  history_data* history_info = historyDataConstructor(sessions_command_history, global_command_history);
  coordinates* cursor_pos;

  // type something to test
  line_info->c = 't';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(autocomplete_info->autocomplete == true);
  cr_expect(strcmp(autocomplete_info->possible_autocomplete, "test") == 0);

  line_info->c = ESCAPE;
  int fds[2];
  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }
  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "ZC", sizeof("ZC")); // emulate left-arrow-press
  close(fds[1]);

  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(autocomplete_info->autocomplete == true);

  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(autocomplete_info->autocomplete == false);
  cr_expect(strcmp(line_info->line, "teste") == 0);
  cr_expect(*line_info->i == strlen("teste"));

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free(history_info);
  free(line_info->line);
  free(line_info->i);
  free(line_info);
}

Test(update, moving_cursor_to_endofline_when_matching_complete_from_global_history) {
  line_data* line_info = lineDataConstructor(8);
  autocomplete_data* autocomplete_info = autocompleteDataConstructor();

  string_array* sessions_command_history = calloc(1, sizeof(string_array));
  char** addr_one = calloc(1, sizeof(char*));
  char** addr_two = calloc(1, sizeof(char*));
  addr_one[0] = calloc(strlen("test") + 1, 1);
  strcpy(addr_one[0], "test");
  addr_two[0] = calloc(strlen("tig") + 1, 1);
  strcpy(addr_two[0], "tig");
  sessions_command_history->values = addr_one;
  sessions_command_history->len = 1;
  string_array global_command_history = {.values = addr_two, .len = 1};
  history_data* history_info = historyDataConstructor(sessions_command_history, global_command_history);
  coordinates* cursor_pos;

  // type something to test
  line_info->c = 't';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(autocomplete_info->autocomplete == true);
  cr_expect(strcmp(autocomplete_info->possible_autocomplete, "test") == 0);

  line_info->c = BACKSPACE;
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "t") == 0);
  cr_expect(*line_info->i == strlen("t"));
  cr_expect(autocomplete_info->autocomplete == true);

  line_info->c = 'i';
  update(line_info, autocomplete_info, history_info, (coordinates){20, 40}, (string_array){0, NULL}, cursor_pos);
  cr_expect(autocomplete_info->autocomplete == true);
  cr_expect(strcmp(autocomplete_info->possible_autocomplete, "tig") == 0);
  cr_expect(strcmp(line_info->line, "ti") == 0);
  cr_expect(*line_info->i == strlen("ti"));

  free(autocomplete_info->possible_autocomplete);
  free(autocomplete_info);
  free(history_info);
  free(line_info->line);
  free(line_info->i);
  free(line_info);
}

Test(tokenizeLine, tokenizes_simple_command) {
  char* line = "ls arg";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 3);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 3);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 3);
  cr_expect(result_arr[2].end == 6);
}

Test(tokenizeLine, tokenizes_command_with_too_much_whitespace) {
  char* line = "   ls  arg_s   ";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 5);
  cr_expect(result_arr[0].token == WHITESPACE);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 3);
  cr_expect(result_arr[1].token == CMD);
  cr_expect(result_arr[1].start == 3);
  cr_expect(result_arr[1].end == 5);
  cr_expect(result_arr[2].token == WHITESPACE);
  cr_expect(result_arr[2].start == 5);
  cr_expect(result_arr[2].end == 7);
  cr_expect(result_arr[3].token == ARG);
  cr_expect(result_arr[3].start == 7);
  cr_expect(result_arr[3].end == 12);
  cr_expect(result_arr[4].token == WHITESPACE);
  cr_expect(result_arr[4].start == 12);
  cr_expect(result_arr[4].end == 15);
}

Test(tokenizeLine, tokenizes_command_with_too_much_whitespace_and_pipe) {
  char* line = "   ls  arg_s   | next_cmd  flag=some";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 10);
  cr_expect(result_arr[0].token == WHITESPACE);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 3);
  cr_expect(result_arr[1].token == CMD);
  cr_expect(result_arr[1].start == 3);
  cr_expect(result_arr[1].end == 5);
  cr_expect(result_arr[2].token == WHITESPACE);
  cr_expect(result_arr[2].start == 5);
  cr_expect(result_arr[2].end == 7);
  cr_expect(result_arr[3].token == ARG);
  cr_expect(result_arr[3].start == 7);
  cr_expect(result_arr[3].end == 12);
  cr_expect(result_arr[4].token == WHITESPACE);
  cr_expect(result_arr[4].start == 12);
  cr_expect(result_arr[4].end == 15);
  cr_expect(result_arr[5].token == PIPE);
  cr_expect(result_arr[5].start == 15);
  cr_expect(result_arr[5].end == 16);
  cr_expect(result_arr[6].token == WHITESPACE);
  cr_expect(result_arr[6].start == 16);
  cr_expect(result_arr[6].end == 17);
  cr_expect(result_arr[7].token == PIPE_CMD);
  cr_expect(result_arr[7].start == 17);
  cr_expect(result_arr[7].end == 25);
  cr_expect(result_arr[8].token == WHITESPACE);
  cr_expect(result_arr[8].start == 25);
  cr_expect(result_arr[8].end == 27);
  cr_expect(result_arr[9].token == ARG);
  cr_expect(result_arr[9].start == 27);
  cr_expect(result_arr[9].end == 36);
}

Test(tokenizeLine, only_command_and_pipe_cmd) {
  char* line = "ls|next_cmd";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 3);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == PIPE);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 3);
  cr_expect(result_arr[2].token == PIPE_CMD);
  cr_expect(result_arr[2].start == 3);
  cr_expect(result_arr[2].end == 11);
}

Test(tokenizeLine, command_and_pipe_cmd_multiple_args) {
  char* line = "ls|next_cmd uwe test";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 7);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == PIPE);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 3);
  cr_expect(result_arr[2].token == PIPE_CMD);
  cr_expect(result_arr[2].start == 3);
  cr_expect(result_arr[2].end == 11);
}

Test(removeWhitespacetokens, removes_whitetoken) {
  token_index arr1 = {.token = WHITESPACE, .start = 0, .end = 2};
  token_index arr2 = {.token = CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2};
  token_index_arr token = {.arr = arr, .len = 2};

  removeWhitespaceTokens(&token);
  cr_expect(token.arr[0].token == CMD);
  cr_expect(token.len == 1);
}

Test(removeWhitespacetokens, removes_whitetoken_when_multiple_args) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = WHITESPACE, .start = 0, .end = 2};
  token_index arr3 = {.token = ARG, .start = 0, .end = 2};
  token_index arr4 = {.token = WHITESPACE, .start = 0, .end = 2};
  token_index arr5 = {.token = ARG, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5};
  token_index_arr token = {.arr = arr, .len = 5};

  removeWhitespaceTokens(&token);
  cr_expect(token.len == 3);
  cr_expect(token.arr[0].token == CMD);
  cr_expect(token.arr[1].token == ARG);
  cr_expect(token.arr[2].token == ARG);
  cr_expect(token.len == 3);
}

Test(isValidSyntax, valid_when_normal_syntax) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr4 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4};
  token_index_arr token = {.arr = arr, .len = 4};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, not_valid_when_multiple_pipes_successively) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr4 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4};
  token_index_arr token = {.arr = arr, .len = 4};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}

Test(isValidSyntax, not_valid_when_last_char_pipe) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2};
  token_index_arr token = {.arr = arr, .len = 2};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}

Test(isValidSyntax, not_valid_when_starts_with_pipe) {
  token_index arr1 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2};
  token_index_arr token = {.arr = arr, .len = 2};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}
