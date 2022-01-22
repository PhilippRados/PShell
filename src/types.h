enum cursor_direction { cursor_up, cursor_down, cursor_left, cursor_right };

enum logger_type {
  integer,
  string,
  character,
};

enum autocomplete_type {
  command,
  file_or_dir,
};

enum color_decorations { standard = 0, bold = 1, underline = 4, reversed = 7 };

typedef struct coordinates {
  int x;
  int y;
} coordinates;

typedef struct {
  int len;
  char** values;
} string_array;

typedef struct {
  string_array array;
  int appending_index;
} autocomplete_array;

typedef struct {
  int one;
  int second;
} integer_tuple;

typedef struct {
  char* removed_sub;
  char* current_dir;
} file_string_tuple;

typedef struct {
  int format_width;
  int col_size;
  int row_size;
  int cursor_height_diff;
  coordinates* cursor_pos;
  coordinates terminal_size;
} render_objects;
