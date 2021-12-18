#include "main.h"

string_array splitString(const char* string_to_split,char delimeter){
  int start = 0;
  int j = 0;
  char** splitted_strings = (char**)calloc(strlen(string_to_split),sizeof(char*));
  string_array result; 

  for (int i = 0;;i++){
    if (string_to_split[i] == delimeter || string_to_split[i] == '\0'){
      splitted_strings[j] = (char*)calloc(i-start + 1,sizeof(char));
      memcpy(splitted_strings[j],&string_to_split[start], i - start);
      start = i + 1;
      j++;
    }
    if (string_to_split[i] == '\0') break;
  }
  result.len = j;
  result.values = splitted_strings;
  return result;
}

void free_string_array(string_array* arr){
  if (arr->values == NULL) return;
  for (int i = 0; i < arr->len; i++){
    free(arr->values[i]);
    arr->values[i] = NULL;
  }
  free(arr->values);
  arr->values = NULL;
}

char* getLastTwoDirs(char* cwd){
  int i = 1;
  int last_slash_pos = 0;
  int second_to_last_slash = 0;

  for(; cwd[i] != '\n' && cwd[i] != '\0';i++){
    if (cwd[i] == '/'){
      second_to_last_slash = last_slash_pos;
      last_slash_pos = i + 1;
    }
  }
  char* last_two_dirs = (char*)calloc(i - second_to_last_slash + 1,sizeof(char));
  strncpy(last_two_dirs,&cwd[second_to_last_slash],i - second_to_last_slash);

  return last_two_dirs;
}

long getFileSizeAtIndex(FILE* file,int index){
  char c;
  int i = 0;
  int line_num = 1;

  while ((c = getc(file)) != EOF){
    if (c == '\n'){
      line_num++;
    }
    if (line_num == index){
      break;
    }  
    i++;
  }
  return i + 1;
}

char* expectedAndReceived(char* expected, char* received){
  char* result = (char*)malloc(sizeof(char) * 100);
  sscanf(result,"\n\tExpected: %s\n\tReceived%s\n",expected,received);
  return result;
}

string_array concatenateArrays(const string_array one, const string_array two){
  string_array concatenated = {.values = calloc((one.len + two.len), sizeof(char*))};
  int i = 0;

  for (int k = 0; k < one.len; k++){
    concatenated.values[i] = calloc(strlen(one.values[k]) + 1, sizeof(char));
    strcpy(concatenated.values[i],one.values[k]);
    i++;
  }
  for (int j = 0; j < two.len; j++){
    concatenated.values[i] = calloc(strlen(two.values[j]) + 1, sizeof(char));
    strcpy(concatenated.values[i],two.values[j]);
    i++;
  }
  concatenated.len = i;

  return concatenated;
}

void moveCursor(coordinates new_pos){
  printf("\033[%d;%dH",new_pos.y,new_pos.x);
}


coordinates getTerminalSize(){
  coordinates size;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  size.x = w.ws_col;
  size.y = w.ws_row;

  return size;
}

bool inArray(char* value, string_array array){
  for (int i = 0; i < array.len; i++){
    if (strcmp(value, array.values[i]) == 0){
      return true;
    }
  }
  return false;
}

string_array removeDots(string_array* array){
  int j = 0;
  bool remove_index;
  char* not_allowed_dots[] = {".", "..", "./", "../"};
  string_array no_dots_array;
  no_dots_array.values = calloc(array->len, sizeof(char*));
  no_dots_array.len = 0;

  for (int i = 0; i < array->len; i++){
    remove_index = false;
    for (int k = 0; k < 4; k++){
      if (strcmp(array->values[i], not_allowed_dots[k]) == 0) {
        remove_index = true;
      }
    }
    if (!remove_index){
      no_dots_array.values[j] = calloc(strlen(array->values[i]) + 1, sizeof(char));
      strcpy(no_dots_array.values[j], array->values[i]);
      no_dots_array.len += 1;
      j++;
    }
  }
  free_string_array(array);
  return no_dots_array;
}

string_array removeDuplicates(string_array* matching_commands){
  int j = 0;
  string_array no_dup_array;
  no_dup_array.values = calloc(matching_commands->len, sizeof(char*));
  no_dup_array.len = 0;

  for (int i = 0; i < matching_commands->len; i++){
    if (!inArray(matching_commands->values[i], no_dup_array)){
      no_dup_array.values[j] = calloc(strlen(matching_commands->values[i]) + 1, sizeof(char));
      strcpy(no_dup_array.values[j], matching_commands->values[i]);
      no_dup_array.len += 1;
      j++;
    }
  }
  free_string_array(matching_commands);

  return no_dup_array;
}

char* removeCharAtPos(char* line,int x_pos){
  for (int i = x_pos - 1; i < strlen(line); i++){
    line[i] = line[i + 1];
  }
  return line;
}

void backspaceLogic(char** line, int* i){
  if (strlen(*line) > 0 && i >= 0){
    *line = removeCharAtPos(*line,*i);

    if (*i > 0){
      *i -= 1;
    }
  }
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

char* removeWhitespace(char* s1){
  char* stripped = calloc(strlen(s1) + 1, sizeof(char));
  int j = 0;

  for (int i = 0; i < strlen(s1); i++){
    if (s1[i] != ' '){
      stripped[j] = s1[i];
      j++;
    }
  }

  return stripped;
}

integer_tuple findDisplayIndices(int matching_commands_len, int cursor_diff, int index){
  int start = 0;
  int end = (matching_commands_len < cursor_diff) ? matching_commands_len : cursor_diff;
  
  if (index >= cursor_diff){
    start = index - cursor_diff + 1;
    end = index + 1;
  }

  integer_tuple result = {
    .one = start,
    .second = end,
  };

  return result;
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

void moveCursorIfShifted(coordinates* cursor_pos, int cursor_height_diff, int row_size){
  if (cursor_height_diff <= row_size || cursor_height_diff == 0){
    cursor_pos->y = cursor_pos->y - (row_size - cursor_height_diff);
    moveCursor(*cursor_pos);
  } else {
    moveCursor(*cursor_pos);
  }
}

int shiftPromptIfOverlapTest(int current_cursor_height, int fuzzy_popup_height){
  if (current_cursor_height < fuzzy_popup_height) return -1;
  int j = 0;

  for (int i = fuzzy_popup_height; i <= current_cursor_height; i++){
    j++;
  }
  return j;
}

string_array getAllFilesInDir(string_array* directory_array){
  struct dirent* file;
  string_array all_path_files; 
  char** files = (char**)calloc(1024, sizeof(char*));
  int j = 0;
  int realloc_index = 1;

  for (int i = 0; i < directory_array->len; i++){
    DIR* dr = opendir(directory_array->values[i]);

    while((file = readdir(dr)) != NULL){
      if (j >= (1024 * realloc_index)){
        realloc_index++;
        files = (char**)realloc(files,realloc_index * (1024 * (sizeof(char) * 24)));
        if (files == NULL){
          exit(0);
        }
      }
      files[j] = (char*)calloc(strlen(file->d_name) + 1,sizeof(char));
      strcpy(files[j],file->d_name);
      j++;
    }
    closedir(dr);
  }
  all_path_files.values = files;
  all_path_files.len = j;

  //free_string_array(directory_array);
  return all_path_files;
}

int getAppendingIndex(char* line, char delimeter){
  int j = 0;
  for (int i = strlen(line) - 1; i > 0; i--){
    if (line[i] == delimeter) return j;
    j++;
  }
  return -1;
}

string_array filterMatching(const char* line, const string_array PATH_BINS){
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

string_array getAllMatchingFiles(char* current_dir_sub, char* removed_sub){
  char* temp_sub = calloc(strlen(current_dir_sub) + 1, sizeof(char));
  strcpy(temp_sub, current_dir_sub);

  string_array current_dir_array = { .len = 1, .values = &temp_sub};
  string_array all_files_in_dir = getAllFilesInDir(&current_dir_array);
  string_array filtered = filterMatching(removed_sub,all_files_in_dir);

  free_string_array(&all_files_in_dir);
  free(temp_sub);

  return filtered;
}

char* getCurrentWordFromLineIndex(string_array command_line, int line_index){
  int current_pos = 0;
  char* result;
  for (int i = 0; i < command_line.len; i++){
    if (line_index >= current_pos && line_index <= (current_pos + strlen(command_line.values[i]))){
      result = calloc(strlen(command_line.values[i]) + 1, sizeof(char));
      strcpy(result, command_line.values[i]);
      break;
    }
    current_pos += strlen(command_line.values[i]) + 1;
  }

  return result;
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

char* insertStringAtPos(char* line, char* insert_string, int position){
  insertCharAtPos(line, position, '%');
  insertCharAtPos(line, position + 1, 's');

  char* new_line = calloc(strlen(line) + strlen(insert_string) + 1, sizeof(char));
  sprintf(new_line, line, insert_string);
  strcpy(line, new_line);
  free(new_line);

  return new_line;
}
