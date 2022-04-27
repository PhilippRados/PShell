#include "util.h"

char* getLastTwoDirs(char*);
void printPrompt(const char* dir, color color);
string_array removeDots(string_array* array);
void stringToLower(char* string);
char* joinHistoryFilePath(char* home_dir, char* destination_file);
wildcard_groups_arr expandWildcardgroups(wildcard_groups_arr wildcards);
bool update(line_data* line_info, autocomplete_data* autocomplete_info, history_data* history_info,
            coordinates terminal_size, string_array PATH_BINS, coordinates* cursor_pos);
line_data* lineDataConstructor(int directory_len);
autocomplete_data* autocompleteDataConstructor();
history_data* historyDataConstructor(string_array* command_history, string_array global_command_history);
string_array concatenateArrays(const string_array one, const string_array two);
token_index_arr tokenizeLine(char* line);
void removeWhitespaceTokens(token_index_arr* tokenized_line);
bool isValidSyntax(token_index_arr tokenized_line);
string_array_token splitLineIntoSimpleCommands(char* line, token_index_arr tokenized_line);
string_array splitByTokens(char* line);
file_redirection_data parseForRedirectionFiles(string_array_token simple_commands_arr);
void stripRedirections(string_array* splitted_line, token_index_arr token);
wildcard_groups_arr groupWildcards(char* line, token_index_arr token);
void removeSlice_clone(char** line, int start, int end);
void replaceLineWithWildcards(char** line, wildcard_groups_arr wildcard_matches);
