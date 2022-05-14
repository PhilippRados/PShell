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
