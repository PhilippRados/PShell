#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "colors.h"

#define BACKSPACE 127
#define CONTROL_F 6
#define TAB 9
#define ESCAPE '\033'
#define CLEAR_LINE printf("%c[2K", 27);
#define CLEAR_BELOW_CURSOR printf("%c[0J",27);
#define HIDE_CURSOR printf("\e[?25l");
#define ENABLE_CURSOR printf("\e[?25h");
#define CLEAR_SCREEN printf(" \e[1;1H\e[2J");

enum { HISTORY_SIZE = 512} ;

enum cursor_direction {
  cursor_up,
  cursor_down,
  cursor_left,
  cursor_right
};

enum logger_type {
  integer,
  string,
  character,
};

typedef struct coordinates {
  int x;
  int y;
} coordinates;

typedef struct {
  int len;
  char** values;
} string_array;

string_array splitString(const char*,char);
char* getLastTwoDirs(char*);
int isBuiltIn(char*);
int getch();
void printPrompt(const char* dir,color color);
int arrowHit();
long getFileSizeAtIndex(FILE* file,int index);
char* expectedAndReceived(char*,char*);
void printColor(const char* string,color color);
string_array concatenateArrays(const string_array one, const string_array two);
void moveCursor(coordinates new_pos);
char* popupFuzzyFinder(const string_array all_time_command_history);
coordinates getTerminalSize();
string_array removeDuplicates(string_array matching_commands);
void backspaceLogic(char** line, int* i);
char* removeCharAtPos(char* line,int x_pos);
void logger(enum logger_type type,void* message);
char* removeWhitespace(char* s1);
