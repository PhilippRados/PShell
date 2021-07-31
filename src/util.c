#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char** splitString(char* string_to_split,char delimeter){
    int start = 0;
    int j = 0;
    char** result = (char**)calloc(strlen(string_to_split),sizeof(char*));

    for (int i = 0;;i++){
        if (string_to_split[i] == delimeter || string_to_split[i] == '\0'){
            result[j] = (char*)calloc(i-start,sizeof(char));
            memcpy(result[j],&string_to_split[start], i - start);
            start = i + 1;
            j++;
        }
        if (string_to_split[i] == '\0') break;
    }
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
  char* last_two_dirs = (char*)calloc(i - second_to_last_slash,sizeof(char));
  strncpy(last_two_dirs,&cwd[second_to_last_slash],i - second_to_last_slash);

  return last_two_dirs;
}
