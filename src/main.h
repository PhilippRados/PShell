#include "util.h"

char* getLastTwoDirs(char*);
string_array removeDots(string_array* array);
char* joinFilePath(char* home_dir, char* destination_file);
wildcard_groups_arr expandWildcardgroups(wildcard_groups_arr wildcards);
void removeWhitespaceTokens(token_index_arr* tokenized_line);
bool isValidSyntax(token_index_arr tokenized_line);
string_array_token splitLineIntoSimpleCommands(char* line, token_index_arr tokenized_line);
string_array splitByTokens(char* line, token_index_arr token);
file_redirection_data parseForRedirectionFiles(string_array simple_command, token_index_arr token);
void stripRedirections(string_array* splitted_line, token_index_arr token);
wildcard_groups_arr groupWildcards(char* line, token_index_arr token);
void replaceLineWithWildcards(char** line, wildcard_groups_arr wildcard_matches);
string_array splitString(const char*, char);
