#include "util.h"

int getAppendingIndex(char* line, char delimeter);
char* getCurrentWordFromLineIndex(string_array command_line, int cursor_x);
void removeSlice(char** line, int start);
void moveCursorIfShifted(coordinates* cursor_pos, int cursor_height_diff, int row_size);
string_array getAllMatchingFiles(char* current_dir_sub, char* removed_sub);
string_array filterMatching(char* line, const string_array PATH_BINS);
