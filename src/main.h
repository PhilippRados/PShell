#include <stdio.h>

enum { HISTORY_SIZE = 10} ;

typedef struct array {
  int size;
  char** values[HISTORY_SIZE];
} history_array;

char** splitString(char*,char);
char* getLastTwoDirs(char*);
int isBuiltIn(char*);
int getch();
char* cycleCommandHistory(const history_array *command_history,int* history_index);
void printPrompt(char* dir, int attr, int fg, int bg);
int arrowHit();
