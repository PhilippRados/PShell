#include "util.h"

string_array getAllMatchingFiles(char* current_dir_sub, char* removed_sub);
string_array filterMatching(char* line, const string_array PATH_BINS);
tab_completion updateCompletion(autocomplete_array possible_tabcomplete, char* c, line_data* line_info,
                                int* tab_index, token_index current_token);
void removeDotFilesIfnecessary(char* current_word, autocomplete_array* possible_tabcomplete);
void escapeWhitespace(string_array* arr);
