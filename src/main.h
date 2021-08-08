#include <stdio.h>

char** splitString(char*,char);
char* getLastTwoDirs(char*);
int isBuiltIn(char*);
int getch();
void cycleCommandHistory();
void printPrompt(char* dir, int attr, int fg, int bg);
int arrowHit();
