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

#define BACKSPACE 127
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

void logger(enum logger_type type,void* message){
  FILE* logfile = fopen("log.txt","a");

  switch (type){
    case integer: {
      fprintf(logfile, "%d", *((int *)message));
      break;
    }
    case string: {
      fprintf(logfile, "%s", (char *)message);
      break;
    }
    case character: {
      fprintf(logfile, "%c", *(char *)message);
      break;
    }
    default:{break;}
  }
  fclose(logfile);
}

coordinates getCursorPos(){
  char buf[1];
  char data[50];
  int y,x;
	char cmd[]="\033[6n";
  coordinates cursor_pos = {.x = -1,.y = -1};
  struct termios oldattr, newattr;

  tcgetattr( STDIN_FILENO, &oldattr );
  newattr = oldattr;
  newattr.c_lflag &= ~( ICANON | ECHO );
  newattr.c_cflag &= ~( CREAD );
  tcsetattr( STDIN_FILENO, TCSANOW, &newattr );

  write(STDIN_FILENO,cmd,sizeof(cmd));
  read(STDIN_FILENO,buf,1);

  if (*buf == '\033'){
    read(STDIN_FILENO,buf,1);
    if (*buf == '['){
      read(STDIN_FILENO,buf,1);
      for (int i = 0;*buf != 'R';i++){
        data[i] = *buf;
        read(STDIN_FILENO,buf,1);
      }
      // check if string matches expected data
      int valid = sscanf(data,"%d;%d",&y,&x);
      if (valid == 2){
        cursor_pos.x = x;
        cursor_pos.y = y;
      }
    }
	}
  tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
  return cursor_pos;
}

bool insertCharAtPos(char* line,int index,char c) {
  if (index >= 0 && index <= strlen(line)) {
    for (int i = strlen(line) - 1; i >= index;i--){
      line[i + 1] = line[i];
    }
    line[index] = c;
  } else {
    return false;
  }
  return true;
}

char* removeCharAtPos(char* line,int x_pos){
  for (int i = x_pos - 1; i < strlen(line); i++){
    line[i] = line[i + 1];
  }
  return line;
}

void moveCursor(coordinates new_pos){
  printf("\033[%d;%dH",new_pos.y,new_pos.x);
}

bool isInPath(char** line, char** PATH_ARR){
  return false;
}

char* readLine(char** PATH,char* directories,history_array *command_history){
  char c;
  char *line = calloc(BUFFER,sizeof(char));
  int i = 0;
  int history_index = 0;
  bool cursor_moved = false;
  coordinates new_pos = getCursorPos();
  int prompt_len = strlen(directories) + 4;
  new_pos.x = prompt_len;

  while((c = getch()) != '\n'){
    if (c == BACKSPACE){
      if (strlen(line) > 0 && i >= 0){
        line = removeCharAtPos(line,i);

        cursor_moved = true;
        if (new_pos.x > prompt_len){
          new_pos.x--;
        } 
        i = new_pos.x - prompt_len;
      }
    } else if (c == '\033'){
      getch();
      int value = getch();
      switch(value) {
        case 'A':
         if (history_index < command_history->len){
            history_index += 1;
            memset(line,0,strlen(line));
            strcpy(line,*command_history->values[history_index - 1].values);

            for (int i = 1; i < command_history->values[history_index - 1].len;i++){
              strcat(line," ");
              strcat(line,command_history->values[history_index - 1].values[i]);
            }
          };

          i = strlen(line);
          break;
        case 'B':
         if(history_index > 1){
            history_index -= 1;
            memset(line,0,strlen(line));
            strcpy(line,*command_history->values[history_index - 1].values);

            for (int i = 1; i < command_history->values[history_index - 1].len;i++){
              strcat(line," ");
              strcat(line,command_history->values[history_index - 1].values[i]);
            }
          } else if (history_index > 0){
            history_index -= 1;
            memset(line,0,strlen(line));
          };
          i = strlen(line);
          break;
        case 'C':{
          cursor_moved = true;
          new_pos.x = (new_pos.x < (prompt_len + strlen(line))) ? new_pos.x + 1 : new_pos.x;
          i = new_pos.x - prompt_len;
          break;
        }
        case 'D':{
          cursor_moved = true;
          new_pos.x = (new_pos.x > prompt_len) ? new_pos.x - 1 : new_pos.x;
          i = new_pos.x - prompt_len;
          break;
        }
      }
    } else {
      if ((c - '0') < 0 || (c - '0') > 127){
        getch();
        cursor_moved = true;
      } else if (i == strlen(line)){
        line[i] = c;
        cursor_moved = true;
        i++;
        new_pos.x = i + prompt_len;
      } else {
        if (insertCharAtPos(line,i,c)){
          cursor_moved = true;
          i++;
          new_pos.x = i + prompt_len;
        }
      }
    }
    printf("%c[2K", 27);
    printf("\r");
    printPrompt(directories,CYAN);
    // if (lineInPath){color = green} else {color = red}
    printf("%s",line);
    if (cursor_moved){
      moveCursor(new_pos);
      cursor_moved = false;
    }
  }
  printf("\n");
  return line;
}

void printColor(char* string,color color){
  char command[13];

	sprintf(command, "%c[%d;%d;%dm", 0x1B, color.attr, color.fg, color.bg);
	printf("%s", command);
  printf("%s ",string);

	sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
	printf("%s", command);
}

void printPrompt(char* dir,color color){
  char command[13];

  printColor(dir,color);

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

void pipeOutputToFile(char* filename){
  int file = open(filename, O_WRONLY | O_CREAT,0777);

  int file2 = dup2(file,STDOUT_FILENO);
  close(file);
}

int runChildProcess(string_array splitted_line) {
  pid_t pid = fork();
  if (pid == 0){
    if (splitted_line.len >= 3) {
      if (strcmp(splitted_line.values[splitted_line.len - 2],">>") == 0){
        pipeOutputToFile(splitted_line.values[splitted_line.len - 1]);
      }
      splitted_line.values[splitted_line.len - 2] = NULL;
    }

    int error = execvp(splitted_line.values[0],splitted_line.values);
    if (error){
      printf("couldn't find command %s\n",splitted_line.values[0]);
      return false;
    }
  } else {
    waitpid(pid,NULL,0);
  }
  return true;
}

void push(string_array splitted_line,history_array *command_history){
  if (command_history->len > 0){
    for (int i = command_history->len; i > 0;i--){
      if(i <= HISTORY_SIZE){
        command_history->values[i] = command_history->values[i-1];
      }
    }
  }
  (command_history->len <= HISTORY_SIZE) ? command_history->len++ : command_history->len;
  command_history->values[0] = splitted_line;
}

bool arrCmp(string_array arr1, string_array arr2){
  if (arr1.len != arr2.len){
    return false;
  }
  for (int i = 0; i < arr1.len;i++){
    if (strcmp(arr1.values[i],arr2.values[i]) != 0){
      return false;
    }
  }
  return true;
}

void logToTestFile(char* line,char* filename){
  FILE* test_file = fopen(filename,"a");
  char *new_line = malloc(sizeof(line));

  strcpy(new_line,line);
  fwrite(strcat(new_line,"\n"),sizeof(char),strlen(line) + 1,test_file);
  fclose(test_file);
}

void removeFileContents(char* filename){
  FILE* test_file = fopen(filename,"w");
  fclose(test_file);
}

bool hasTestFlag(int argc, char* argv[]){
  if (argc > 1 && strcmp(argv[argc - 1],"-test") == 0){
    return true;
  } else {
  return false;
  }
}

int main(int argc, char* argv[]) {
  char *line;
  string_array splitted_line;
  int child_id;
  int status;
  char cd[512];
  history_array command_history = {
    .len = 0,
    .values = {}
  }; 
  string_array PATH_ARR = splitString(getenv("PATH"),';');
  char* test_file = "user_test.txt";

  if (hasTestFlag(argc,argv)){
    removeFileContents(test_file);
  }

  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  char* current_dir = getcwd(cd,sizeof(cd));
  char* last_two_dirs = getLastTwoDirs(current_dir);
  while (1){
    printf("\n");
    printPrompt(last_two_dirs,CYAN);

    line = readLine(PATH_ARR.values,last_two_dirs,&command_history);
    if (hasTestFlag(argc,argv)){
      logToTestFile(line,test_file);
    }
    if(strcmp(line,"q") == 0){
      printf("uwe");
      break;
    }
    if (strlen(line) > 0){
      splitted_line = splitString(line,' ');
      if (strcmp(splitted_line.values[0],"cd") == 0){
        chdir(splitted_line.values[1]);
        current_dir = getcwd(cd,sizeof(cd));
        last_two_dirs = getLastTwoDirs(current_dir);
        if (!arrCmp(command_history.values[0],splitted_line)){
          push(splitted_line,&command_history);
        }
      } else {
        runChildProcess(splitted_line);
        if (!arrCmp(command_history.values[0],splitted_line)){
          push(splitted_line,&command_history);
        }
      }
    }
  }
  free(splitted_line.values);
  free(line);
  free(PATH_ARR.values);
}
