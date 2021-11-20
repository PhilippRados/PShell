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
#define ESCAPE '\033'
#define CLEAR_LINE printf("%c[2K", 27);
#define CLEAR_BELOW_CURSOR printf("%c[0J",27);
#define HIDE_CURSOR printf("\e[?25l");
#define ENABLE_CURSOR printf("\e[?25h");
#define CLEAR_SCREEN printf(" \e[1;1H\e[2J");
const int BUFFER = 256;

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

void backspaceLogic(char** line, int* i){
  if (strlen(*line) > 0 && i >= 0){
    *line = removeCharAtPos(*line,*i);

    if (*i > 0){
      *i -= 1;
    }
  }
}

void upArrowPress(int* history_index, char** line, const string_array* command_history){
 if (*history_index < command_history->len){
    *history_index += 1;
    memset(*line,0,strlen(*line));
    strcpy(*line,command_history->values[*history_index -1]);
  };
}

void downArrowPress(int* history_index, char** line, const string_array* command_history){
 if(*history_index > 1){
    *history_index -= 1;
    memset(*line,0,strlen(*line));
    strcpy(*line,command_history->values[*history_index - 1]);

  } else if (*history_index > 0){
    *history_index -= 1;
    memset(*line,0,strlen(*line));
  };
}

bool typedLetter(char** line, const char c, const int i){
  bool cursor_moved = false;
  if (strlen(*line) == 0 && (c == 32 || c == TAB)){
    return false;
  }

  if (c < 0 || c > 127){
    getch();
  } else if (i == strlen(*line)){
    (*line)[i] = c;
    cursor_moved = true;
  } else if (insertCharAtPos(*line,i,c)){
      cursor_moved = true;
  }

  return cursor_moved;
}

void arrowPress(char** line,int* i, int* history_index, const bool autocomplete, const char* possible_autocomplete,const string_array* command_history, const char value){
  switch(value) {
    case 'A':
      upArrowPress(history_index, line, command_history);

      *i = strlen(*line);
      break;

    case 'B':
      downArrowPress(history_index, line, command_history);

      *i = strlen(*line);
      break;

    case 'C':{ // right-arrow
      if (autocomplete && strcmp(*line,possible_autocomplete) != 0){
        memset(*line,0,strlen(*line));
        strcpy(*line,possible_autocomplete);
        *i = strlen(*line);
      } else {
        *i = (*i < strlen(*line)) ? (*i) + 1 : *i;
      }
      break;
    }

    case 'D':{ // left-arrow
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

string_array filterHistory(const string_array concatenated, const char* line){
  char** possible_matches = calloc(512, sizeof(char*));
  int matches_num = 0;

  if (strlen(line) > 0){
    for (int i = 0; i < concatenated.len; i++){
      if (strncmp(line,concatenated.values[i],strlen(line)) == 0){
        possible_matches[matches_num] = calloc(strlen(concatenated.values[i]) + 1, sizeof(char));
        strcpy(possible_matches[matches_num], concatenated.values[i]);
        matches_num++;
      }         
    }
  }
  string_array result = {
    .values = possible_matches,
    .len = matches_num
  };

  return result;
}

bool filterHistoryForMatchingAutoComplete(const string_array concatenated, const char* line, char* possible_autocomplete){

  for (int i = 0; i < concatenated.len; i++){
    if (strncmp(line,concatenated.values[i],strlen(line)) == 0){
      strcpy(possible_autocomplete, concatenated.values[i]);

      return true;
    }         
  }

  return false;
}

void render(const char* line, const string_array command_history, const string_array PATH_BINS, const bool autocomplete, const char* possible_autocomplete){
  string_array command_line = splitString(line,' ');

  isInPath(command_line.values[0],PATH_BINS) ? printColor(command_line.values[0],GREEN) : printColor(command_line.values[0],RED);
  for (int i = 1; i < command_line.len; i++){
    printf(" %s",command_line.values[i]);
  }

  if (autocomplete){
    printf("%s",&possible_autocomplete[strlen(line)]);
  }
}

void tabRender(char* line, string_array possible_tabcomplete, int tab_index, string_array PATH_BINS){
  int format_width = getLongestWordInArray(possible_tabcomplete) + 2;
  int terminal_width = getTerminalSize().x;
  int col_size = terminal_width / format_width;
  int row_size = possible_tabcomplete.len / col_size;
  int j = 0;
  bool running = true;

  while (running){
    printf("\n");
    for (int x = 0; x < col_size; x++){
      if (j >= possible_tabcomplete.len){
        running = false;
        break;
      }

      if (tab_index == j){
        int diff_len = strlen(possible_tabcomplete.values[j]) - format_width;

        printColor(possible_tabcomplete.values[j],GREEN);
        printf("%-*s",diff_len,"");
      } else { 
        printf("%-*s",format_width,possible_tabcomplete.values[j]);
      }
      j++;
    }
  }
}

char tabLoop(char* line, const string_array PATH_BINS, const coordinates cursor_pos){
  char c = TAB;
  string_array possible_tabcomplete;
  int tab_index = -1;
  char answer;
  possible_tabcomplete = checkForCommandAutoComplete(splitString(line,' '),PATH_BINS);

  if (possible_tabcomplete.len > 30){
    printf("\nThe list of possible matches is %d. Do you want to print all of them? (y/n) ", possible_tabcomplete.len);
    answer = getch();

    moveCursor(cursor_pos);
    if (answer != 'y'){
      return '\n';
    }
  }
  do {
    CLEAR_BELOW_CURSOR;
    if (c == TAB){
      if (possible_tabcomplete.len == 1){
        strcpy(line,possible_tabcomplete.values[0]);

      } else {

        if (tab_index < possible_tabcomplete.len - 1){
          tab_index += 1;
        } else {
          tab_index = 0;
        }

        tabRender(line, possible_tabcomplete, tab_index, PATH_BINS);
        moveCursor(cursor_pos);
      }
    } else if (c == '\n'){
      strcpy(line, possible_tabcomplete.values[tab_index]);
      return c;

    } else {
      return c;
    }
  } while ((c = getch()));

  return 0;
}

void drawPopupBox(const coordinates terminal_size, const int width, const int height){
  CLEAR_SCREEN

  for (int row = 0; row < terminal_size.y; row++){
    if (row == (height / 2) || row == (terminal_size.y - (height / 2))){
      for (int i = 0; i < terminal_size.x; i++){
        if (i > (width / 2) && i < (terminal_size.x - (width / 2))){
          printf("\u2550");
        } else if (i == (width / 2)){
          if (row == (height / 2)){
            printf("\u2554");
          } else {
            printf("\u255A");
          }
        } else if (i == (terminal_size.x - (width / 2))){
          if (row == (height / 2)){
            printf("\u2557");
          } else {
            printf("\u255D");
          }
        } else {
          printf(" ");
        }
      }
    } else if (row > (height / 2) && row < (terminal_size.y - (height / 2))){
      for (int col = 0; col < terminal_size.x; col++){
        if (col == (width / 2) || col == (terminal_size.x - (width / 2))){
          printf("\u2551");
        } else {
          printf(" ");
        }
      }
    } else {
      printf("\n");
    }
  }
  coordinates bottom_box_pos = {
    .x = (width / 2) + 3,
    .y = terminal_size.y - (height / 2)
  };
  moveCursor(bottom_box_pos);
}

bool inArray(char* value, string_array array){
  for (int i = 0; i < array.len; i++){
    if (strcmp(value, array.values[i]) == 0){
      return true;
    }
  }
  return false;
}

string_array removeDuplicates(string_array matching_commands){
  int j = 0;
  string_array no_dup_array;
  no_dup_array.values = calloc(matching_commands.len, sizeof(char*));
  no_dup_array.len = 0;

  for (int i = 0; i < matching_commands.len; i++){
    if (!inArray(matching_commands.values[i], no_dup_array)){
      no_dup_array.values[j] = calloc(strlen(matching_commands.values[i]) + 1, sizeof(char));
      strcpy(no_dup_array.values[j], matching_commands.values[i]);
      no_dup_array.len += 1;
      j++;
    }
  }

  return no_dup_array;
}

void clearFuzzyWindow(coordinates initial_cursor_pos, int box_width, int box_height){
  for (int rows = initial_cursor_pos.x + 2; rows < box_width; rows++){
    for (int cols = initial_cursor_pos.y; cols < box_height; cols++){
      coordinates cursor = {.x = rows, .y = cols};
      moveCursor(cursor);
      printf(" ");
    }
  }

  moveCursor(initial_cursor_pos);
}

char* popupFuzzyFinder(const string_array all_time_command_history){
  char c;
  coordinates terminal_size = getTerminalSize();
  int width = terminal_size.x * 0.4;
  int height = terminal_size.y * 0.3;
  int index = 0;
  int i = 0;
  char* line = calloc(64, sizeof(char));

  drawPopupBox(terminal_size, width, height);

  coordinates cursor = {.x = (width / 2) + 3, .y = (height / 2)};
  moveCursor(cursor);
  printf("Fuzzy Find through past commands");

  coordinates initial_cursor_pos = {
    .x = cursor.x,
    .y = cursor.y + 2,
  };
  moveCursor(initial_cursor_pos);
  printf("\u2771 ");

  string_array matching_commands;
  matching_commands.len = 0;

  while ((c = getch())){
    clearFuzzyWindow(initial_cursor_pos, terminal_size.x - width, terminal_size.y - height);
    if (c == '\n'){
      if (matching_commands.len > 0){
        memset(line,0,strlen(line));
        strcpy(line, matching_commands.values[index]);
      }
      goto FINISH_LOOP;
    } else if (c == BACKSPACE){
      backspaceLogic(&line, &i);
    } else if (c == ESCAPE){
      if (getch() == ESCAPE){
        goto FINISH_LOOP;
      }
      int value = getch();

      if (value == 'A'){
        (index > 0) ? index-- : index;
      } else if (value == 'B'){
        (index < matching_commands.len - 1) ? index++ : index;
      }
    } else {
      if (strlen(line) < 63){
        index = 0;
        line[i] = c;
        i++;
      }
    }
    
    matching_commands = removeDuplicates(filterHistory(all_time_command_history, line));

    for (int j = 0; j < matching_commands.len; j++){
      coordinates drawing_pos = {
        .y = initial_cursor_pos.y + j + 1,
        .x = initial_cursor_pos.x + 2,
      };
      moveCursor(drawing_pos);

      if (j == index){
        printColor(matching_commands.values[j],HIGHLIGHT);
      } else {
        printf("%s", matching_commands.values[j]);
      }
    }

    moveCursor(initial_cursor_pos);
    printf("\u2771 %s", line);
  }
  FINISH_LOOP:

  CLEAR_SCREEN;
  printf("\n");

  return line;
}

char* readLine(string_array PATH_BINS,char* directories,string_array* command_history, const string_array global_command_history){
  char c;
  char* line = calloc(BUFFER,sizeof(char));
  char* possible_autocomplete = calloc(BUFFER,sizeof(char));
  int i = 0;
  int history_index = 0;
  coordinates cursor_pos = getCursorPos();
  int prompt_len = strlen(directories) + 4;
  bool autocomplete = false;
  char temp;

  while((c = getch())){
    string_array all_time_command_history = concatenateArrays(*command_history, global_command_history);

    if (c == TAB && strlen(line) > 0){
      if ((temp = tabLoop(line, PATH_BINS, cursor_pos)) != '\n'){
        c = temp;
      } else {
        c = -1;
      }
      i = strlen(line);
    }
    switch (c){
      case ('\n'):{
        goto ENDLOOP;
        break;
      }
      case (BACKSPACE): {
        backspaceLogic(&line,&i);

        break;
      }
      case (ESCAPE): {
        getch();
        int value = getch();
        arrowPress(&line,&i,&history_index,autocomplete,possible_autocomplete,command_history,value);
        break;
      }
      default: {
        if ((int)c == 6){ //control+f

          string_array concatenated_history_commands = concatenateArrays(global_command_history, *command_history);
          line = popupFuzzyFinder(concatenated_history_commands);
          i = strlen(line);
          moveCursor(cursor_pos);
        } else if ((int)c == 8){ //control+h
        } else if (c != -1 && typedLetter(&line, c, i)){
          i++;
        }
        break;
      }
    }

    cursor_pos.x = i + prompt_len;
    autocomplete = filterHistoryForMatchingAutoComplete(all_time_command_history,line, possible_autocomplete);

    CLEAR_LINE;
    CLEAR_BELOW_CURSOR;
    printf("\r");
    printPrompt(directories,CYAN);

    if (strlen(line) > 0){
      render(line, *command_history, PATH_BINS, autocomplete,possible_autocomplete);
    }

    moveCursor(cursor_pos);
  }
  ENDLOOP:

  printf("\n");
  return line;
}

void printColor(const char* string,color color){
  char command[13];

	sprintf(command, "%c[%d;%d;%dm", 0x1B, color.attr, color.fg, color.bg);
	printf("%s", command);
  printf("%s",string);

	sprintf(command, "%c[%d;%d;%dm", 0x1B, 0, 37, 10);
	printf("%s", command);
}

void printPrompt(const char* dir,color color){
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
  int file = open(filename, O_WRONLY | O_CREAT,0777);

  int file2 = dup2(file,STDOUT_FILENO);
  close(file);
}

int runChildProcess(string_array splitted_line) {
  pid_t pid = fork();

  if (pid == 0){
    int error = execvp(splitted_line.values[0],splitted_line.values);
    if (error){
      printf("couldn't find command %s\n",splitted_line.values[0]);
    }
  } else {
    return pid;
  }
  return false;
}

void push(char* line,string_array* command_history){
  if (command_history->len > 0){
    for (int i = command_history->len; i > 0;i--){
      if(i <= HISTORY_SIZE){
        command_history->values[i] = command_history->values[i-1];
      }
    }
  }
  (command_history->len <= HISTORY_SIZE) ? command_history->len++ : command_history->len;
  command_history->values[0] = calloc(strlen(line) + 1,sizeof(char));
  strcpy(command_history->values[0],line);
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

string_array getAllHistoryCommands(){
  string_array result = {.len = 0, .values = calloc(512, sizeof(char*))};
  char* home_dir = getenv("HOME");
  char* file_path = strncat(home_dir, "/.psh_history", strlen("/.psh_history"));
  FILE* file_to_read = fopen(file_path,"r");
  char* buf = calloc(64, sizeof(char));
  int line_len;
  unsigned long buf_size;
  int i = 0;
  int realloc_index = 1;

  if (file_to_read == NULL){
    return result;
  }

  while ((line_len = getline(&buf, &buf_size, file_to_read)) != -1){
    if (i >= (realloc_index * 512)){
      realloc_index++;
      result.values = realloc(result.values,realloc_index * 512 * sizeof(char*));
    }
    /* insertCharAtPos(buf, strlen(buf) - 1, '\0'); */
    /* buf[strlen(buf) - 1] = 0; */
    result.values[i] = calloc(strlen(buf),sizeof(char));
    strncpy(result.values[i], buf, strlen(buf) - 1);
    i++;
  }
  result.len = i;

  fclose(file_to_read);
  return result;
}

void writeSessionCommandsToGlobalHistoryFile(string_array command_history){
  char* home_dir = getenv("HOME"); // weird that home_dir now already includes .psh_history
  FILE* file_to_write = fopen(home_dir, "a");
  if (file_to_write == NULL){
    return;
  }

  for (int i = 0; i < command_history.len; i++){
    fprintf(file_to_write, "%s\n", command_history.values[i]);
  }

  fclose(file_to_write);
}

int main(int argc, char* argv[]) {
  char *line;
  string_array splitted_line;
  int child_id;
  int status;
  char cd[512];
  char* test_file = "user_test.txt";
  string_array command_history = {
    .len = 0,
    .values = calloc(HISTORY_SIZE,sizeof(char*))
  }; 
  string_array PATH_ARR = splitString(getenv("PATH"),':');
  string_array PATH_BINS = getAllPathBinaries(PATH_ARR);

  string_array global_command_history = getAllHistoryCommands();

  if (hasTestFlag(argc,argv)){
    removeFileContents(test_file);
  }

  CLEAR_SCREEN
  char* current_dir = getcwd(cd,sizeof(cd));
  char* last_two_dirs = getLastTwoDirs(current_dir);

  while (1){
    printf("\n");
    printPrompt(last_two_dirs,CYAN);

    line = readLine(PATH_BINS,last_two_dirs,&command_history, global_command_history);
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

        if (command_history.len == 0 || strcmp(command_history.values[0],line) != 0){
          push(line,&command_history);
        }
      } else {
        pid_t child;
        int wstatus;

        child = runChildProcess(splitted_line);

        if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          exit(EXIT_FAILURE);
        }

        if (command_history.len == 0 || strcmp(command_history.values[0],line) != 0){
          push(line,&command_history);
        }
      }
    }
  }

  writeSessionCommandsToGlobalHistoryFile(command_history);
  free(splitted_line.values);
  free(line);
  free(PATH_ARR.values);
}
