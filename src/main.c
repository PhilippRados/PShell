#include "main.h"
#include <stdio.h>
#include <stdlib.h>

int BUFFER = 512;

char* readLine(){
  char c;
  char *line = malloc(sizeof(char) * BUFFER);
  int i = 0;

  while((c = getchar()) != '\n'){
    line[i] = c;
    i++;
  }
  return line;
}

int main() {
    char *line;
    while (1){
      printf("$ ");

      line = readLine();
      printf("%s\n",line);
      break;
  }
}

