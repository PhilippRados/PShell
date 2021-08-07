#include "main.h"
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

const int BUFFER = 256;
enum { HISTORY_SIZE = 10} ;
const char *CLEAR_SCREEN = " \e[1;1H\e[2J";

typedef struct array {
  int size;
  char** values[HISTORY_SIZE];
} array;

int getch(int echo)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    if (echo){
      newattr.c_lflag &= ~( ICANON );
    } else {
      newattr.c_lflag &= ~( ICANON | ECHO );
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

char* readLine(){
  char c;
  char *line = calloc(BUFFER,sizeof(char));
  int i = 0;

  while((c = getchar()) != '\n'){
    line[i] = c;
    i++;
  }
  return line;
}

void printPrompt(char* dir,int attr, int fg,int bg){
  char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg, bg);
	printf("%s", command);
  printf("%s ",dir);

  //reset color
	sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
	printf("%s", command);

  //pick unicode char
  setlocale(LC_CTYPE, "");
  wchar_t arrow = 0x2771;

  //set unicode color and print
	sprintf(command, "%c[%d;%d;%dm", 0x1B, 1, 32, 10);
	printf("%s", command);
  wprintf(L"%lc ", arrow);

  //resetting colors
	sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
	printf("%s", command);
  /* free(dir); */
}

int arrowHit() {
  int pressed_key = 0;
  if (getch(false) == '\033') { // if the first value is esc
    getch(false); // skip the [
    int value = getch(false);
    switch(value) { // the real value
      case 'A':
        pressed_key = 1;
          break;
      case 'B':
        pressed_key = 2;
          break;
    }
  } 
  return pressed_key;
}

int runChildProcess(char** splitted_line) {
  pid_t pid = fork();
  if (pid == 0){

    int error = execvp(splitted_line[0],splitted_line);
    if (error){
      printf("couldn't find command %s\n",splitted_line[0]);
      return false;
    }
  } else {
    waitpid(pid,NULL,0);
  }
  return true;
}

void cycleCommandHistory(array *command_history,char* directories){
  int direction;
  char cd[120];
  int j = 0;

  printf("\r");
  printPrompt(directories,1,36,10);
  printf("%s ",command_history->values[j][0]);
  while(1){
    printf("\r");
    printPrompt(directories,1,36,10);
    if((direction = arrowHit())){
      switch (direction) {
        case 1: if (j < command_history->size){j++;};
          break;
        case 2: if(j >= 0){j--;};
          break;
      }
      if (j == -1){
        return;
      } else {
        printf("%s ",command_history->values[j][0]);
      }
    } else {
      return;
    }
  }
}

void runIfBuiltin(char** splitted_line) {
  if (strcmp(splitted_line[0],"cd") == 0){
    chdir(splitted_line[1]);
  }
}

void push(char** splitted_line,array *command_history){
  if (command_history->size > 0){
    for (int i = command_history->size; i >= 0;i--){
      if(i <= HISTORY_SIZE){
        command_history->values[i] = command_history->values[i-1];
      }
    }
  }
  (command_history->size <= HISTORY_SIZE) ? command_history->size++ : command_history->size;
  command_history->values[0] = splitted_line;
}

int main() {
  char *line;
  char **splitted_line;
  int child_id;
  int status;
  char cd[512];
  array command_history = {
    .size = 0,
    .values = NULL
  };

  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  while (1){
    char* current_dir = getcwd(cd,sizeof(cd));
    char* last_two_dirs = getLastTwoDirs(current_dir);
    printf("\n");
    printPrompt(last_two_dirs,1,36,10);
    int direction;

    if ((direction = arrowHit()) != 0) {
      if (direction == 1 && command_history.size > 0){
        cycleCommandHistory(&command_history,last_two_dirs);
      }
    } else {
      line = readLine();
      if(line[0] == 'q' && strlen(line) == 1){
        break;
      }
      splitted_line = splitString(line,' ');
      if (strcmp(splitted_line[0],"cd") == 0){
        chdir(splitted_line[1]);
        push(splitted_line,&command_history);
      } else {
          runChildProcess(splitted_line);
          push(splitted_line,&command_history);
      }
    }
  }
  free(splitted_line);
  free(line);
}
