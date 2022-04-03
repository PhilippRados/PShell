#include "util.h"

integer_tuple findDisplayIndices(int matching_commands_len, int cursor_diff, int index);
int shiftPromptIfOverlapTest(int current_cursor_height, int fuzzy_popup_height);
char* removeWhitespace(char* s1);
bool updateFuzzyfinder(char** line, char c, string_array matching_commands, int* index, int* i, int max_input_len);
