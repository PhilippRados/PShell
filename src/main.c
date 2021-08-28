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

coordinates getCursorPos(){
  char buf[20];
  char data[20];
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
    string_array splitted = splitString(data,';');
    int x_pos = atoi(splitted.values[1]);
    int y_pos = atoi(splitted.values[0]);
    cursor_pos.x = x_pos;
    cursor_pos.y = y_pos;
	}
  tcsetattr( fd, TCSANOW, &oldattr );
  return cursor_pos;
}

void insertCharAtPos(char* line,int index,char c) {
  if (index >= 0 && index <= strlen(line)) {
    for (int i = strlen(line) - 1; i >= index;i--){
      line[i + 1] = line[i];
    }
    line[index] = c;
  }
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
  bool hit_hori_arrow = false;
  coordinates new_pos;
  int prompt_len = strlen(directories) + 4;

  while((c = getch()) != '\n'){
    if (c == 127){
      coordinates cursor = getCursorPos();
      if (strlen(line) > 0 && i > 0){
        removeCharAtPos(line,cursor.x - prompt_len);
        i--;
      }
    } else if (c == '\033'){
      getch();
      int value = getch();
      switch(value) {
        case 'A':
         if (history_index < command_history->len){
            history_index += 1;
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
          hit_hori_arrow = true;
          coordinates current_pos = getCursorPos();
          new_pos.y =  current_pos.y;
          new_pos.x =  (current_pos.x < (prompt_len + strlen(line))) ? current_pos.x + 1 : current_pos.x;
          i = new_pos.x - prompt_len;
          break;
        }
        case 'D':{
          hit_hori_arrow = true;
          coordinates current_pos = getCursorPos();
          new_pos.y =  current_pos.y;
          new_pos.x =  (current_pos.x > prompt_len) ? current_pos.x - 1 : current_pos.x;
          i = new_pos.x - prompt_len;
          break;
        }
      }
    } else {
      if (i == strlen(line)){
        line[i] = c;
      } else {
        hit_hori_arrow = true;
        insertCharAtPos(line,i,c);
        new_pos.x = i + 1 + prompt_len;
      }
      i++;
    }
    printf("%c[2K", 27);
    printf("\r");
    printPrompt(directories,CYAN);
    // if (lineInPath){color = green} else {color = red}
    printf("%s",line);
    if (hit_hori_arrow){
      moveCursor(new_pos);
      hit_hori_arrow = false;
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

void logToTestFile(char* line){
      FILE* test_file = fopen("user_test.txt","a");
      char *new_line = malloc(sizeof(line));
      strcpy(new_line,line);
      fwrite(strcat(new_line,"\n"),sizeof(char),strlen(line) + 1,test_file);
      fclose(test_file);
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
  if (argc > 1 && strcmp(argv[argc - 1],"-test") == 0){
    FILE* test_file = fopen("user_test.txt","w");
    fclose(test_file);
  }


  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  while (1){
    char* current_dir = getcwd(cd,sizeof(cd));
    char* last_two_dirs = getLastTwoDirs(current_dir);
    printf("\n");
    printPrompt(last_two_dirs,CYAN);

    line = readLine(PATH_ARR.values,last_two_dirs,&command_history);
    if (argc > 1 && strcmp(argv[argc - 1],"-test") == 0){
      logToTestFile(line);
    }
    if(strcmp(line,"q") == 0){
      break;
    }
    if (strlen(line) > 0){
      splitted_line = splitString(line,' ');
      if (strcmp(splitted_line.values[0],"cd") == 0){
        chdir(splitted_line.values[1]);
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
}
