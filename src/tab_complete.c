#include "main.h"
#include <sys/stat.h>

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

void tabRender(string_array possible_tabcomplete, int tab_index, int col_size, int format_width){
  int j = 0;

  while (j < possible_tabcomplete.len){
    printf("\n");
    for (int x = 0; x < col_size; x++){
      if (j >= possible_tabcomplete.len){
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

bool isFile(char* path){
  struct stat path_stat;
  stat(path, &path_stat);

  return S_ISREG(path_stat.st_mode);
}

autocomplete_array checkForCommandAutoComplete(const string_array command_line,const string_array PATH_BINS){
  autocomplete_array possible_autocomplete = {
    .array.len = 0
  };
  if (command_line.len == 1){ // Command-completion
    string_array filtered = filterMatching(command_line.values[0],PATH_BINS);

    possible_autocomplete = (autocomplete_array){
      .tag = command,
      .array.values = filtered.values,
      .array.len = filtered.len
    };
  } else if (command_line.len > 1){ // File-completion
    char cd[256];
    char* current_dir = calloc(256, sizeof(char));
    char* current_path = strcat(getcwd(cd, sizeof(cd)), "/");

    current_dir = strcat(current_path, command_line.values[1]); // shouldnt be hardcoded in the future as file-comp. can happen anywhere
    char* current_dir_sub = calloc(strlen(current_dir) + 256, sizeof(char));

    char* removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir,'/')]);
    strncpy(current_dir_sub, current_dir, strlen(current_dir) - getAppendingIndex(current_dir, '/') - 1);

    string_array current_dir_array = { .len = 1, .values = &current_dir_sub, };
      
    char copy[256];
    string_array filtered = filterMatching(removed_sub,getAllFilesInDir(current_dir_array));
    int* appending_index = calloc(filtered.len + 1, sizeof(int));
    for (int i = 0; i < filtered.len; i++){
      strcat(current_dir_sub,"/");
      char* temp = strcpy(copy, strcat(current_dir_sub,filtered.values[i]));

      if (!isFile(temp)){ // Doesnt register directories bc strcat saves permanently
        logger(string, "is directory\n");
        filtered.values[i] = realloc(filtered.values[i], strlen(filtered.values[i]) + 2);
        filtered.values[i][strlen(filtered.values[i]) + 1] = '\0';
        filtered.values[i][strlen(filtered.values[i])] = '/';
      }
      memset(copy, 0,strlen(copy));
      filtered.values[i] = filtered.values[i];
      appending_index[i] = strlen(removed_sub);
      
    }
    possible_autocomplete = (autocomplete_array){
      .tag = file_or_dir,
      .array.values = filtered.values,
      .array.len = filtered.len,
      .appending_index = appending_index
     };
  }
  
  return possible_autocomplete;
}

char tabLoop(char* line, coordinates* cursor_pos, const string_array PATH_BINS, const coordinates terminal_size){
  char c = TAB;
  int tab_index = -1;
  char answer;
  string_array splitted_line = splitString(line, ' ');
  autocomplete_array possible_tabcomplete = checkForCommandAutoComplete(splitted_line, PATH_BINS);
  int format_width = getLongestWordInArray(possible_tabcomplete.array) + 2;
  int col_size = terminal_size.x / format_width;
  int row_size = ceil(possible_tabcomplete.array.len / (float)col_size);
  int cursor_height_diff = terminal_size.y - cursor_pos->y;

  if (possible_tabcomplete.array.len > 30){
    printf("\nThe list of possible matches is %d. Do you want to print all of them? (y/n) ", possible_tabcomplete.array.len);
    answer = getch();

    moveCursorIfShifted(cursor_pos, cursor_height_diff, 1);
    if (answer != 'y'){
      return '\n';
    }
  }
  do {
    CLEAR_BELOW_CURSOR;
    cursor_height_diff = terminal_size.y - cursor_pos->y;

    if (c == TAB){
      if (possible_tabcomplete.array.len == 1){
        if (possible_tabcomplete.tag == command){
          strcpy(line,possible_tabcomplete.array.values[0]);
        } else if (possible_tabcomplete.tag == file_or_dir){
          strcat(line, &(possible_tabcomplete.array.values[0])[possible_tabcomplete.appending_index[0]]);
        }
        return 0;
      } else {
        if (tab_index < possible_tabcomplete.array.len - 1){
          tab_index += 1;
        } else {
          tab_index = 0;
        }
        tabRender(possible_tabcomplete.array, tab_index, col_size, format_width);
        moveCursorIfShifted(cursor_pos, cursor_height_diff, row_size);
      }
    } else if (c == ESCAPE){
      getch();
      if (getch() == 'Z'){ // Shift-Tab
        if (tab_index > 0){
          tab_index--;
        } else {
          tab_index = possible_tabcomplete.array.len - 1;
        }
        tabRender(possible_tabcomplete.array, tab_index, col_size, format_width);
        moveCursorIfShifted(cursor_pos, cursor_height_diff, row_size);
      }
    } else if (c == '\n'){
      if (possible_tabcomplete.tag == command){
        strcpy(line,possible_tabcomplete.array.values[tab_index]);
      } else if (possible_tabcomplete.tag == file_or_dir){
        strcat(line, &(possible_tabcomplete.array.values[tab_index])[possible_tabcomplete.appending_index[tab_index]]);
      }
      return c;

    } else {
      return c;
    }
  } while ((c = getch()));

  return 0;
}
