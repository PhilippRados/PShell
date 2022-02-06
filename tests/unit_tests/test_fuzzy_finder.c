#include "../../src/fuzzy_finder.h"
#include <criterion/criterion.h>

Test(findDisplayIndices, if_matching_commands_less_than_fuzzy_height) {
  int matching_commands_len = 3;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 3);
}

Test(findDisplayIndices, if_matching_commands_more_than_fuzzy_height_but_index_less) {
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 7);
}

Test(findDisplayIndices, if_matching_commands_and_index_more_than_fuzzy_height) {
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 12;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 6);
  cr_expect(result.second == 13);
}

Test(findDisplayIndices, if_index_equals_fuzzy_height) {
  int matching_commands_len = 10;
  int cursor_height = 7;
  int index = 7;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 1);
  cr_expect(result.second == 8);
}

Test(shift_prompt_fuzzy, shift_when_equal) {
  int result = shiftPromptIfOverlapTest(11, 11);

  cr_expect(result == 1);
}

Test(shift_prompt_fuzzy, dont_shift_when_cursor_higher) {
  int result = shiftPromptIfOverlapTest(10, 11);

  cr_expect(result == -1);
}

Test(shift_prompt_fuzzy, shift_when_cursor_lower) {
  int result = shiftPromptIfOverlapTest(14, 10);

  cr_expect(result == 5);
}

Test(removing_whitespace, initial_string_didnt_change) {
  char* s1 = calloc(12, sizeof(char));
  strcpy(s1, "test ing");

  char* result = calloc(12, sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(s1, "test ing") == 0);
  free(s1);
  free(result);
}

Test(removing_whitespace, removing_single_whitespace) {
  char* s1 = calloc(12, sizeof(char));
  strcpy(s1, "test ing");

  char* result = calloc(12, sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
  free(s1);
  free(result);
}

Test(removing_whitespace, removing_multiple_whitespaces) {
  char* s1 = calloc(12, sizeof(char));
  strcpy(s1, "test    ing");

  char* result = calloc(12, sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
  free(s1);
  free(result);
}

Test(updateFuzzyFinder, when_pressing_enter_copies_current_match_to_line) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 2, .values = addr_one};
  char c = '\n';
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 1;
  int* i;

  bool result = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result == false);
  cr_expect(strcmp(line, "two") == 0);
  free(line);
  free(index);
}

Test(updateFuzzyFinder, when_pressing_backspace_deletes_last_char) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 2, .values = addr_one};
  char c = BACKSPACE;
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 1;
  int* i = calloc(1, sizeof(int));
  *i = strlen(line);

  bool result = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result == true);
  cr_expect(strcmp(line, "not thi") == 0);
  cr_expect(*index == 0);
  cr_expect(*i == strlen("not thi"));
  free(line);
  free(index);
  free(i);
}

Test(updateFuzzyFinder, when_escape_twice_exits_finder) {
  int fds[2];

  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }

  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "\033", sizeof("\033"));
  close(fds[1]);

  // Rest of the program thinks that stdin is from
  // pipe

  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 2, .values = addr_one};
  char c = ESCAPE;
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 1;
  int* i;

  bool result = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result == false);
  cr_expect(strcmp(line, "") == 0);
  free(line);
  free(index);
}

Test(updateFuzzyFinder, when_up_arrowpress_dec_index) {
  int fds[2];

  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }

  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "ZAZAZA", sizeof("ZAZAZA")); // Z is any char that is not ESC
  close(fds[1]);

  // Rest of the program thinks that stdin is from
  // pipe

  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 2, .values = addr_one};
  char c = ESCAPE;
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 2;
  int* i;

  bool result1 = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result1 == true);
  cr_expect(strcmp(line, "not this") == 0);
  cr_expect(*index == 1);

  bool result2 = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result2 == true);
  cr_expect(strcmp(line, "not this") == 0);
  cr_expect(*index == 0);

  bool result3 = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result3 == true);
  cr_expect(strcmp(line, "not this") == 0);
  cr_expect(*index == 0);
  free(line);
  free(index);
}

Test(updateFuzzyFinder, when_up_downarrow_inc_index_if_not_bottom) {
  int fds[2];

  if (pipe(fds) == 1) {
    perror("pipe");
    exit(1);
  }

  dup2(fds[0], 0);
  close(fds[0]);
  write(fds[1], "ZB", sizeof("ZB")); // Z is any char that is not ESC
  close(fds[1]);

  // Rest of the program thinks that stdin is from
  // pipe

  char* one = "one";
  char* two = "two";
  char* three = "three";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 3, .values = addr_one};
  char c = ESCAPE;
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 1;
  int* i;

  bool result1 = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result1 == true);
  cr_expect(strcmp(line, "not this") == 0);
  cr_expect(*index == 2);

  bool result2 = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result2 == true);
  cr_expect(strcmp(line, "not this") == 0);
  cr_expect(*index == 2);

  free(line);
  free(index);
}

Test(updateFuzzyFinder, when_typing_char_appends_to_line) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};
  string_array arr1 = {.len = 2, .values = addr_one};
  char c = 's';
  char* line = calloc(12, sizeof(char));
  strcpy(line, "not this");
  int* index = calloc(1, sizeof(int));
  *index = 1;
  int* i = calloc(1, sizeof(int));
  *i = strlen(line);

  bool result = updateFuzzyfinder(line, c, arr1, index, i);
  cr_expect(result == true);
  cr_expect(strcmp(line, "not thiss") == 0);
  cr_expect(*index == 0);
  cr_expect(*i == strlen("not thiss"));
  free(line);
  free(index);
  free(i);
}
