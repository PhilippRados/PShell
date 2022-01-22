#include "util.h"

integer_tuple findDisplayIndices(int matching_commands_len, int cursor_diff,
                                 int index);
int shiftPromptIfOverlapTest(int current_cursor_height, int fuzzy_popup_height);
char* removeWhitespace(char* s1);
