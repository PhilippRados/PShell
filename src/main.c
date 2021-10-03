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
#include <dirent.h>
#include <sys/ioctl.h>

#define BACKSPACE 127
#define TAB 9
#define CLEAR_LINE printf("%c[2K", 27);
#define CLEAR_BELOW_CURSOR printf("%c[0J",27);
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

bool isInPath(char* line, string_array PATH_BINS){
  for (int i = 0; i < PATH_BINS.len;i++){
    if (strcmp(PATH_BINS.values[i],line) == 0){
      return true;
    }
  }
  return false;
}

void backspaceLogic(char** line, int* i, const int prompt_len){
  if (strlen(*line) > 0 && i >= 0){
    *line = removeCharAtPos(*line,*i);

    if (*i > 0){
      *i -= 1;
    }
  }
}

void upArrowPress(int* history_index, char** line, const history_array* command_history){
 if (*history_index < command_history->len){
    *history_index += 1;
    memset(*line,0,strlen(*line));
    strcpy(*line,*command_history->values[*history_index - 1].values);

    for (int i = 1; i < command_history->values[*history_index - 1].len;i++){
      strcat(*line," ");
      strcat(*line,command_history->values[*history_index - 1].values[i]);
    }
  };
}

void downArrowPress(int* history_index, char** line, const history_array* command_history){
 if(*history_index > 1){
    *history_index -= 1;
    memset(*line,0,strlen(*line));
    strcpy(*line,*command_history->values[*history_index - 1].values);

    for (int i = 1; i < command_history->values[*history_index - 1].len;i++){
      strcat(*line," ");
      strcat(*line,command_history->values[*history_index - 1].values[i]);
    }
  } else if (*history_index > 0){
    *history_index -= 1;
    memset(*line,0,strlen(*line));
  };
}

bool typedLetter(char** line, const char c, const int i){
  bool cursor_moved = false;
  if (strlen(*line) == 0 && c == 32){
    return false;
  }

  if (c < 0 || c > 127){
    getch();
  } else if (i == strlen(*line)){
    (*line)[i] = c;
    cursor_moved = true;
  } else if (insertCharAtPos(*line,i,c)){
      cursor_moved = true;
  }

  return cursor_moved;
}

void arrowPress(char** line,int* i, int* history_index,  const history_array* command_history, const char value){
  switch(value) {
    case 'A':
      upArrowPress(history_index, line, command_history);

      *i = strlen(*line);
      break;

    case 'B':
      downArrowPress(history_index, line, command_history);

      *i = strlen(*line);
      break;

    case 'C':{
      *i = (*i < strlen(*line)) ? (*i) + 1 : *i;
      break;
    }

    case 'D':{
      *i = (*i > 0) ? (*i) - 1 : *i;
      break;
    }
  }
}

string_array filterBinaries(const char* line, const string_array PATH_BINS){
  int buf_size = 24;
  int realloc_index = 1;
  char** matching_binaries = calloc(buf_size,sizeof(char*));
  string_array result;
  int j = 0;

  for (int i = 0; i < PATH_BINS.len; i++){
    if (strncmp(PATH_BINS.values[i],line,strlen(line)) == 0){
      if (j >= (realloc_index * buf_size)){
        realloc_index++;
        matching_binaries = realloc(matching_binaries,realloc_index * buf_size * sizeof(char*));
      }
      matching_binaries[j] = calloc(strlen(PATH_BINS.values[i]) + 1,sizeof(char));
      strcpy(matching_binaries[j],PATH_BINS.values[i]);
      j++;
    }
  }
  result.values = matching_binaries;
  result.len = j;

  return result;
}


string_array checkForCommandAutoComplete(const string_array command_line,const string_array PATH_BINS){
  string_array possible_autocomplete = {
    .len = 0
  };
  if (command_line.len == 1){
    possible_autocomplete = filterBinaries(command_line.values[0],PATH_BINS);
  }
  
  return possible_autocomplete;
}

coordinates getTerminalSize(){
  coordinates size;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  size.x = w.ws_col;
  size.y = w.ws_row;

  return size;
}

int getLongestWordInArray(const string_array array){
  int longest = 0;
  int currrent_len = 0;

  for (int i = 0; i < array.len; i++){
    currrent_len = strlen(array.values[i]);
    if (currrent_len > longest){
      longest = currrent_len;
    }
  }
  
  return longest;
}

char* readLine(string_array PATH_BINS,char* directories,history_array* command_history){
  char c;
  char* line = calloc(BUFFER,sizeof(char));
  string_array possible_autocomplete;
  int i = 0;
  int history_index = 0;
  coordinates cursor_pos = getCursorPos();
  int prompt_len = strlen(directories) + 4;
  bool interactive_mode = false;
  int tab_index = -1;

  while((c = getch())){
    if (c == '\n'){
      if (interactive_mode && tab_index != -1){
        strcpy(line,possible_autocomplete.values[tab_index]);
        i = strlen(line);

        interactive_mode = false;
        tab_index = -1;
      } else {
        break;
      }
    } else if (c == BACKSPACE){
      backspaceLogic(&line,&i,prompt_len);

      interactive_mode = false;
      tab_index = -1;
    } else if (c == '\033'){
      getch();
      int value = getch();
      arrowPress(&line,&i,&history_index,command_history,value);

    } else if (c == TAB){
      if (i > 0){
        int format_width = getLongestWordInArray(possible_autocomplete) + 2;
        int terminal_width = getTerminalSize().x;
        int col_size = terminal_width / format_width;
        int row_size = possible_autocomplete.len / col_size;
        int j = 0;
        bool running = true;

        if (possible_autocomplete.len == 1){
          strcpy(line,possible_autocomplete.values[0]);
          i = strlen(line);

          interactive_mode = false;
          tab_index = -1;
        } else {
          if (tab_index < possible_autocomplete.len - 1){
            tab_index++;
          } else {
            tab_index = 0;
          }
          while (running){
            printf("\n");
            for (int x = 0; x < col_size; x++){
              if (j >= possible_autocomplete.len){
                running = false;
                break;
              }

              if (tab_index == j){
                int diff_len = strlen(possible_autocomplete.values[j]) - format_width;
                printColor(possible_autocomplete.values[j],GREEN);
                printf("%-*s",diff_len,"");
              } else { 
                printf("%-*s",format_width,possible_autocomplete.values[j]);
              }
              j++;
            }
          }
          interactive_mode = true;
        }
      } else {
        CLEAR_BELOW_CURSOR;
      }
    } else {
      if (typedLetter(&line, c, i)){
        i++;
        interactive_mode = false;
        tab_index = -1;
      }
    }

    if (i > 0){
      string_array command_line = splitString(line,' ');
      possible_autocomplete = checkForCommandAutoComplete(command_line,PATH_BINS);

      if (!interactive_mode){
        CLEAR_LINE;
        CLEAR_BELOW_CURSOR;
        printf("\r");
        printPrompt(directories,CYAN);

        isInPath(command_line.values[0],PATH_BINS) ? printColor(command_line.values[0],GREEN) : printColor(command_line.values[0],RED);
        for (int i = 1; i < command_line.len; i++){
          printf(" %s",command_line.values[i]);
        }
      }
    } else {
      CLEAR_LINE;
      CLEAR_BELOW_CURSOR;
      printf("\r");
      printPrompt(directories,CYAN);
    }

    cursor_pos.x = i + prompt_len;
    moveCursor(cursor_pos);
  }
  printf("\n");
  return line;
}

void printColor(char* string,color color){
  char command[13];

	sprintf(command, "%c[%d;%d;%dm", 0x1B, color.attr, color.fg, color.bg);
	printf("%s", command);
  printf("%s",string);

	sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
	printf("%s", command);
}

void printPrompt(char* dir,color color){
  char command[13];

  printColor(dir,color);
  printf(" ");

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

string_array getAllPathBinaries(string_array PATH_ARR){
  struct dirent* bin;
  string_array all_path_bins; 
  char** binaries = (char**)calloc(1024, sizeof(char*));
  int j = 0;
  int realloc_index = 1;

  for (int i = 0; i < PATH_ARR.len; i++){
    DIR* dr = opendir(PATH_ARR.values[i]);

    while((bin = readdir(dr)) != NULL){
      if (j >= (1024 * realloc_index)){
        realloc_index++;
        binaries = (char**)realloc(binaries,realloc_index * (1024 * (sizeof(char) * 24)));
        if (binaries == NULL){
          exit(0);
        }
      }
      if (!(strcmp(bin->d_name,".") == 0) && !(strcmp(bin->d_name,"..") == 0)){
        binaries[j] = (char*)calloc(strlen(bin->d_name) + 1,sizeof(char));
        strcpy(binaries[j],bin->d_name);
        j++;
      }
    }
    closedir(dr);
  }
  all_path_bins.values = binaries;
  all_path_bins.len = j;
  return all_path_bins;
}

int main(int argc, char* argv[]) {
  char *line;
  string_array splitted_line;
  int child_id;
  int status;
  char cd[512];
  char* test_file = "user_test.txt";
  history_array command_history = {
    .len = 0,
    .values = {}
  }; 
  string_array PATH_ARR = splitString(getenv("PATH"),':');
  string_array PATH_BINS = getAllPathBinaries(PATH_ARR);

  if (hasTestFlag(argc,argv)){
    removeFileContents(test_file);
  }

  write(STDOUT_FILENO,CLEAR_SCREEN,strlen(CLEAR_SCREEN));
  char* current_dir = getcwd(cd,sizeof(cd));
  char* last_two_dirs = getLastTwoDirs(current_dir);

  while (1){
    printf("\n");
    printPrompt(last_two_dirs,CYAN);

    line = readLine(PATH_BINS,last_two_dirs,&command_history);
    if (hasTestFlag(argc,argv)){
      logToTestFile(line,test_file);
    }
    if(strcmp(line,"q") == 0){
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
