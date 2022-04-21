typedef struct {
  int attr;
  int fg;
  int bg;
} color;

static const color WHITE = {
    .attr = 0,
    .fg = 37,
    .bg = 10,
};

static const color RED = {
    .attr = 0,
    .fg = 31,
    .bg = 10,
};

static const color GREEN = {
    .attr = 0,
    .fg = 32,
    .bg = 10,
};

static const color CYAN = {
    .attr = 1,
    .fg = 36,
    .bg = 10,
};

static const color HIGHLIGHT = {
    .attr = 1,
    .fg = 37,
    .bg = 42,
};

static const color YELLOW = {
    .attr = 0,
    .fg = 33,
    .bg = 10,
};

static const color BLUE = {
    .attr = 0,
    .fg = 34,
    .bg = 10,
};
