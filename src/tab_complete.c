#include "main.h"

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

char tabLoop(char* line, coordinates* cursor_pos, const string_array PATH_BINS, const coordinates terminal_size){
  char c = TAB;
  int tab_index = -1;
  char answer;
  string_array possible_tabcomplete = checkForCommandAutoComplete(splitString(line,' '),PATH_BINS);
  int format_width = getLongestWordInArray(possible_tabcomplete) + 2;
  int col_size = terminal_size.x / format_width;
  int row_size = ceil(possible_tabcomplete.len / (float)col_size);
  int cursor_height_diff = terminal_size.y - cursor_pos->y;

  if (possible_tabcomplete.len > 30){
    printf("\nThe list of possible matches is %d. Do you want to print all of them? (y/n) ", possible_tabcomplete.len);
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
      if (possible_tabcomplete.len == 1){
        strcpy(line,possible_tabcomplete.values[0]);
      } else {
        if (tab_index < possible_tabcomplete.len - 1){
          tab_index += 1;
        } else {
          tab_index = 0;
        }
        tabRender(possible_tabcomplete, tab_index, col_size, format_width);
        moveCursorIfShifted(cursor_pos, cursor_height_diff, row_size);
      }
    } else if (c == ESCAPE){
      getch();
      if (getch() == 'Z'){ // Shift-Tab
        if (tab_index > 0){
          tab_index--;
        } else {
          tab_index = possible_tabcomplete.len - 1;
        }
        tabRender(possible_tabcomplete, tab_index, col_size, format_width);
        moveCursorIfShifted(cursor_pos, cursor_height_diff, row_size);
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
