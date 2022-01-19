#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

Test(findDisplayIndices, if_matching_commands_less_than_fuzzy_height){
  int matching_commands_len = 3;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 3);
}

Test(findDisplayIndices, if_matching_commands_more_than_fuzzy_height_but_index_less){
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 7);
}

Test(findDisplayIndices, if_matching_commands_and_index_more_than_fuzzy_height){
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 12;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 6);
  cr_expect(result.second == 13);
}

Test(findDisplayIndices, if_index_equals_fuzzy_height){
  int matching_commands_len = 10;
  int cursor_height = 7;
  int index = 7;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 1);
  cr_expect(result.second == 8);
}

Test(shift_prompt_fuzzy, shift_when_equal){
  int result = shiftPromptIfOverlapTest(11,11);

  cr_expect(result == 1);
}

Test(shift_prompt_fuzzy, dont_shift_when_cursor_higher){
  int result = shiftPromptIfOverlapTest(10,11);

  cr_expect(result == -1);
}

Test(shift_prompt_fuzzy, shift_when_cursor_lower){
  int result = shiftPromptIfOverlapTest(14,10);

  cr_expect(result == 5);
}

