#include "../../src/tab_complete.h"
#include <criterion/criterion.h>

Test(removeSlice, remove_nothing_cursor_end_of_current_word) {
  char* word = calloc(52, sizeof(char));
  strcpy(word, "testing if Makefile works");
  int start = 19;

  removeSlice(&word, start, start);

  logger(string, word);
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
  int* tab_index;
  line_data* line_info;
  token_index token;

  tab_completion result = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result.continue_loop == false);
  free(c);
}

line_data* lineDataConstructor_for_testing(int directory_len) {
  line_data* line_info = calloc(1, sizeof(line_data));
  *line_info = (line_data){
      .line = calloc(BUFFER, sizeof(char)),
      .i = calloc(1, sizeof(int)),
      .prompt_len = directory_len + 4,
      .line_row_count_with_autocomplete = 0,
      .cursor_row = 0,
      .size = BUFFER * sizeof(char),
  };
  *line_info->i = 0;

  return line_info;
}

Test(updateCompletion, test_tabpress_when_single_command_match) {
  char* one = "testing";
  char* addr_one[] = {one};

  string_array arr1 = {.len = 1, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 3};
  char* c = calloc(1, sizeof(char));
  *c = TAB;
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;
  line_data* line_info = lineDataConstructor_for_testing(4);
  strcpy(line_info->line, "tesuwe");
  *line_info->i = 3;
  token_index token = {
      .token = CMD,
      .start = 0,
      .end = 6,
  };

  tab_completion result = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result.continue_loop == false);
  cr_expect(strcmp(line_info->line, "testing") == 0);
  cr_expect(result.successful);

  free(tab_index);
  free(line_info->line);
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
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;
  line_data* line_info = lineDataConstructor_for_testing(4);
  strcpy(line_info->line, "tre");
  *line_info->i = strlen(line_info->line);
  token_index token = {
      .token = CMD,
      .start = 0,
      .end = 3,
  };

  tab_completion result1 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result1.continue_loop == true);
  cr_expect(strcmp(line_info->line, "tre") == 0);
  cr_expect(result1.successful == false);
  cr_expect(*tab_index == 0);

  tab_completion result2 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(*tab_index == 1);
  tab_completion result3 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(*tab_index == 0);

  free(tab_index);
  free(line_info->line);
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
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = 1;
  line_data* line_info = lineDataConstructor_for_testing(4);
  strcpy(line_info->line, "tre");
  *line_info->i = strlen(line_info->line);
  token_index token = {
      .token = CMD,
      .start = 0,
      .end = 3,
  };

  tab_completion result1 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result1.continue_loop == false);
  cr_expect(strcmp(line_info->line, "trey") == 0);
  cr_expect(result1.successful);
  cr_expect(*tab_index == 1);

  free(tab_index);
  free(line_info->line);
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
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = 0;
  line_data* line_info = lineDataConstructor_for_testing(5);
  strcpy(line_info->line, "ls test/dir/why/tr");
  *line_info->i = strlen(line_info->line);
  token_index token = {
      .token = ARG,
      .start = 3,
      .end = strlen("test/dir/why/tr"),
  };

  tab_completion result1 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result1.continue_loop == false);
  cr_expect(strcmp(line_info->line, "ls test/dir/why/tree") == 0);
  cr_expect(result1.successful);
  cr_expect(*tab_index == 0);

  free(tab_index);
  free(line_info->line);
  free(c);
}

Test(updateCompletion, appends_correct_fileend_to_filecomp_on_dirs_when_cursor_middle) {
  char* two = "tree";
  char* addr_one[] = {two};

  string_array arr1 = {.len = 1, .values = addr_one};
  autocomplete_array possible_tab_complete = {.array = arr1, .appending_index = 0};

  char* c = calloc(1, sizeof(char));
  *c = TAB;
  int* tab_index = calloc(1, sizeof(int*));
  *tab_index = -1;
  line_data* line_info = lineDataConstructor_for_testing(4);
  strcpy(line_info->line, "ls test/dir/why/tr");
  *line_info->i = strlen("ls test/dir/");
  token_index token = {
      .token = ARG,
      .start = 3,
      .end = 3 + strlen("test/dir/why/tr"),
  };

  tab_completion result1 = updateCompletion(possible_tab_complete, c, line_info, tab_index, token);
  cr_expect(result1.continue_loop == false);
  cr_expect(result1.successful);
  cr_expect(strcmp(line_info->line, "ls test/dir/tree") == 0);
  cr_expect(*tab_index == -1);

  free(tab_index);
  free(line_info->line);
  free(c);
}

Test(removeDotFilesIfnecessary, should_remove_if_no_dot_at_first_char) {
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1, 1);
  strcpy(addr_one[0], "one");
  addr_one[1] = calloc(strlen(".two") + 1, 1);
  strcpy(addr_one[1], ".two");
  addr_one[2] = calloc(strlen("testi.ng") + 1, 1);
  strcpy(addr_one[2], "testi.ng");
  addr_one[3] = calloc(strlen("..") + 1, 1);
  strcpy(addr_one[3], "..");
  string_array arr1 = {.len = 4, .values = addr_one};
  autocomplete_array autocomplete = {.array = arr1, .appending_index = 0};
  char* current_word = "test";

  removeDotFilesIfnecessary(current_word, &autocomplete);

  cr_expect(strcmp(autocomplete.array.values[0], "one") == 0);
  cr_expect(strcmp(autocomplete.array.values[1], "testi.ng") == 0);
  cr_expect(autocomplete.array.len == 2);
  cr_expect(autocomplete.appending_index == 0);

  free_string_array(&autocomplete.array);
}

Test(getCurrentWordFromLineIndex, should_not_remove_if_current_word_starts_with_dot) {
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1, 1);
  strcpy(addr_one[0], "one");
  addr_one[1] = calloc(strlen(".two") + 1, 1);
  strcpy(addr_one[1], ".two");
  addr_one[2] = calloc(strlen("testi.ng") + 1, 1);
  strcpy(addr_one[2], "testi.ng");
  addr_one[3] = calloc(strlen("..") + 1, 1);
  strcpy(addr_one[3], "..");
  string_array arr1 = {.len = 4, .values = addr_one};
  autocomplete_array autocomplete = {.array = arr1, .appending_index = 0};
  char* current_word = "test/..";

  removeDotFilesIfnecessary(current_word, &autocomplete);

  cr_expect(strcmp(autocomplete.array.values[0], "one") == 0);
  cr_expect(strcmp(autocomplete.array.values[1], ".two") == 0);
  cr_expect(strcmp(autocomplete.array.values[2], "testi.ng") == 0);
  cr_expect(strcmp(autocomplete.array.values[3], "..") == 0);
  cr_expect(autocomplete.array.len == 4);
  cr_expect(autocomplete.appending_index == 0);

  free_string_array(&autocomplete.array);
}

Test(escapeWhitespace, when_whitespace_in_array_gets_escaped) {
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1, 1);
  strcpy(addr_one[0], "one");
  addr_one[1] = calloc(strlen("  two") + 1, 1);
  strcpy(addr_one[1], "  two");
  addr_one[2] = calloc(strlen("testi ng") + 1, 1);
  strcpy(addr_one[2], "testi ng");
  string_array arr1 = {.len = 3, .values = addr_one};

  escapeWhitespace(&arr1);
  cr_expect(strcmp(arr1.values[0], "one") == 0);
  cr_expect(strcmp(arr1.values[1], "\\ \\ two") == 0);
  cr_expect(strcmp(arr1.values[2], "testi\\ ng") == 0);
}
