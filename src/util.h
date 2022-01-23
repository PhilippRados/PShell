#ifndef UTIL_H
#define UTIL_H

#include "colors.h"
#include "types.h"

#include <dirent.h>
#include <fcntl.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
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

// ======= util.c functions ========

string_array splitString(const char*, char);
int getch();
void printColor(const char* string, color color,
                enum color_decorations color_decorations);
void moveCursor(coordinates new_pos);
char* popupFuzzyFinder(const string_array all_time_command_history,
                       const coordinates terminal_size,
                       int current_cursor_height);
coordinates getTerminalSize();
string_array removeDuplicates(string_array* matching_commands);
void backspaceLogic(char** line, int* i);
char* removeCharAtPos(char* line, int x_pos);
void logger(enum logger_type type, void* message);
char tabLoop(char* line, coordinates* cursor_pos, const string_array PATH_BINS,
             const coordinates terminal_size, int line_index);
coordinates getCursorPos();
void free_string_array(string_array* arr);
void insertStringAtPos(char* line, char* insert_string, int position);
int getWordEndIndex(char* line, int start);
int isDirectory(const char* path);
string_array copyStringArray(string_array arr);

// ======= main.c functions ========

char* getLastTwoDirs(char*);
int isBuiltIn(char*);
void printPrompt(const char* dir, color color);
string_array concatenateArrays(const string_array one, const string_array two);
bool inArray(char* value, string_array array);
string_array getAllFilesInDir(string_array* directory_array);
string_array removeDots(string_array* array);
bool insertCharAtPos(char* line, int index, char c);
void stringToLower(char* string);
char* joinHistoryFilePath(char* home_dir, char* destination_file);
int isFile(const char* path);
void replaceAliases(string_array* splitted_line);

#endif // !UTIL_H
