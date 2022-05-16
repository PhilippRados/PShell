#include "../../src/readline.h"
#include <criterion/criterion.h>

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
