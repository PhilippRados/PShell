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
