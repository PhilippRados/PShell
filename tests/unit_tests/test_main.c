#include "../../src/main.h"
#include <criterion/criterion.h>

// Unit Tests
Test(Parsing_directory, get_last_two_dirs) {
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell");

  cr_expect(strcmp(last_two_dirs, "c/pshell") == 0);
}

Test(Split_func, split_string_at_delimeter) {
  string_array result = splitString("this.should.split", '.');

  cr_expect(strcmp(result.values[0], "this") == 0);
  cr_expect(strcmp(result.values[1], "should") == 0);
  cr_expect(strcmp(result.values[2], "split") == 0);
}

Test(Split_func, single_strings_are_nullterminated) {
  string_array result = splitString("this.should.split", '.');

  cr_expect(result.values[0][strlen(result.values[0])] == '\0', "%c", result.values[0][strlen(result.values[0])]);
}

Test(Split_func, quits_on_q) {
  string_array result = splitString("q", ' ');

  cr_expect(strcmp(result.values[0], "q") == 0);
}

Test(Split_func, end_string_split_with_NULL) {
  string_array result = splitString("ls -l", ' ');

  cr_expect_null(result.values[3]);
}

Test(Split_func, only_command_len) {
  string_array result = splitString("ma", ' ');

  cr_expect(result.len == 1);
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
  token_index arr1 = {.token = CMD};
  token_index arr2 = {.token = WHITESPACE};
  token_index arr3 = {.token = ARG};
  token_index arr4 = {.token = WHITESPACE};
  token_index arr5 = {.token = ARG};
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

Test(isValidSyntax, cmd_with_args_pipe_and_ampamp) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = AMPAMP, .start = 0, .end = 2};
  token_index arr4 = {.token = AMP_CMD, .start = 0, .end = 2};
  token_index arr5 = {.token = ARG, .start = 0, .end = 2};
  token_index arr6 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr7 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6, arr7};
  token_index_arr token = {.arr = arr, .len = 7};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, cmd_pipe_and_ampamp) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr4 = {.token = AMPAMP, .start = 0, .end = 2};
  token_index arr5 = {.token = AMP_CMD, .start = 0, .end = 2};
  token_index arr6 = {.token = ARG, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6};
  token_index_arr token = {.arr = arr, .len = 6};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, redirectction_end_of_line) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr4 = {.token = ARG, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4};
  token_index_arr token = {.arr = arr, .len = 4};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}
Test(isValidSyntax, redirectction_end_of_line_no_filename) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3};
  token_index_arr token = {.arr = arr, .len = 3};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}

Test(isValidSyntax, redirection_also_pipe) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr3 = {.token = ARG, .start = 0, .end = 2};
  token_index arr4 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr5 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5};
  token_index_arr token = {.arr = arr, .len = 5};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, multiple_redirection_also_pipe) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr3 = {.token = ARG, .start = 0, .end = 2};
  token_index arr4 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr5 = {.token = ARG, .start = 0, .end = 2};
  token_index arr6 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr7 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6, arr7};
  token_index_arr token = {.arr = arr, .len = 7};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, only_redirection) {
  token_index arr1 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2};
  token_index_arr token = {.arr = arr, .len = 2};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, redirection_between_cmd_and_args) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr3 = {.token = ARG, .start = 0, .end = 2};
  token_index arr4 = {.token = ARG, .start = 0, .end = 2};
  token_index arr5 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr6 = {.token = ARG, .start = 0, .end = 2};
  token_index arr7 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr8 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6, arr7, arr8};
  token_index_arr token = {.arr = arr, .len = 8};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, redirection_pipe_and_pipe_cmd) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr3 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr4 = {.token = ARG, .start = 0, .end = 2};
  token_index arr5 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr6 = {.token = ARG, .start = 0, .end = 2};
  token_index arr7 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6, arr7};
  token_index_arr token = {.arr = arr, .len = 7};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, redirection_ampamp_and_amp_cmd) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = AMPAMP, .start = 0, .end = 2};
  token_index arr3 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr4 = {.token = ARG, .start = 0, .end = 2};
  token_index arr5 = {.token = AMP_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5};
  token_index_arr token = {.arr = arr, .len = 5};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, mulitple_redirections_without_filenames) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr4 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr5 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5};
  token_index_arr token = {.arr = arr, .len = 5};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}
Test(isValidSyntax, redirectction_after_pipecmd_and_arg) {
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr4 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr5 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr6 = {.token = ARG, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6};
  token_index_arr token = {.arr = arr, .len = 6};

  bool result = isValidSyntax(token);
  cr_expect(result == true);
}

Test(isValidSyntax, only_redirection_and_pipe_cmd) {
  token_index arr1 = {.token = GREAT, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 0, .end = 2};
  token_index arr3 = {.token = PIPE, .start = 0, .end = 2};
  token_index arr4 = {.token = PIPE_CMD, .start = 0, .end = 2};
  token_index arr[] = {arr1, arr2, arr3, arr4};
  token_index_arr token = {.arr = arr, .len = 4};

  bool result = isValidSyntax(token);
  cr_expect(result == false);
}

Test(splitLineIntoSimpleCommands, splits_at_pipe) {
  char* line = "ls  |uwe";
  token_index arr1 = {.token = CMD, .start = 0, .end = 4};
  token_index arr2 = {.token = PIPE, .start = 4, .end = 5};
  token_index arr3 = {.token = PIPE_CMD, .start = 5, .end = 8};
  token_index arr[] = {arr1, arr2, arr3};
  token_index_arr token = {.arr = arr, .len = 3};

  string_array_token result = splitLineIntoSimpleCommands(line, token);
  cr_expect(result.len == 2);
  cr_expect(strcmp(result.values[0], "ls  ") == 0);
  cr_expect(strcmp(result.values[1], "uwe") == 0);
}

Test(splitLineIntoSimpleCommands, tokenizes_splitted_commands_into_ampamp_or_pipe) {
  char* line = "ls  arg&& uwe | last";
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 4, .end = 7};
  token_index arr3 = {.token = AMPAMP, .start = 7, .end = 9};
  token_index arr4 = {.token = AMP_CMD, .start = 10, .end = 13};
  token_index arr5 = {.token = PIPE, .start = 14, .end = 15};
  token_index arr6 = {.token = PIPE_CMD, .start = 16, .end = 20};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6};
  token_index_arr token = {.arr = arr, .len = 6};

  string_array_token result = splitLineIntoSimpleCommands(line, token);
  cr_expect(result.len == 3);
  cr_expect(strcmp(result.values[0], "ls  arg") == 0);
  cr_expect(result.token_arr[0] == CMD);
  cr_expect(strcmp(result.values[1], "uwe ") == 0);
  cr_expect(result.token_arr[1] == AMP_CMD);
  cr_expect(strcmp(result.values[2], "last") == 0);
  cr_expect(result.token_arr[2] == PIPE_CMD);
}

Test(splitLineIntoSimpleCommands, splits_at_pipe_with_arg_and_whitespace) {
  char* line = "ls some_arg| uwe also|last";
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 3, .end = 11};
  token_index arr3 = {.token = PIPE, .start = 11, .end = 12};
  token_index arr4 = {.token = PIPE_CMD, .start = 13, .end = 16};
  token_index arr5 = {.token = ARG, .start = 17, .end = 21};
  token_index arr6 = {.token = PIPE, .start = 21, .end = 22};
  token_index arr7 = {.token = PIPE_CMD, .start = 22, .end = 26};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5, arr6, arr7};
  token_index_arr token = {.arr = arr, .len = 7};

  string_array_token result = splitLineIntoSimpleCommands(line, token);
  cr_expect(result.len == 3);
  cr_expect(strcmp(result.values[0], "ls some_arg") == 0);
  cr_expect(result.token_arr[0] == CMD);
  cr_expect(strcmp(result.values[1], "uwe also") == 0);
  cr_expect(result.token_arr[1] == PIPE_CMD);
  cr_expect(strcmp(result.values[2], "last") == 0);
  cr_expect(result.token_arr[2] == PIPE_CMD);
}

Test(splitLineIntoSimpleCommands, splits_at_pipe_with_mutliple_whitespace) {
  char* line = "ls  |  cd|bat";
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = PIPE, .start = 4, .end = 5};
  token_index arr3 = {.token = PIPE_CMD, .start = 7, .end = 9};
  token_index arr4 = {.token = PIPE, .start = 9, .end = 10};
  token_index arr5 = {.token = PIPE_CMD, .start = 10, .end = 12};
  token_index arr[] = {arr1, arr2, arr3, arr4, arr5};
  token_index_arr token = {.arr = arr, .len = 5};

  string_array_token result = splitLineIntoSimpleCommands(line, token);
  cr_expect(result.len == 3);
  cr_expect(strcmp(result.values[0], "ls  ") == 0);
  cr_expect(result.token_arr[0] == CMD);
  cr_expect(strcmp(result.values[1], "cd") == 0);
  cr_expect(result.token_arr[1] == PIPE_CMD);
  cr_expect(strcmp(result.values[2], "bat") == 0);
  cr_expect(result.token_arr[2] == PIPE_CMD);
}

Test(splitLineIntoSimpleCommands, split_with_ampamp) {
  char* line = "ls  &&cmd";
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = AMPAMP, .start = 4, .end = 6};
  token_index arr3 = {.token = AMP_CMD, .start = 6, .end = 9};
  token_index arr[] = {arr1, arr2, arr3};
  token_index_arr token = {.arr = arr, .len = 3};

  string_array_token result = splitLineIntoSimpleCommands(line, token);
  cr_expect(result.len == 2);
  cr_expect(strcmp(result.values[0], "ls  ") == 0);
  cr_expect(result.token_arr[0] == CMD);
  cr_expect(strcmp(result.values[1], "cmd") == 0);
  cr_expect(result.token_arr[1] == AMP_CMD);
}

Test(splitByToken, splitByWhitespaceToken) {
  char* line = "ls ..";
  token_index arr1 = {.token = CMD, .start = 0, .end = 2};
  token_index arr2 = {.token = ARG, .start = 3, .end = 5};
  token_index arr[] = {arr1, arr2};
  token_index_arr token = {.arr = arr, .len = 2};
  string_array result = splitByTokens(line, token);

  cr_expect(result.len == 2);
  cr_expect(strcmp(result.values[0], "ls") == 0);
  cr_expect(strcmp(result.values[1], "..") == 0);
}

Test(splitByToken, split_only_token_also_with_multiple_whitespace) {
  char* line = "    bat";
  token_index arr1 = {.token = CMD, .start = 4, .end = 7};
  token_index arr[] = {arr1};
  token_index_arr token = {.arr = arr, .len = 1};
  string_array result = splitByTokens(line, token);

  cr_expect(result.len == 1);
  cr_expect(strcmp(result.values[0], "bat") == 0);
}

Test(parseForRedirectionFiles, removes_all_redirections_in_split) {
  char* simple_cmd1 = ">log.txt   bat";
  token_index arr1 = {.token = GREAT, .start = 0, .end = 1};
  token_index arr2 = {.token = ARG, .start = 1, .end = 8};
  token_index arr3 = {.token = CMD, .start = 11, .end = 14};
  token_index t1[] = {arr1, arr2, arr3};
  token_index_arr token1 = {.arr = t1, .len = 3};
  char* simple_cmd2 = " < fl.txt cmd arg";
  token_index arr4 = {.token = LESS, .start = 1, .end = 2};
  token_index arr5 = {.token = ARG, .start = 3, .end = 9};
  token_index arr6 = {.token = CMD, .start = 11, .end = 14};
  token_index arr7 = {.token = ARG, .start = 16, .end = 19};
  token_index t2[] = {arr4, arr5, arr6, arr7};
  token_index_arr token2 = {.arr = t2, .len = 4};
  string_array split1 = splitByTokens(simple_cmd1, token1);
  string_array split2 = splitByTokens(simple_cmd2, token2);

  file_redirection_data result1 = parseForRedirectionFiles(split1, token1);
  file_redirection_data result2 = parseForRedirectionFiles(split2, token2);

  cr_expect(strcmp(result1.output_filename, "log.txt") == 0);
  cr_expect_null(result1.input_filename);
  cr_expect(result1.output_append == false);
  cr_expect_null(result2.output_filename);
  cr_expect(strcmp(result2.input_filename, "fl.txt") == 0);
  cr_expect(result2.output_append == false);
}

Test(expandWildcardgroups, replace_wildcard_astrisk_when_single_match) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls sr*");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "src ") == 0);
  free(line);
}

Test(expandWildcardgroups, replace_wildcard_astrisk_with_everything_if_not_after_file) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls *");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "Dockerfile Makefile tests README.md log.txt"
                                               " compile_flags.txt src ") == 0);
  free(line);
}

Test(expandWildcardgroups, replace_wildcard_astrisk_with_multiple_matches_in_dir) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls tests/* some_other");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "tests/unit_tests tests/integration_tests ") == 0);
  free(line);
}

Test(expandWildcardgroups, if_asterisk_in_middle_of_arg) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls Do*ile");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "Dockerfile ") == 0);
  free(line);
}

Test(expandWildcardgroups, multiple_asterisks_in_one_arg) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls sr*/fuz*");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "src/fuzzy_finder.c src/fuzzy_finder.h ") == 0);
  free(line);
}

Test(expandWildcardgroups, multiple_asterisks_in_line) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls sr*/fuz*  *  *file te*&&");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 4);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "src/fuzzy_finder.c src/fuzzy_finder.h ") == 0);

  cr_expect(strcmp(result.arr[1].wildcard_arg, "Dockerfile Makefile tests README.md log.txt"
                                               " compile_flags.txt src ") == 0);
  cr_expect(strcmp(result.arr[2].wildcard_arg, "Dockerfile Makefile ") == 0);
  cr_expect(strcmp(result.arr[3].wildcard_arg, "tests ") == 0);
  free(line);
}

Test(expandWildcardgroups, escapes_whitespace_in_wildcard_match) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls tests/unit_tests/prop_test_files/*");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr result = expandWildcardgroups(groups);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "tests/unit_tests/prop_test_files/with\\ \\ multiple\\ white "
                                               "tests/unit_tests/prop_test_files/with\\ whitespace ") == 0);

  free(line);
}

Test(groupWildcards, finds_all_wildcard_groupings) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls sr*/fuz*  *  *file te*&&");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr result = groupWildcards(line, token);
  cr_expect(result.len == 4);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "sr*/fuz*") == 0);
  cr_expect(result.arr[0].start == 3);
  cr_expect(result.arr[0].end == 11);
  cr_expect(strcmp(result.arr[1].wildcard_arg, "*") == 0);
  cr_expect(result.arr[1].start == 13);
  cr_expect(result.arr[1].end == 14);
  cr_expect(strcmp(result.arr[2].wildcard_arg, "*file") == 0);
  cr_expect(result.arr[2].start == 16);
  cr_expect(result.arr[2].end == 21);
  cr_expect(strcmp(result.arr[3].wildcard_arg, "te*") == 0);
  cr_expect(result.arr[3].start == 22);
  cr_expect(result.arr[3].end == 25);
  free(line);
  free(result.arr);
}

Test(groupWildcards, groups_with_question_wildcard) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls Do??erfile");
  token_index_arr token = tokenizeLine(line);

  wildcard_groups_arr result = groupWildcards(line, token);
  cr_expect(result.len == 1);
  cr_expect(strcmp(result.arr[0].wildcard_arg, "Do??erfile") == 0);
  cr_expect(result.arr[0].start == 3);
  cr_expect(result.arr[0].end == 13);
  free(line);
  free(result.arr);
}

Test(replaceLineWithWildcards, replaces_regular_line_with_wildcard_match) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls sr*/fuz* *file te*&&");

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr wildcard_matches = expandWildcardgroups(groups);
  replaceLineWithWildcards(&line, wildcard_matches);

  cr_expect(strcmp(line, "ls src/fuzzy_finder.c src/fuzzy_finder.h  Dockerfile Makefile  tests &&") == 0);
  free(line);
}

Test(replaceLineWithWildcards, double_question_wildcard_in_center) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls Do??erfile");

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr wildcard_matches = expandWildcardgroups(groups);
  replaceLineWithWildcards(&line, wildcard_matches);

  cr_expect(strcmp(line, "ls Dockerfile ") == 0);
  free(line);
}
Test(replaceLineWithWildcards, single_letter_between_two_asterisks) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls *a*");

  wildcard_groups_arr groups = groupWildcards(line, tokenizeLine(line));
  wildcard_groups_arr wildcard_matches = expandWildcardgroups(groups);
  replaceLineWithWildcards(&line, wildcard_matches);

  cr_expect(strcmp(line, "ls Makefile compile_flags.txt ") == 0);
  free(line);
}
