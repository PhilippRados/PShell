#include "main.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(const char *s1, char *s2, int s1_len) {
  char* s2_substring = calloc(strlen(s1) + 1, sizeof(char));
  s2_substring = strncpy(s2_substring,&s2[0], strlen(s1));

  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = s1len;
  unsigned int column[s1len + 1];

  for (y = 1; y <= s1len; y++){
    column[y] = y;
  }

  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y-1] == s2_substring[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }

  return column[s1len];
}

string_array filterHistory(const string_array concatenated, char* line){
  char** possible_matches = calloc(512, sizeof(char*));
  int matches_num = 0;
  char* line_no_whitespace = removeWhitespace(line);

  if (strlen(line) > 0){
    for (int i = 0; i < concatenated.len; i++){
      char* values_no_whitespace = removeWhitespace(concatenated.values[i]);

      if (levenshtein(line_no_whitespace, values_no_whitespace,strlen(line_no_whitespace)) < 2){
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
  int index = 0;
  int i = 0;
  char* line = calloc(64, sizeof(char));

  coordinates cursor = {.x = 2, .y = terminal_size.y * 0.9};
  moveCursor(cursor);
  CLEAR_BELOW_CURSOR;
  for (int i = 0; i < terminal_size.x - 1; i++){
    printf("\u2501");
  }

  coordinates initial_cursor_pos = {
    .x = cursor.x,
    .y = cursor.y + 2,
  };
  moveCursor(initial_cursor_pos);
  printf("\u2771 ");

  string_array matching_commands;
  matching_commands.len = 0;

  while ((c = getch())){
    CLEAR_LINE;
    CLEAR_BELOW_CURSOR;

    if (c == '\n'){
      if (matching_commands.len > 0){
        memset(line,0,strlen(line));
        strcpy(line, matching_commands.values[index]);
      }
      break;
    } else if (c == BACKSPACE){
      backspaceLogic(&line, &i);
    } else if (c == ESCAPE){
      if (getch() == ESCAPE){
        break;
      }
      int value = getch();

      if (value == 'A'){
        (index > 0) ? index-- : index;
      } else if (value == 'B'){
        (index < matching_commands.len - 1) ? index++ : index;
      }
    } else {
      if (strlen(line) < 63 && c > 0 && c < 127){
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

  CLEAR_SCREEN;
  printf("\n");

  return line;
}
