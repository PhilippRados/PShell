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
#include <fcntl.h>

const int BUFFER = 256;
const char *CLEAR_SCREEN = " \e[1;1H\e[2J";

int getch(){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

typedef struct coordinates {
  int x;
  int y;
} coordinates;

coordinates getCursorPos(){
  char buf[20];
  char data[10];
  int x_pos;
	char cmd[]="\033[6n";
  int fd = open(ttyname(STDIN_FILENO), O_RDWR | O_NOCTTY);
  coordinates cursor_pos = {.x = 0,.y = 0};
  struct termios oldattr, newattr;

  tcgetattr( fd, &oldattr );
  newattr = oldattr;
  newattr.c_lflag &= ~( ICANON | ECHO );
  newattr.c_cflag &= ~( CREAD );
  tcsetattr( fd, TCSANOW, &newattr );

	if (isatty(fileno(stdin))){
		write(fd,cmd,sizeof(cmd));
		read(fd,buf ,sizeof(buf));

    int j = 0;
    for(int i = 2; buf[i] != 'R';i++){
      data[j] = buf[i];
        j++;
    }
    char** splitted = splitString(data,';');
    int x_pos = atoi(splitted[1]);
    int y_pos = atoi(splitted[0]);
    cursor_pos.x = x_pos;
    cursor_pos.y = y_pos;
	}
  tcsetattr( fd, TCSANOW, &oldattr );
  return cursor_pos;
}

char* removeCharAtPos(char* line,int x_pos){
  if (strlen(line) == 0) return false;
  for (int i = x_pos - 1; i < strlen(line); i++){
    line[i] = line[i + 1];
  }
  return line;
}

char* readLine(char* directories,history_array *command_history){
  char c;
  char *line = calloc(BUFFER,sizeof(char));
  int i = 0;
  int history_index = 0;

  while((c = getch()) != '\n'){
    if (c == 127){
      //backspace-logic
      int prompt_len = strlen(directories) + 4;
      coordinates cursor = getCursorPos();
      /* moveCursor(cursor); */
      removeCharAtPos(line,cursor.x - prompt_len);
      i--;

    } else if (c == '\033'){
      getch();
      int value = getch();
      switch(value) {
        case 'A':
         if (history_index < command_history->size){
            history_index += 1;
            strcpy(line,command_history->values[history_index - 1][0]);
          };
          break;
        case 'B':
         if(history_index > 1){
            history_index -= 1;
            strcpy(line,command_history->values[history_index - 1][0]);
          } else if (history_index > 0){
            history_index -= 1;
            memset(line,0,strlen(line));
          };
          break;
      }
    } else {
      line[i] = c;
      i++;
    }
    printf("%c[2K", 27);
    printf("\r");
    printPrompt(directories,1,36,10);
    printf("%s",line);
  }
  printf("\n");
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


void runIfBuiltin(char** splitted_line) {
  if (strcmp(splitted_line[0],"cd") == 0){
    chdir(splitted_line[1]);
  }
}

void push(char** splitted_line,history_array *command_history){
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
  history_array command_history = {
    .size = 0,
    .values = NULL
  };

  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  while (1){
    char* current_dir = getcwd(cd,sizeof(cd));
    char* last_two_dirs = getLastTwoDirs(current_dir);
    printf("\n");
    printPrompt(last_two_dirs,1,36,10);

    line = readLine(last_two_dirs,&command_history);
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
  free(splitted_line);
  free(line);
}
