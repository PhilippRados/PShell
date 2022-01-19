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

      int diff_len = strlen(possible_tabcomplete.values[j]) - format_width;
      if (tab_index == j){

        printColor(possible_tabcomplete.values[j],GREEN, reversed);
        printf("%-*s",diff_len,"");
      } else { 
        if (possible_tabcomplete.values[j][strlen(possible_tabcomplete.values[j]) - 1] == '/'){
          printColor(possible_tabcomplete.values[j], CYAN, bold);
          printf("%-*s",diff_len,"");
        } else {
          printf("%-*s",format_width,possible_tabcomplete.values[j]);
        }
      }
      j++;
    }
  }
}

void fileDirArray(string_array* filtered, char* current_dir_sub, char* removed_sub){
    char* current_dir_sub_copy = calloc(strlen(current_dir_sub) + 256, sizeof(char));
    char* temp;
    char copy[512];
    strcat(current_dir_sub,"/");

    for (int i = 0; i < filtered->len; i++){
      strcpy(current_dir_sub_copy, current_dir_sub);

      temp = strcpy(copy, strcat(current_dir_sub_copy,filtered->values[i]));
      
      if (isDirectory(temp)){
        filtered->values[i] = realloc(filtered->values[i], strlen(filtered->values[i]) + 2);
        filtered->values[i][strlen(filtered->values[i]) + 1] = '\0';
        filtered->values[i][strlen(filtered->values[i])] = '/';
      }
      memset(copy, 0,strlen(copy));
      memset(temp, 0,strlen(temp));
      memset(current_dir_sub_copy, 0, strlen(current_dir_sub_copy));
      
    }
    free(current_dir_sub_copy);
    free(current_dir_sub);
}

int getCurrentWordPosInLine(string_array command_line, char* word){
  for (int i = 0; i < command_line.len; i++){
    if (strncmp(command_line.values[i], word, strlen(word)) == 0){
      return i;
    }
  }
  
  return -1;
}

file_string_tuple getFileStrings(char* current_word, char* current_path){
  char* current_dir;
  char* removed_sub;

  switch (current_word[0]){
    case '/':{
      current_dir = current_word;
      if (strlen(current_dir) == 1){
        removed_sub = "";
      } else {
        removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir,'/')]); // c_e
      }
      break;
    }
    case '~':{
      char* home_path = getenv("HOME"); // Users/username
      char* home_path_copy = calloc(strlen(home_path) + strlen(current_word) + 2, sizeof(char));
      strcpy(home_path_copy, home_path);

      char* current_path = strcat(home_path_copy, "/"); // Users/username/
      current_dir = strcat(current_path, &(current_word[1])); //Users/username/documents
      removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir,'/')]); // documents
      break;
    }
    default: {
      current_dir = strcat(current_path, current_word); // documents/coding/c_e
      removed_sub = &(current_dir[strlen(current_dir) - getAppendingIndex(current_dir,'/')]); // c_e
      break;
    }
  }

  return (file_string_tuple){
    .removed_sub = removed_sub,
    .current_dir = current_dir
  };
}

autocomplete_array checkForCommandAutoComplete(char* current_word, char* first_word, int line_index,const string_array PATH_BINS){
  autocomplete_array possible_autocomplete = {
    .array.len = 0
  };

  if (strlen(first_word) >= line_index){ // autocomplete for commands
    string_array filtered = filterMatching(current_word,PATH_BINS);

    possible_autocomplete = (autocomplete_array){
      .tag = command,
      .array.values = filtered.values,
      .array.len = filtered.len,
      .appending_index = strlen(current_word)
    };
  } else { // autocomplete for files
    char cd[256];
    file_string_tuple file_strings = getFileStrings(current_word, strcat(getcwd(cd, sizeof(cd)), "/"));

    char* current_dir_sub = calloc(strlen(file_strings.current_dir) + 2, sizeof(char));
    strncpy(current_dir_sub, file_strings.current_dir, strlen(file_strings.current_dir) - getAppendingIndex(file_strings.current_dir, '/')); // documents/coding
    string_array filtered = getAllMatchingFiles(current_dir_sub, file_strings.removed_sub); // c_excercises, c_experiments

    fileDirArray(&filtered, current_dir_sub, file_strings.removed_sub); // directories get / appended

    possible_autocomplete = (autocomplete_array){
      .tag = file_or_dir,
      .array.values = filtered.values,
      .array.len = filtered.len,
      .appending_index = strlen(file_strings.removed_sub)
     };
  }
  
  return possible_autocomplete;
}

bool tooManyMatches(coordinates* cursor_pos,int row_size, int cursor_height_diff){
  char answer;
  if (row_size > 10){
    printf("\nThe list of possible matches is %d lines. Do you want to print all of them? (y/n) ", row_size);
    answer = getch();

    moveCursorIfShifted(cursor_pos, cursor_height_diff, 1);
    if (answer != 'y'){
      return true;
    }
  }
  return false;
}

bool tabPress(autocomplete_array possible_tabcomplete, int* tab_index, char* line, int line_index){
  if (possible_tabcomplete.array.len == 1){
    removeSlice(&line, line_index);
    insertStringAtPos(line, &(possible_tabcomplete.array.values[0])[possible_tabcomplete.appending_index],line_index);

    return true;
  } else if (possible_tabcomplete.array.len > 1){
    if (*tab_index < possible_tabcomplete.array.len - 1){
      *tab_index += 1;
    } else {
      *tab_index = 0;
    }
  }
  return false;
}

void shiftTabPress(autocomplete_array possible_tabcomplete, int* tab_index){
  getch();
  if (getch() == 'Z'){ // Shift-Tab
    if (*tab_index > 0){
      *tab_index -= 1;
    } else {
      *tab_index = possible_tabcomplete.array.len - 1;
    }
  }
}

void enterPress(autocomplete_array possible_tabcomplete,char* line, int line_index, int tab_index){
  removeSlice(&line, line_index);
  insertStringAtPos(line, &(possible_tabcomplete.array.values[tab_index])[possible_tabcomplete.appending_index],line_index);
}

bool updateCompletion(autocomplete_array possible_tabcomplete, char* c, char* line, 
                      int line_index, int* tab_index){
  bool loop = true;
  if (*c == TAB){
    if (tabPress(possible_tabcomplete, tab_index, line, line_index)){
      *c = '\n';
      loop =  false;
    }

  } else if (*c == ESCAPE){
    shiftTabPress(possible_tabcomplete, tab_index);

  } else if (*c == '\n'){
    enterPress(possible_tabcomplete, line, line_index, *tab_index);
    loop = false;

  } else {
    loop = false;
  }

  return loop;
}

void renderCompletion(autocomplete_array possible_tabcomplete, char c, int tab_index, render_objects* render_data){
  logger(integer, &tab_index);
  render_data->cursor_height_diff = render_data->terminal_size.y - render_data->cursor_pos->y;

  moveCursor((coordinates){1000, render_data->cursor_pos->y}); // have to move cursor to end of line to not cut off in middle
  CLEAR_BELOW_CURSOR;
  tabRender(possible_tabcomplete.array, tab_index, render_data->col_size, render_data->format_width);
  moveCursorIfShifted(render_data->cursor_pos, render_data->cursor_height_diff, render_data->row_size);
}

render_objects initializeRenderObjects(coordinates terminal_size, autocomplete_array possible_tabcomplete,
                                      coordinates* cursor_pos){
  int format_width = getLongestWordInArray(possible_tabcomplete.array) + 2;
  int col_size = terminal_size.x / format_width;
  int row_size = ceil(possible_tabcomplete.array.len / (float)col_size);
  int cursor_height_diff = terminal_size.y - cursor_pos->y;

  return (render_objects){
    .format_width = format_width,
    .col_size = col_size,
    .row_size = row_size,
    .cursor_height_diff = cursor_height_diff,
    .cursor_pos = cursor_pos,
    .terminal_size = terminal_size,
  };
}

char tabLoop(char* line, coordinates* cursor_pos, const string_array PATH_BINS, const coordinates terminal_size, int line_index){
  char* c = calloc(1, sizeof(char));
  *c = TAB;
  int tab_index = -1;
  string_array splitted_line = splitString(line, ' ');
  char* current_word = getCurrentWordFromLineIndex(splitted_line, line_index);
  autocomplete_array possible_tabcomplete = checkForCommandAutoComplete(current_word,splitted_line.values[0],line_index, PATH_BINS);
  free_string_array(&splitted_line);
  free(current_word);
  render_objects render_data = initializeRenderObjects(terminal_size, possible_tabcomplete, cursor_pos);
  bool loop = true;

  if (tooManyMatches(cursor_pos, render_data.row_size, render_data.cursor_height_diff)){
    free_string_array(&(possible_tabcomplete.array));
    return '\n';
  }
  do {
    if ((loop = updateCompletion(possible_tabcomplete, c, line, line_index, &tab_index))){
      renderCompletion(possible_tabcomplete, *c, tab_index, &render_data);
    }

  } while (loop && (*c = getch()));
  free_string_array(&(possible_tabcomplete.array));

  return *c;
}
