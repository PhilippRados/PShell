#ifndef UTIL_H
#define UTIL_H

#include "colors.h"
#include "types.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <regex.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

#define BACKSPACE 127
#define CONTROL_F 6
#define TAB 9
#define ESCAPE '\033'
#define CLEAR_LINE printf("%c[2K", 27);
#define CLEAR_BELOW_CURSOR printf("%c[0J", 27);
#define HIDE_CURSOR printf("\e[?25l");
#define ENABLE_CURSOR printf("\e[?25h");
#define CLEAR_SCREEN printf(" \e[1;1H\e[2J");

enum { HISTORY_SIZE = 512 };
#define BUFFER 256

// ======= util.c functions ========

int getch();
void printColor(const char* string, color color, enum color_decorations color_decorations);
void moveCursor(coordinates new_pos);
fuzzy_result popupFuzzyFinder(const string_array all_time_command_history, const coordinates terminal_size,
                              int current_cursor_height, int line_row_count_with_autocomplete);
coordinates getTerminalSize();
string_array removeDuplicates(string_array* matching_commands);
void backspaceLogic(char* line, int* i);
char* removeCharAtPos(char* line, int x_pos);
void logger(enum logger_type type, void* message);
bool tabLoop(line_data* line_info, coordinates* cursor_pos, const string_array PATH_BINS,
             const coordinates terminal_size, token_index current_token);
coordinates getCursorPos();
void free_string_array(string_array* arr);
void insertStringAtPos(char** line, char* insert_string, int position);
int isDirectory(const char* path);
string_array copyStringArray(string_array arr);
bool insertCharAtPos(char* line, int index, char c);
bool inArray(char* value, string_array array);
string_array getAllFilesInDir(string_array* directory_array);
autocomplete_array fileComp(char* current_word);
void fileDirArray(string_array* filtered, char* current_dir_sub);
int getAppendingIndex(char* line, char delimeter);
file_string_tuple getFileStrings(char* current_word, char* current_path);
coordinates calculateCursorPos(coordinates terminal_size, coordinates cursor_pos, int prompt_len, int i);
int calculateRowCount(coordinates terminal_size, int prompt_len, int i);
char* shortenIfTooLong(char* word, int terminal_width);
bool isOnlyDelimeter(const char* string, char delimeter);
int firstNonDelimeterIndex(string_array splitted_line);
int getLongestWordInArray(const string_array array);
char* removeMultipleWhitespaces(char* string);
bool isExec(char* file);
token_index getCurrentToken(int line_index, token_index_arr tokenized_line);
void removeEscapesString(char** string);
void removeSlice(char** line, int start, int end);
token_index_arr tokenizeLine(char* line);
int isBuiltin(char* command, builtins_array builtins);
void replaceAliasesString(char** line);
char* readLine(string_array PATH_BINS, char* directories, string_array* command_history,
               const string_array global_command_history, builtins_array BUILTINS);
void printPrompt(const char* dir, color color);

#endif // !UTIL_H
