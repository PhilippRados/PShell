#include "main.h"

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

bool isInPath(char* line, string_array PATH_BINS){
  for (int i = 0; i < PATH_BINS.len;i++){
    if (strcmp(PATH_BINS.values[i],line) == 0){
      return true;
    }
  }
  return false;
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
  free_string_array(&command_line);

  if (autocomplete){
    printf("%s",&possible_autocomplete[strlen(line)]);
  }
}

char* readLine(string_array PATH_BINS,char* directories,string_array* command_history, const string_array global_command_history){
  const coordinates terminal_size = getTerminalSize();
  char c;
  char* line = calloc(BUFFER,sizeof(char));
  char* possible_autocomplete = calloc(BUFFER,sizeof(char));
  int i = 0;
  int history_index = 0;
  coordinates cursor_pos = getCursorPos();
  int prompt_len = strlen(directories) + 4;
  bool autocomplete = false;
  char temp;
  string_array all_time_command_history = concatenateArrays(*command_history, global_command_history);
  char* popup_result;
  string_array concatenated_history_commands;

  while((c = getch())){

    if (c == TAB && strlen(line) > 0){
      if ((temp = tabLoop(line, &cursor_pos, PATH_BINS, terminal_size, i)) != '\n'){
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
        if ((int)c == CONTROL_F){
          string_array* ref = &concatenated_history_commands;
          concatenated_history_commands = concatenateArrays(global_command_history, *command_history);
          popup_result = popupFuzzyFinder(concatenated_history_commands, terminal_size, cursor_pos.y);
          if (strcmp(popup_result, "") != 0){
            strcpy(line, popup_result);
          }
          free(popup_result);
          free_string_array(ref);
          i = strlen(line);

          if (cursor_pos.y >= (terminal_size.y * 0.85) - 2){
            cursor_pos.y = (terminal_size.y * 0.85) - 3;
            moveCursor(cursor_pos);
          } else {
            moveCursor(cursor_pos);
          }
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
  free(possible_autocomplete);
  free_string_array(&all_time_command_history);

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
  printColor(dir,color);
  printf(" ");
  printColor("\u2771 ", GREEN);
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

char* joinHistoryFilePath(char* home_dir, char* destination_file){
  char* home_dir_copied = calloc(strlen(home_dir) + strlen("/.psh_history") + 1, sizeof(char));
  strcpy(home_dir_copied, home_dir);

  char* file_path = strcat(home_dir_copied, "/.psh_history");

  return file_path;
}

string_array getAllHistoryCommands(){
  string_array result = {.len = 0, .values = calloc(512, sizeof(char*))};
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_read = fopen(file_path,"r");
  free(file_path);
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

    result.values[i] = calloc(strlen(buf),sizeof(char));
    strncpy(result.values[i], buf, strlen(buf) - 1);
    i++;
  }
  result.len = i;

  free(buf);
  fclose(file_to_read);
  return result;
}

void writeSessionCommandsToGlobalHistoryFile(string_array command_history){
  string_array history_commands = getAllHistoryCommands();
  char* file_path = joinHistoryFilePath(getenv("HOME"), "/.psh_history");
  FILE* file_to_write = fopen(file_path, "a");
  free(file_path);

  if (file_to_write == NULL){
    logger(string, "error\n");
    return;
  }

  for (int i = 0; i < command_history.len; i++){
    if (!inArray(command_history.values[i], history_commands)){
      fprintf(file_to_write, "%s\n", command_history.values[i]);
    }
  }

  fclose(file_to_write);
  free_string_array(&history_commands);
}

int main(int argc, char* argv[]) {
  char *line;
  string_array splitted_line;
  char cd[512];
  string_array command_history = {
    .len = 0,
    .values = calloc(HISTORY_SIZE,sizeof(char*))
  }; 
  string_array PATH_ARR = splitString(getenv("PATH"),':');
  string_array all_files_in_dir = getAllFilesInDir(&PATH_ARR);
  free_string_array(&PATH_ARR);
  string_array removed_dots = removeDots(&all_files_in_dir);
  string_array PATH_BINS = removeDuplicates(&removed_dots);
  string_array global_command_history = getAllHistoryCommands();

  char* current_dir = getcwd(cd,sizeof(cd));
  char* last_two_dirs = getLastTwoDirs(current_dir);

  CLEAR_SCREEN;

  while (1){
    printf("\n");
    printPrompt(last_two_dirs,CYAN);

    line = readLine(PATH_BINS,last_two_dirs,&command_history, global_command_history);
    if(strcmp(line,"q") == 0){
      free(line);
      break;
    }
    if (strlen(line) > 0){
      splitted_line = splitString(line,' ');
      if (strcmp(splitted_line.values[0],"cd") == 0){
        chdir(splitted_line.values[1]);
        current_dir = getcwd(cd,sizeof(cd));
        free(last_two_dirs);
        last_two_dirs = getLastTwoDirs(current_dir);

        if (command_history.len == 0 || strcmp(command_history.values[0],line) != 0){
          push(line,&command_history);
        }
        free_string_array(&splitted_line);
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
      free_string_array(&splitted_line);
    }
    free(line);
  }

  writeSessionCommandsToGlobalHistoryFile(command_history);
  free_string_array(&command_history);
  free_string_array(&PATH_BINS);
  free(last_two_dirs);
  //free_string_array(&global_command_history);
  int chunk_size = (global_command_history.len / 512) + 1;
  int j = 0;
  logger(integer, &chunk_size);
  for (int i = 0; i < chunk_size; i++){
	for (int k = 0; k < 512; k++){
		free(global_command_history.values[j]);
		global_command_history.values[j] = NULL;
		j++;
	}
  }
  free(global_command_history.values);

	  
  /* free(splitted_line.values); */
  /* free(line); */
  /* free(PATH_ARR.values); */
}

