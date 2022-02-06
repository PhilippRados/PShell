#include "../../src/tab_complete.h"
#include <criterion/criterion.h>

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

Test(getCurrentWordFromLineIndex, cursor_in_middle_of_word) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};

  string_array arr1 = {.len = 2, .values = addr_one};

  char* result = getCurrentWordFromLineIndex(arr1, 6);
  cr_expect(strcmp(result, "tw") == 0);
}

Test(getCurrentWordFromLineIndex, cursor_at_beginning_of_line) {
  char* one = "one";
  char* two = "two";
  char* three = "three";
  char* addr_one[] = {one, two, three};

  string_array arr1 = {.len = 3, .values = addr_one};

  char* result = getCurrentWordFromLineIndex(arr1, 3);
  cr_expect(strcmp(result, "one") == 0);
}

Test(getCurrentWordFromLineIndex, cursor_at_end_of_line) {
  char* one = "one";
  char* two = "two";
  char* addr_one[] = {one, two};

  string_array arr1 = {.len = 2, .values = addr_one};

  char* result = getCurrentWordFromLineIndex(arr1, 6);
  cr_expect(strcmp(result, "tw") == 0);
}

Test(removeSlice, remove_end_if_cursor_middle) {
  char* word = calloc(52, sizeof(char));
  strcpy(word, "testing if Makefile works");
  int start = 14;

  removeSlice(&word, start);

  cr_expect(strcmp(word, "testing if Mak works") == 0);
  free(word);
}

Test(removeSlice, remove_nothing_cursor_end_of_current_word) {
  char* word = calloc(52, sizeof(char));
  strcpy(word, "testing if Makefile works");
  int start = 19;

  removeSlice(&word, start);

  cr_expect(strcmp(word, "testing if Makefile works") == 0);
  free(word);
}

Test(getAllMatchingFiles, should_match_only_one_file) {
  char* current_dir_sub = "/Users/philipprados/documents/coding/c/pshell";
  char* removed_sub = "Ma";

  string_array result = getAllMatchingFiles(current_dir_sub, removed_sub);

  cr_expect(result.len == 1);
  cr_expect(strcmp(result.values[0], "Makefile") == 0);

  free_string_array(&result);
}

Test(updateCompletion, exit_out_if_random_letter_press) {
  autocomplete_array possible_tab_complete;
  char* c = calloc(1, sizeof(char));
  *c = 'l';
  char* line;
  int line_index;
  int* tab_index;

  bool result = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result == false);
  free(c);
}

Test(updateCompletion, test_tabpress_when_single_command_match) {
  char* one = "testing";
  char* addr_one[] = {one};

  string_array arr1 = {.len = 1, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 3};
  char* c = calloc(1, sizeof(char));
  *c = TAB;
  char* line = calloc(10, sizeof(char));
  strcpy(line, "tesuwe");
  int line_index = 3;
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;

  bool result = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result == false);
  logger(string, line);
  cr_expect(strcmp(line, "testing") == 0);
  cr_expect(*c == 0);

  free(tab_index);
  free(line);
  free(c);
}

Test(updateCompletion, resets_tabindex_when_no_more_matches) {
  char* two = "tree";
  char* three = "trey";
  char* addr_one[] = {two, three};

  string_array arr1 = {.len = 2, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 2};
  char* c = calloc(1, sizeof(char));
  *c = TAB;
  char* line = calloc(10, sizeof(char));
  strcpy(line, "tre");
  int line_index = strlen(line);
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;

  bool result1 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result1 == true);
  cr_expect(strcmp(line, "tre") == 0);
  cr_expect(*c == TAB);
  cr_expect(*tab_index == 0);

  bool result2 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(*tab_index == 1);
  bool result3 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(*tab_index == 0);

  free(tab_index);
  free(line);
  free(c);
}

Test(updateCompletion, copies_current_tabindex_on_enter) {
  char* two = "tree";
  char* three = "trey";
  char* addr_one[] = {two, three};

  string_array arr1 = {.len = 2, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 3};
  char* c = calloc(1, sizeof(char));
  *c = '\n';
  char* line = calloc(10, sizeof(char));
  strcpy(line, "tre");
  int line_index = strlen(line);
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = 1;

  bool result1 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result1 == false);
  cr_expect(strcmp(line, "trey") == 0);
  cr_expect(*c == 0);
  cr_expect(*tab_index == 1);

  free(tab_index);
  free(line);
  free(c);
}

Test(updateCompletion, appends_correct_fileend_to_filecomp_on_dirs) {
  char* two = "tree";
  char* three = "trey";
  char* addr_one[] = {two, three};

  string_array arr1 = {.len = 2, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 2};

  char* c = calloc(1, sizeof(char));
  *c = '\n';
  char* line = calloc(32, sizeof(char));
  strcpy(line, "ls test/dir/why/tr");
  int line_index = strlen(line);
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = 0;

  bool result1 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result1 == false);
  cr_expect(strcmp(line, "ls test/dir/why/tree") == 0);
  cr_expect(*c == 0);
  cr_expect(*tab_index == 0);

  free(tab_index);
  free(line);
  free(c);
}

Test(updateCompletion, appends_correct_fileend_to_filecomp_on_dirs_when_cursor_middle) {
  char* two = "tree";
  char* addr_one[] = {two};

  string_array arr1 = {.len = 1, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 0};

  char* c = calloc(1, sizeof(char));
  *c = TAB;
  char* line = calloc(32, sizeof(char));
  strcpy(line, "ls test/dir/why/tr");
  int line_index = strlen("ls test/dir/");
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;

  bool result1 = updateCompletion(possible_tab_complete, c, line, line_index, tab_index);
  cr_expect(result1 == false);
  cr_expect(strcmp(line, "ls test/dir/tree") == 0);
  cr_expect(*c == 0);
  cr_expect(*tab_index == -1);

  free(tab_index);
  free(line);
  free(c);
}
