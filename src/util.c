#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  int len;
  char** values;
} string_array;

string_array splitString(char* string_to_split,char delimeter){
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
  char* last_dir;
  int start = 1;
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
