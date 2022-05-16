#include "../../src/util.h"
#include <criterion/criterion.h>

Test(removeMultipleWhitespace, removes_any_extraneous_whitespace) {
  char* line = calloc(64, sizeof(char));
  strcpy(line, "    uwe   ist cool");
  char* result = removeMultipleWhitespaces(line);
  cr_expect(strcmp(result, "uwe ist cool") == 0);
  free(result);
}

Test(removeMultipleWhitespace, only_whitespace) {
  char* empty = calloc(5, sizeof(char));
  strcpy(empty, "    ");
  char* result = removeMultipleWhitespaces(empty);

  cr_expect(strcmp(result, "") == 0);
  free(result);
}

Test(removeMultipleWhitespace, when_everything_fine_nothing_changs) {
  char* line = calloc(64, sizeof(char));
  strcpy(line, "ls .");
  char* result = removeMultipleWhitespaces(line);
  cr_expect(strcmp(result, "ls .") == 0);
  free(result);
}

Test(insertStringAtPos, insert_string_at_end) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "testing the waters");
  char* insert_string = " here";

  insertStringAtPos(&line, insert_string, strlen(line));
  cr_expect(strcmp(line, "testing the waters here") == 0);

  free(line);
}

Test(insertStringAtPos, insert_string_in_middle) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "testing the waters");
  char* insert_string = "cold ";

  insertStringAtPos(&line, insert_string, 12);

  cr_expect(strcmp(line, "testing the cold waters") == 0);

  free(line);
}

Test(insertStringAtPos, insert_string_at_start) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "~/testing");
  char* insert_string = "/Users";

  removeCharAtPos(line, 1);
  insertStringAtPos(&line, insert_string, 0);

  cr_expect(strcmp(line, "/Users/testing") == 0);

  free(line);
}

Test(getAppendingIndex, returns_3_if_second_word_is_len_3) {
  char line[64] = "make mak";
  int result = getAppendingIndex(line, ' ');

  cr_expect(result == 3);
}

Test(getAppendingIndex, still_works_with_only_space) {
  char line[64] = "make ";
  int result = getAppendingIndex(line, ' ');

  cr_expect(result == 0);
}

Test(calculateCursorPos, jumps_down_if_current_line_longer_than_term) {
  coordinates cursor_pos = {.x = 20, .y = 2};
  coordinates term_size = {.x = 20, .y = 100};
  int prompt_len = 5;
  int i = 20;

  coordinates result = calculateCursorPos(term_size, cursor_pos, prompt_len, i);
  cr_expect(result.x == 5);
  cr_expect(result.y == 3);
}

Test(calculateCursorPos, shouldnt_change_coordinates_when_line_short) {
  coordinates cursor_pos = {.x = 15, .y = 2};
  coordinates term_size = {.x = 20, .y = 100};
  int prompt_len = 5;
  int i = 10;

  coordinates result = calculateCursorPos(term_size, cursor_pos, prompt_len, i);
  cr_expect(result.x == 15);
  cr_expect(result.y == 2);
}

Test(calculateCursorPos, when_on_last_row_should_still_increment) {
  coordinates cursor_pos = {.x = 15, .y = 100};
  coordinates term_size = {.x = 20, .y = 100};
  int prompt_len = 5;
  int i = 20;

  coordinates result = calculateCursorPos(term_size, cursor_pos, prompt_len, i);
  cr_expect(result.x == 5);
  cr_expect(result.y == 101);
}

Test(calculateCursorPos, when_line_expands_over_many_rows_and_cursor_shouldnt_jump_down_too_early) {
  coordinates cursor_pos = {.x = 0, .y = 1};
  coordinates term_size = {.x = 45, .y = 100};
  int prompt_len = 0;
  int i = 90;

  coordinates result = calculateCursorPos(term_size, cursor_pos, prompt_len, i);

  cr_expect(result.x == 45);
  cr_expect(result.y == 2);
}

Test(isOnlyDelimeter, true_if_just_white_space) {
  char* string = "                  ";
  bool result = isOnlyDelimeter(string, ' ');

  cr_expect(result == true);
}

Test(isOnlyDelimeter, false_if_even_single_char) {
  char* string = "                  l";
  bool result = isOnlyDelimeter(string, ' ');

  cr_expect(result == false);
}

Test(firstNonDelimeterIndex, returns_index_of_splitted_array_where_not_delimeter) {
  char* one = "";
  char* two = "";
  char* three = "com";
  char* four = "uwe";
  char* addr_one[] = {one, two, three, four};

  string_array arr1 = {.len = 4, .values = addr_one};
  int result = firstNonDelimeterIndex(arr1);

  cr_expect(result == 2);
}

Test(firstNonDelimeterIndex, if_first_elem_not_delim_returns_zero) {
  char* three = "com";
  char* four = "uwe";
  char* addr_one[] = {three, four};

  string_array arr1 = {.len = 2, .values = addr_one};
  int result = firstNonDelimeterIndex(arr1);

  cr_expect(result == 0);
}

Test(insertCharAtPos, see_if_string_reference_changes) {
  char line[24] = "uwe tested";

  insertCharAtPos(line, 3, 'i');
  cr_expect(strcmp(line, "uwei tested") == 0);
}

Test(removeSlice, when_slice_is_complete_line_empties_string) {
  char* line = calloc(4, sizeof(char));
  strcpy(line, "src");
  removeSlice(&line, 0, 3);
  cr_expect(strcmp(line, "") == 0);
  free(line);
}

Test(removeSlice, remove_nothing_cursor_end_of_current_word) {
  char* word = calloc(52, sizeof(char));
  strcpy(word, "testing if Makefile works");
  int start = 19;

  removeSlice(&word, start, start);

  cr_expect(strcmp(word, "testing if Makefile works") == 0);
  free(word);
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

Test(tokenizeLine, tokenizes_line_with_escaped_whitespace_as_single_arg) {
  char* line = "ls this_is_\\ one_arg";
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
  cr_expect(result_arr[2].end == 20);
}

Test(tokenizeLine, tokenizes_line_with_multiple_escaped_whitespace_as_single_arg) {
  char* line = "ls this_is_\\ \\ \\ one_arg";
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
  cr_expect(result_arr[2].end == 24);
}

Test(tokenizeLine, tokenizes_complex_command_with_escapes) {
  char* line = "echo this\\ is&&echo this\\ is";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 7);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 4);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 4);
  cr_expect(result_arr[1].end == 5);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 5);
  cr_expect(result_arr[2].end == 13);
  cr_expect(result_arr[3].token == AMPAMP);
  cr_expect(result_arr[3].start == 13);
  cr_expect(result_arr[3].end == 15);
  cr_expect(result_arr[4].token == AMP_CMD);
  cr_expect(result_arr[4].start == 15);
  cr_expect(result_arr[4].end == 19);
  cr_expect(result_arr[5].token == WHITESPACE);
  cr_expect(result_arr[5].start == 19);
  cr_expect(result_arr[5].end == 20);
  cr_expect(result_arr[6].token == ARG);
  cr_expect(result_arr[6].start == 20);
  cr_expect(result_arr[6].end == 28);
}

Test(tokenizeLine, tokenizes_everything_in_quotes_as_arg) {
  char* line = "ls 'this is one big arg'";
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
  cr_expect(result_arr[2].end == 24);
}

Test(tokenizeLine, can_seperate_multiple_quoted_args_in_same_line) {
  char* line = "ls 'this is one big arg'&&ls 'this_another'";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 7);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 3);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 3);
  cr_expect(result_arr[2].end == 24);
  cr_expect(result_arr[3].token == AMPAMP);
  cr_expect(result_arr[3].start == 24);
  cr_expect(result_arr[3].end == 26);
  cr_expect(result_arr[4].token == AMP_CMD);
  cr_expect(result_arr[4].start == 26);
  cr_expect(result_arr[4].end == 28);
  cr_expect(result_arr[5].token == WHITESPACE);
  cr_expect(result_arr[5].start == 28);
  cr_expect(result_arr[5].end == 29);
  cr_expect(result_arr[6].token == ARG);
  cr_expect(result_arr[6].start == 29);
  cr_expect(result_arr[6].end == 43);
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

Test(tokenizeLine, first_token_great_redirection) {
  char* line = ">file";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 2);
  cr_expect(result_arr[0].token == GREAT);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 1);
  cr_expect(result_arr[1].token == ARG);
  cr_expect(result_arr[1].start == 1);
  cr_expect(result_arr[1].end == 5);
}

Test(tokenizeLine, first_token_greatgreat_redirection) {
  char* line = ">>file";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 2);
  cr_expect(result_arr[0].token == GREATGREAT);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == ARG);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 6);
}

Test(tokenizeLine, first_token_greatgreat_redirection_prefixed_with_fd) {
  char* line = "1>>   file";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 3);
  cr_expect(result_arr[0].token == GREATGREAT);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 3);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 3);
  cr_expect(result_arr[1].end == 6);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 6);
  cr_expect(result_arr[2].end == 10);
}

Test(tokenizeLine, multiple_redirections_with_ampamp) {
  char* line = "<log.txt   bat&& > fl.txt cmd arg";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 13);
  cr_expect(result_arr[0].token == LESS);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 1);
  cr_expect(result_arr[1].token == ARG);
  cr_expect(result_arr[1].start == 1);
  cr_expect(result_arr[1].end == 8);
  cr_expect(result_arr[2].token == WHITESPACE);
  cr_expect(result_arr[2].start == 8);
  cr_expect(result_arr[2].end == 11);
  cr_expect(result_arr[3].token == CMD);
  cr_expect(result_arr[3].start == 11);
  cr_expect(result_arr[3].end == 14);
  cr_expect(result_arr[4].token == AMPAMP);
  cr_expect(result_arr[4].start == 14);
  cr_expect(result_arr[4].end == 16);
  cr_expect(result_arr[5].token == WHITESPACE);
  cr_expect(result_arr[5].start == 16);
  cr_expect(result_arr[5].end == 17);
  cr_expect(result_arr[6].token == GREAT);
  cr_expect(result_arr[6].start == 17);
  cr_expect(result_arr[6].end == 18);
  cr_expect(result_arr[7].token == WHITESPACE);
  cr_expect(result_arr[7].start == 18);
  cr_expect(result_arr[7].end == 19);
  cr_expect(result_arr[8].token == ARG);
  cr_expect(result_arr[8].start == 19);
  cr_expect(result_arr[8].end == 25);
  cr_expect(result_arr[9].token == WHITESPACE);
  cr_expect(result_arr[9].start == 25);
  cr_expect(result_arr[9].end == 26);
  cr_expect(result_arr[10].token == AMP_CMD);
  cr_expect(result_arr[10].start == 26);
  cr_expect(result_arr[10].end == 29);
  cr_expect(result_arr[11].token == WHITESPACE);
  cr_expect(result_arr[11].start == 29);
  cr_expect(result_arr[11].end == 30);
  cr_expect(result_arr[12].token == ARG);
  cr_expect(result_arr[12].start == 30);
  cr_expect(result_arr[12].end == 33);
}

Test(tokenizeLine, tokenizes_asterisks) {
  char* line = "ls s* * -a";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 8);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 2);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 2);
  cr_expect(result_arr[1].end == 3);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 3);
  cr_expect(result_arr[2].end == 4);
  cr_expect(result_arr[3].token == ASTERISK);
  cr_expect(result_arr[3].start == 4);
  cr_expect(result_arr[3].end == 5);
  cr_expect(result_arr[4].token == WHITESPACE);
  cr_expect(result_arr[4].start == 5);
  cr_expect(result_arr[4].end == 6);
  cr_expect(result_arr[5].token == ASTERISK);
  cr_expect(result_arr[5].start == 6);
  cr_expect(result_arr[5].end == 7);
  cr_expect(result_arr[6].token == WHITESPACE);
  cr_expect(result_arr[6].start == 7);
  cr_expect(result_arr[6].end == 8);
  cr_expect(result_arr[7].token == ARG);
  cr_expect(result_arr[7].start == 8);
  cr_expect(result_arr[7].end == 10);
}

Test(tokenizeLine, if_asterisks_in_cmd_just_includes_to_command) {
  char* line = "ls*op -a && so*cmd *  arg*";
  token_index_arr result = tokenizeLine(line);
  token_index* result_arr = result.arr;

  cr_expect(result.len == 12);
  cr_expect(result_arr[0].token == CMD);
  cr_expect(result_arr[0].start == 0);
  cr_expect(result_arr[0].end == 5);
  cr_expect(result_arr[1].token == WHITESPACE);
  cr_expect(result_arr[1].start == 5);
  cr_expect(result_arr[1].end == 6);
  cr_expect(result_arr[2].token == ARG);
  cr_expect(result_arr[2].start == 6);
  cr_expect(result_arr[2].end == 8);
  cr_expect(result_arr[3].token == WHITESPACE);
  cr_expect(result_arr[3].start == 8);
  cr_expect(result_arr[3].end == 9);
  cr_expect(result_arr[4].token == AMPAMP);
  cr_expect(result_arr[4].start == 9);
  cr_expect(result_arr[4].end == 11);
  cr_expect(result_arr[5].token == WHITESPACE);
  cr_expect(result_arr[5].start == 11);
  cr_expect(result_arr[5].end == 12);
  cr_expect(result_arr[6].token == AMP_CMD);
  cr_expect(result_arr[6].start == 12);
  cr_expect(result_arr[6].end == 18);
  cr_expect(result_arr[7].token == WHITESPACE);
  cr_expect(result_arr[7].start == 18);
  cr_expect(result_arr[7].end == 19);
  cr_expect(result_arr[8].token == ASTERISK);
  cr_expect(result_arr[8].start == 19);
  cr_expect(result_arr[8].end == 20);
  cr_expect(result_arr[10].token == ARG);
  cr_expect(result_arr[10].start == 22);
  cr_expect(result_arr[10].end == 25);
  cr_expect(result_arr[11].token == ASTERISK);
  cr_expect(result_arr[11].start == 25);
  cr_expect(result_arr[11].end == 26);
}

Test(tokenizeLine, tokenizes_question_wildcard) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls Do??erfile");
  token_index_arr token = tokenizeLine(line);

  cr_expect(token.len == 6);
  cr_expect(token.arr[0].token == CMD);
  cr_expect(token.arr[1].token == WHITESPACE);
  cr_expect(token.arr[2].token == ARG);
  cr_expect(token.arr[3].token == QUESTION);
  cr_expect(token.arr[4].token == QUESTION);
  cr_expect(token.arr[5].token == ARG);
  free(line);
}

Test(tokenizeLine, redirections_with_whitespace_filename) {
  char* line = calloc(512, sizeof(char));
  strcpy(line, "ls 'one whole arg' > some\\ file");
  token_index_arr token = tokenizeLine(line);

  cr_expect(token.len == 7);
  logger(integer, &token.len);
  cr_expect(token.arr[0].token == CMD);
  cr_expect(token.arr[1].token == WHITESPACE);
  cr_expect(token.arr[2].token == ARG);
  cr_expect(token.arr[3].token == WHITESPACE);
  cr_expect(token.arr[4].token == GREAT);
  cr_expect(token.arr[5].token == WHITESPACE);
  cr_expect(token.arr[6].token == ARG);
  for (int i = 0; i < token.len; i++) {
    logger(integer, &token.arr[i].token);
    logger(string, "|");
  }
  logger(string, "\n");
  free(line);
}
