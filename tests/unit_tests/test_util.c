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

Test(insertStringAtPos, insert_string_at_end) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "testing the waters");
  char* insert_string = " here";

  insertStringAtPos(line, insert_string, strlen(line));
  cr_expect(strcmp(line, "testing the waters here") == 0);

  free(line);
}

Test(insertStringAtPos, insert_string_in_middle) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "testing the waters");
  char* insert_string = "cold ";

  insertStringAtPos(line, insert_string, 12);

  cr_expect(strcmp(line, "testing the cold waters") == 0);

  free(line);
}

Test(insertStringAtPos, insert_string_at_start) {
  char* line = calloc(24, sizeof(char));
  strcpy(line, "~/testing");
  char* insert_string = "/Users";

  removeCharAtPos(line, 1);
  insertStringAtPos(line, insert_string, 0);

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
