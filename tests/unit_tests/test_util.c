#include "../../src/util.h"
#include <criterion/criterion.h>

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

Test(Split_func, only_whitespace) {
  string_array result = splitString("    ", ' ');

  cr_expect(result.len == 1);
  cr_expect(strcmp(result.values[0], "    ") == 0);
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

Test(getWordEndIndex, index_in_middle_of_word) {
  char* word = calloc(52, sizeof(char));
  strcpy(word, "testing Makefile works");
  int start = 9;

  int result = getWordEndIndex(word, start);

  cr_expect(result == 16);
  free(word);
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
