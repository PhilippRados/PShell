#include "main.h"

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
  int width = terminal_size.x * 0.4;
  int height = terminal_size.y * 0.3;
  int index = 0;
  int i = 0;
  char* line = calloc(64, sizeof(char));

  drawPopupBox(terminal_size, width, height);

  coordinates cursor = {.x = (width / 2) + 3, .y = (height / 2)};
  moveCursor(cursor);
  printf("Fuzzy Find through past commands");

  coordinates initial_cursor_pos = {
    .x = cursor.x,
    .y = cursor.y + 2,
  };
  moveCursor(initial_cursor_pos);
  printf("\u2771 ");

  string_array matching_commands;
  matching_commands.len = 0;

  while ((c = getch())){
    clearFuzzyWindow(initial_cursor_pos, terminal_size.x - width, terminal_size.y - height);
    if (c == '\n'){
      if (matching_commands.len > 0){
        memset(line,0,strlen(line));
        strcpy(line, matching_commands.values[index]);
      }
      goto FINISH_LOOP;
    } else if (c == BACKSPACE){
      backspaceLogic(&line, &i);
    } else if (c == ESCAPE){
      if (getch() == ESCAPE){
        goto FINISH_LOOP;
      }
      int value = getch();

      if (value == 'A'){
        (index > 0) ? index-- : index;
      } else if (value == 'B'){
        (index < matching_commands.len - 1) ? index++ : index;
      }
    } else {
      if (strlen(line) < 63){
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
  FINISH_LOOP:

  CLEAR_SCREEN;
  printf("\n");

  return line;
}
