#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

int BUFFER = 256;
const char *CLEAR_SCREEN = " \e[1;1H\e[2J";
const char *BASE_PATH = "/usr/bin/";

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
	printf("\n%s", command);
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
}


int main() {
  char *line;
  char **splitted_line;
  int child_id;
  int status;
  char cd[512];

  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  while (1){
    char* current_dir = getcwd(cd,sizeof(cd));
    printPrompt(getLastTwoDirs(current_dir),1,36,10);

    line = readLine();
    if(line[0] == 'q' && strlen(line) == 1){
      break;
    }
    splitted_line = splitString(line,' ');

    if (strcmp(splitted_line[0],"cd") == 0){
      chdir(splitted_line[1]);

    } else {
      pid_t pid = fork();
      if (pid == 0){

        int error = execvp(splitted_line[0],splitted_line);
        printf("%d\n",error);
      } else {
        waitpid(pid,NULL,0);
      }
    }
    free(splitted_line);
    free(line);
  }
}

