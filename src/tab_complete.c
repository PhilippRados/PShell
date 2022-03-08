#include "tab_complete.h"

int getLongestWordInArray(const string_array array) {
  int longest = 0;
  int current_len = 0;

  for (int i = 0; i < array.len; i++) {
    current_len = strlen(array.values[i]);
    if (current_len > longest) {
      longest = current_len;
    }
  }

  return longest;
}

void tabRender(string_array possible_tabcomplete, int tab_index, int col_size, int format_width) {
  int j = 0;

  while (j < possible_tabcomplete.len) {
    printf("\n");
    for (int x = 0; x < col_size; x++) {
      if (j >= possible_tabcomplete.len) {
        break;
      }

      int diff_len = strlen(possible_tabcomplete.values[j]) - format_width;
      if (tab_index == j) {

        printColor(possible_tabcomplete.values[j], GREEN, reversed);
        printf("%-*s", diff_len, "");
      } else {
        if (possible_tabcomplete.values[j][strlen(possible_tabcomplete.values[j]) - 1] == '/') {
          printColor(possible_tabcomplete.values[j], CYAN, bold);
          printf("%-*s", diff_len, "");
        } else {
          printf("%-*s", format_width, possible_tabcomplete.values[j]);
        }
      }
      j++;
    }
  }
}

char* getCurrentWordFromLineIndex(string_array command_line, int line_index) {
  int current_pos = 0;
  char* result;
  for (int i = 0; i < command_line.len; i++) {
    if (line_index >= current_pos && line_index <= (current_pos + strlen(command_line.values[i]))) {
      int index_in_word = line_index - current_pos;
      result = calloc(strlen(command_line.values[i]) + 1, sizeof(char));
      strncpy(result, command_line.values[i], index_in_word);
      break;
    }
    current_pos += strlen(command_line.values[i]) + 1;
  }

  return result;
}

// 0-indexed
void removeSlice(char** line, int start) {
  int end = getWordEndIndex(*line, start);
  for (int i = start; i < end; i++) {
    *line = removeCharAtPos(*line, start + 1);
  }
}

autocomplete_array checkForAutocomplete(char* current_word, char* first_word, int line_index,
                                        const string_array PATH_BINS) {
  autocomplete_array possible_autocomplete = {.array.len = 0};

  if (strlen(first_word) >= line_index) { // autocomplete for commands
    string_array filtered = filterMatching(current_word, PATH_BINS);

    possible_autocomplete = (autocomplete_array){
        .array.values = filtered.values, .array.len = filtered.len, .appending_index = strlen(current_word)};
  } else { // autocomplete for files
    possible_autocomplete = fileComp(current_word);
  }

  return possible_autocomplete;
}

void moveCursorIfShifted(render_objects* render_data) {
  if (render_data->cursor_height_diff <= render_data->row_size || render_data->cursor_height_diff == 0) {
    render_data->cursor_pos->y -= render_data->row_size - render_data->cursor_height_diff;
    moveCursor(*render_data->cursor_pos);
  } else {
    moveCursor(*render_data->cursor_pos);
  }
  render_data->cursor_pos->y -= render_data->cursor_row;
}

void renderCompletion(autocomplete_array possible_tabcomplete, int tab_index, render_objects* render_data) {
  render_data->cursor_pos->y += render_data->cursor_row;
  int bottom_line_y = render_data->cursor_pos->y - render_data->cursor_row + render_data->line_row_count;
  render_data->cursor_height_diff = render_data->terminal_size.y - bottom_line_y;

  moveCursor((coordinates){1000, bottom_line_y}); // have to move cursor to end of
                                                  // line to not cut off in middle
  CLEAR_BELOW_CURSOR;
  tabRender(possible_tabcomplete.array, tab_index, render_data->col_size, render_data->format_width);
  moveCursorIfShifted(render_data);
}

bool dontShowMatches(char answer, render_objects* render_data, autocomplete_array possible_tabcomplete) {
  bool result = false;

  if (answer != 'y') {
    result = true;
  } else if (render_data->row_size >= render_data->terminal_size.y) {
    renderCompletion(possible_tabcomplete, -1, render_data);
    printf("\n\n");

    for (int i = 0; i < render_data->line_row_count; i++) {
      printf("\n");
    }
    render_data->cursor_pos->y =
        render_data->terminal_size.y + render_data->cursor_row - render_data->line_row_count;
    result = true;
  }
  return result;
}

bool tooManyMatches(render_objects* render_data, autocomplete_array possible_tabcomplete) {
  char answer;
  char* prompt_sentence = "\nThe list of possible matches is %d lines. Do you want to print all of them? (y/n) ";
  int bottom_line_y = render_data->cursor_pos->y + render_data->line_row_count;

  moveCursor((coordinates){1000, bottom_line_y});

  if (!(render_data->row_size > 10 || render_data->row_size > render_data->terminal_size.y))
    return false;

  printf(prompt_sentence, render_data->row_size);
  answer = getch();

  int prompt_row_count = calculateRowCount(render_data->terminal_size, 0, strlen(prompt_sentence) - 1) + 1;
  int diff = prompt_row_count + bottom_line_y - render_data->terminal_size.y;
  if (diff > 0) {
    render_data->cursor_pos->y -= diff;
  }

  return dontShowMatches(answer, render_data, possible_tabcomplete);
}

bool tabPress(autocomplete_array possible_tabcomplete, int* tab_index, char* line, int line_index) {
  if (possible_tabcomplete.array.len == 1) {
    removeSlice(&line, line_index);
    insertStringAtPos(line, &(possible_tabcomplete.array.values[0])[possible_tabcomplete.appending_index],
                      line_index);

    return true;
  } else if (possible_tabcomplete.array.len > 1) {
    if (*tab_index < possible_tabcomplete.array.len - 1) {
      *tab_index += 1;
    } else {
      *tab_index = 0;
    }
  }
  return false;
}

void shiftTabPress(autocomplete_array possible_tabcomplete, int* tab_index) {
  getch();
  if (getch() == 'Z') { // Shift-Tab
    if (*tab_index > 0) {
      *tab_index -= 1;
    } else {
      *tab_index = possible_tabcomplete.array.len - 1;
    }
  }
}

void enterPress(autocomplete_array possible_tabcomplete, char* line, int line_index, int tab_index) {
  removeSlice(&line, line_index);
  insertStringAtPos(line, &(possible_tabcomplete.array.values[tab_index])[possible_tabcomplete.appending_index],
                    line_index);
}

bool updateCompletion(autocomplete_array possible_tabcomplete, char* c, char* line, int line_index,
                      int* tab_index) {
  bool loop = true;
  if (*c == TAB) {
    if (tabPress(possible_tabcomplete, tab_index, line, line_index)) {
      *c = 0;
      loop = false;
    }

  } else if (*c == ESCAPE) {
    shiftTabPress(possible_tabcomplete, tab_index);

  } else if (*c == '\n') {
    enterPress(possible_tabcomplete, line, line_index, *tab_index);
    *c = 0;
    loop = false;

  } else {
    loop = false;
  }

  return loop;
}

render_objects initializeRenderObjects(coordinates terminal_size, autocomplete_array possible_tabcomplete,
                                       coordinates* cursor_pos, int cursor_row, int line_row_count) {
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
      .cursor_row = cursor_row,
      .line_row_count = line_row_count,
  };
}

autocomplete_array removeDotFiles(autocomplete_array* tabcomp) {
  autocomplete_array new;
  new.array.values = calloc(tabcomp->array.len, sizeof(char*));
  int j = 0;
  for (int i = 0; i < tabcomp->array.len; i++) {
    if (tabcomp->array.values[i][0] != '.') {
      new.array.values[j] = calloc(strlen(tabcomp->array.values[i]) + 1, sizeof(char));
      strcpy(new.array.values[j], tabcomp->array.values[i]);
      j++;
    }
  }
  new.appending_index = tabcomp->appending_index;
  new.array.len = j;
  free_string_array(&tabcomp->array);
  return new;
}

void removeDotFilesIfnecessary(char* current_word, autocomplete_array* possible_tabcomplete) {
  int appending_index;
  char* removed_sub;
  if ((appending_index = getAppendingIndex(current_word, '/')) != -1) {
    removed_sub = &(current_word[strlen(current_word) - getAppendingIndex(current_word, '/')]);
  } else {
    removed_sub = current_word;
  }
  if (removed_sub[0] != '.') {
    *possible_tabcomplete = removeDotFiles(possible_tabcomplete);
  }
}

void shortenCompletesLongerThanTerm(autocomplete_array* possible_tabcomplete, int terminal_width) {
  for (int i = 0; i < possible_tabcomplete->array.len; i++) {
    if (strlen(possible_tabcomplete->array.values[i]) > (terminal_width - 2)) {
      possible_tabcomplete->array.values[i][terminal_width - 2] = '\0';
      for (int j = terminal_width - 3; j > (terminal_width - 6); j--) {
        possible_tabcomplete->array.values[i][j] = '.';
      }
    }
  }
}

char tabLoop(line_data line_info, coordinates* cursor_pos, const string_array PATH_BINS,
             const coordinates terminal_size) {
  char* c = calloc(1, sizeof(char));
  *c = TAB;
  int tab_index = -1;
  string_array splitted_line = splitString(line_info.line, ' ');
  char* current_word = getCurrentWordFromLineIndex(splitted_line, *line_info.i);
  autocomplete_array possible_tabcomplete =
      checkForAutocomplete(current_word, splitted_line.values[0], *line_info.i, PATH_BINS);
  removeDotFilesIfnecessary(current_word, &possible_tabcomplete);
  shortenCompletesLongerThanTerm(&possible_tabcomplete, terminal_size.x);
  render_objects render_data = initializeRenderObjects(terminal_size, possible_tabcomplete, cursor_pos,
                                                       line_info.cursor_row, line_info.line_row_count);
  bool loop = true;

  if (possible_tabcomplete.array.len <= 0 || tooManyMatches(&render_data, possible_tabcomplete)) {
    free_string_array(&(possible_tabcomplete.array));
    free_string_array(&splitted_line);
    free(current_word);
    return -1;
  }
  do {
    if ((loop = updateCompletion(possible_tabcomplete, c, line_info.line, *line_info.i, &tab_index))) {
      renderCompletion(possible_tabcomplete, tab_index, &render_data);
    }

  } while (loop && (*c = getch()));

  free_string_array(&(possible_tabcomplete.array));
  free_string_array(&splitted_line);
  free(current_word);

  return *c;
}
