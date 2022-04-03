#include "fuzzy_finder.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(const char* s1, char* s2, int s1_len) {
  if (s1_len == 0)
    return 0;
  char* s2_substring = calloc(strlen(s1) + 1, sizeof(char));
  s2_substring = strncpy(s2_substring, &s2[0], strlen(s1));

  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = s1len;
  unsigned int column[s1len + 1];

  for (y = 1; y <= s1len; y++) {
    column[y] = y;
  }

  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2_substring[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  free(s2_substring);

  return column[s1len];
}

char* removeWhitespace(char* s1) {
  char* stripped = calloc(strlen(s1) + 1, sizeof(char));
  int j = 0;

  for (int i = 0; i < strlen(s1); i++) {
    if (s1[i] != ' ') {
      stripped[j] = s1[i];
      j++;
    }
  }

  return stripped;
}

string_array filterHistory(const string_array concatenated, char* line) {
  char** possible_matches = calloc(concatenated.len, sizeof(char*));
  int matches_num = 0;
  char* line_no_whitespace = removeWhitespace(line);

  for (int i = 0; i < concatenated.len; i++) {
    char* values_no_whitespace = removeWhitespace(concatenated.values[i]);

    if (levenshtein(line_no_whitespace, values_no_whitespace, strlen(line_no_whitespace)) < 2) {
      possible_matches[matches_num] = calloc(strlen(concatenated.values[i]) + 1, sizeof(char));
      strcpy(possible_matches[matches_num], concatenated.values[i]);
      matches_num++;
    }
    free(values_no_whitespace);
  }
  free(line_no_whitespace);
  string_array result = {.values = possible_matches, .len = matches_num};

  return result;
}

integer_tuple findDisplayIndices(int matching_commands_len, int cursor_diff, int index) {
  int start = 0;
  int end = (matching_commands_len < cursor_diff) ? matching_commands_len : cursor_diff;

  if (index >= cursor_diff) {
    start = index - cursor_diff + 1;
    end = index + 1;
  }

  integer_tuple result = {
      .one = start,
      .second = end,
  };

  return result;
}

int shiftPromptIfOverlapTest(int current_cursor_height, int fuzzy_popup_height) {
  if (current_cursor_height < fuzzy_popup_height)
    return -1;
  int j = 0;

  for (int i = fuzzy_popup_height; i <= current_cursor_height; i++) {
    j++;
  }
  return j;
}

void drawPopupBox(const coordinates terminal_size, const int width, const int height) {
  CLEAR_SCREEN

  for (int row = 0; row < terminal_size.y; row++) {
    if (row == (height / 2) || row == (terminal_size.y - (height / 2))) {
      for (int i = 0; i < terminal_size.x; i++) {
        if (i > (width / 2) && i < (terminal_size.x - (width / 2))) {
          printf("\u2550");
        } else if (i == (width / 2)) {
          if (row == (height / 2)) {
            printf("\u2554");
          } else {
            printf("\u255A");
          }
        } else if (i == (terminal_size.x - (width / 2))) {
          if (row == (height / 2)) {
            printf("\u2557");
          } else {
            printf("\u255D");
          }
        } else {
          printf(" ");
        }
      }
    } else if (row > (height / 2) && row < (terminal_size.y - (height / 2))) {
      for (int col = 0; col < terminal_size.x; col++) {
        if (col == (width / 2) || col == (terminal_size.x - (width / 2))) {
          printf("\u2551");
        } else {
          printf(" ");
        }
      }
    } else {
      printf("\n");
    }
  }
  coordinates bottom_box_pos = {.x = (width / 2) + 3, .y = terminal_size.y - (height / 2)};
  moveCursor(bottom_box_pos);
}

void clearFuzzyWindow(coordinates initial_cursor_pos, int box_width, int box_height) {
  for (int rows = initial_cursor_pos.x + 2; rows < box_width; rows++) {
    for (int cols = initial_cursor_pos.y; cols < box_height; cols++) {
      coordinates cursor = {.x = rows, .y = cols};
      moveCursor(cursor);
      printf(" ");
    }
  }

  moveCursor(initial_cursor_pos);
}

void renderMatches(string_array matching_commands, coordinates initial_cursor_pos, int index,
                   integer_tuple start_end, int terminal_width) {
  int i = 0;
  char* complete;

  for (int j = start_end.one; j < start_end.second; j++) {
    coordinates drawing_pos = {
        .y = initial_cursor_pos.y + i + 1,
        .x = initial_cursor_pos.x + 2,
    };
    moveCursor(drawing_pos);

    complete = shortenIfTooLong(matching_commands.values[j], terminal_width - 1);
    if (j == index) {
      printColor(complete, GREEN, reversed);
    } else {
      printf("%s", complete);
    }
    free(complete);
    i++;
  }
}

void renderFuzzyFinder(coordinates initial_cursor_pos, int terminal_width, char* line, int index,
                       string_array matching_commands, int cursor_terminal_height_diff) {
  CLEAR_LINE;
  CLEAR_BELOW_CURSOR;
  coordinates end_of_line = {
      .x = initial_cursor_pos.x + (terminal_width - 10),
      .y = initial_cursor_pos.y,
  };

  integer_tuple display_ranges = findDisplayIndices(matching_commands.len, cursor_terminal_height_diff, index);
  renderMatches(matching_commands, initial_cursor_pos, index, display_ranges, terminal_width);

  moveCursor(end_of_line);
  printf("%d/%d", index + 1, matching_commands.len);

  moveCursor(initial_cursor_pos);
  printf("\u2771 %s", line);
}

bool shiftPromptIfOverlap(int current_cursor_height, int fuzzy_popup_height,
                          int line_row_count_with_autocomplete) {
  if ((current_cursor_height + line_row_count_with_autocomplete) < fuzzy_popup_height)
    return false;

  for (int i = fuzzy_popup_height; i <= (current_cursor_height + line_row_count_with_autocomplete); i++) {
    printf("\n");
  }
  return true;
}

bool drawFuzzyPopup(int current_cursor_height, coordinates initial_cursor_pos, int terminal_width,
                    int line_row_count_with_autocomplete) {
  bool shifted =
      shiftPromptIfOverlap(current_cursor_height, initial_cursor_pos.y - 2, line_row_count_with_autocomplete);
  moveCursor((coordinates){initial_cursor_pos.x, initial_cursor_pos.y - 2});

  for (int i = 0; i < terminal_width - 1; i++) {
    printf("\u2501");
  }

  moveCursor(initial_cursor_pos);
  printf("\u2771 ");

  return shifted;
}

bool updateFuzzyfinder(char** line, char c, string_array matching_commands, int* fuzzy_index, int* i,
                       int max_input_len) {
  bool loop = true;
  if (c == '\n') {
    if (matching_commands.len > 0) {
      memset(*line, 0, strlen(*line));
      if (strlen(matching_commands.values[*fuzzy_index]) > BUFFER) {
        *line = realloc(*line, (strlen(matching_commands.values[*fuzzy_index]) + 1) * sizeof(char));
      }
      strcpy(*line, matching_commands.values[*fuzzy_index]);
    }
    loop = false;
  } else if (c == BACKSPACE) {
    backspaceLogic(*line, i);
    *fuzzy_index = 0;
  } else if (c == ESCAPE) {
    if (getch() == ESCAPE) {
      strcpy(*line, "");
      return false;
    }
    int value = getch();

    if (value == 'A') {
      (*fuzzy_index > 0) ? (*fuzzy_index)-- : *fuzzy_index;
    } else if (value == 'B') {
      (*fuzzy_index < matching_commands.len - 1) ? (*fuzzy_index)++ : *fuzzy_index;
    }
  } else {
    if (strlen(*line) < max_input_len - 1 && c > 0 && c < 127) {
      *fuzzy_index = 0;
      (*line)[*i] = c;
      (*i)++;
    }
  }
  return loop;
}

fuzzy_result popupFuzzyFinder(const string_array all_time_command_history, const coordinates terminal_size,
                              int current_cursor_height, int line_row_count_with_autocomplete) {
  char c = -1;
  int* fuzzy_index = calloc(1, sizeof(int));
  *fuzzy_index = 0;
  int* i = calloc(1, sizeof(int));
  *i = 0;
  int max_input_len = terminal_size.x - 15;
  char* line = calloc(max_input_len, sizeof(char));
  coordinates fuzzy_cursor_height = {.x = 2, .y = terminal_size.y * 0.85};
  int cursor_terminal_height_diff = terminal_size.y - fuzzy_cursor_height.y;
  string_array matching_commands;
  matching_commands.len = 0;
  matching_commands.values = NULL;
  bool loop = true;

  moveCursor(terminal_size); // moving Cursor to bottom so that newline
                             // character shifts line up
  bool shifted = drawFuzzyPopup(current_cursor_height, fuzzy_cursor_height, terminal_size.x,
                                line_row_count_with_autocomplete);

  do {
    loop = updateFuzzyfinder(&line, c, matching_commands, fuzzy_index, i, max_input_len);

    if (loop) {
      string_array filtered_history = filterHistory(all_time_command_history, line);
      free_string_array(&matching_commands);
      matching_commands = removeDuplicates(&filtered_history);

      renderFuzzyFinder(fuzzy_cursor_height, terminal_size.x, line, *fuzzy_index, matching_commands,
                        cursor_terminal_height_diff);
    }
  } while (loop && (c = getch()));

  free_string_array(&matching_commands);
  free(fuzzy_index);
  free(i);

  moveCursor((coordinates){0, fuzzy_cursor_height.y - 2});
  CLEAR_LINE;
  CLEAR_BELOW_CURSOR;

  return (fuzzy_result){.line = line, .shifted = shifted};
}
