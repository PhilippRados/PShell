#include "../../src/main.h"
#include "criterion/assert.h"
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

  bool result =
      update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);

  cr_expect(result == true);
  cr_expect(strcmp(line_info->line, "l") == 0);
  cr_expect(*line_info->i == 1);
  cr_expect(autocomplete_info->autocomplete == false);

  line_info->c = 's';
  bool result2 =
      update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 's';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);

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
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 1);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 0);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 0);

  line_info->c = ESCAPE;
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "ls") == 0);
  cr_expect(*line_info->i == 1);

  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 's';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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

  bool result =
      update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(result == true);
  cr_expect(strcmp(line_info->line, "yeyeye") == 0);
  cr_expect(*line_info->i == strlen("yeyeye"));
  cr_expect(history_info->history_index == 1);

  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(history_info->history_index == 2);

  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(history_info->history_index == 2);

  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "yeyeye") == 0);
  cr_expect(*line_info->i == strlen("yeyeye"));
  cr_expect(history_info->history_index == 1);

  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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

  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "test") == 0);
  cr_expect(*line_info->i == strlen("test"));
  cr_expect(autocomplete_info->autocomplete == true);

  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  line_info->c = 'e';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(autocomplete_info->autocomplete == true);
  cr_expect(strcmp(autocomplete_info->possible_autocomplete, "test") == 0);

  line_info->c = BACKSPACE;
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
  cr_expect(strcmp(line_info->line, "t") == 0);
  cr_expect(*line_info->i == strlen("t"));
  cr_expect(autocomplete_info->autocomplete == true);

  line_info->c = 'i';
  update(line_info, autocomplete_info, history_info, (coordinates){0, 0}, (string_array){0, NULL}, cursor_pos);
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

  logger(integer, &result.x);
  logger(string, ":");
  logger(integer, &result.y);
  logger(string, "\n");
  cr_expect(result.x == 45);
  cr_expect(result.y == 2);
}
