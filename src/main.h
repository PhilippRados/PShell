#include <stdio.h>

enum { HISTORY_SIZE = 10} ;

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

typedef struct {
  int attr;
  int fg;
  int bg;
} color;

const color WHITE = {
  .attr = 0,
  .fg = 37,
  .bg = 10,
};

const color RED = {
  .attr = 0,
  .fg = 31,
  .bg = 10,
};

const color GREEN = {
  .attr = 0,
  .fg = 32,
  .bg = 10,
};

const color CYAN = {
  .attr = 1,
  .fg = 36,
  .bg = 10,
};

string_array splitString(char*,char);
char* getLastTwoDirs(char*);
int isBuiltIn(char*);
int getch();
void printPrompt(char* dir,color color);
int arrowHit();
long getFileSizeAtIndex(FILE* file,int index);
char* expectedAndReceived(char*,char*);
void printColor(char* string,color color);
