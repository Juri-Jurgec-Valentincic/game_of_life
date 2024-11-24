#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

char *field;

struct winsize window_size;
#define COLS window_size.ws_col
#define ROWS window_size.ws_row
typedef unsigned short us;

// █
unsigned char alive_char = 219;

char field_val(us row, us col) { return field[row * col + col]; }

char new_state(us row, us col) {
  char sum = 0;
  bool is_top, is_bottom, is_left, is_right;
  if (row == 0)
    is_top = true;
  else if (row == ROWS - 1)
    is_bottom = true;
  if (col == 0)
    is_left = true;
  else if (col == COLS - 1)
    is_right = false;

  if (!is_top) {
    if (!is_left)
      sum += field_val(row - 1, col - 1);
    if (!is_right)
      sum += field_val(row - 1, col + 1);
    sum += field_val(row - 1, col);
  }
  if (!is_left)
    sum += field_val(row, col - 1);
  sum += field_val(row, col);
  if (!is_right)
    sum += field_val(row, col + 1);
  if (!is_bottom) {
    if (!is_left)
      sum += field_val(row + 1, col - 1);
    if (!is_right)
      sum += field_val(row + 1, col + 1);
    sum += field_val(row + 1, col);
  }
  if (sum == 3)
    return 1;
  if (sum == 4)
    return field_val(row, col);
  return 0;
}

void update() {
  char buf1[COLS], buf2[COLS], *use = buf1;
  for (us row = 0; row < ROWS; row++) {
    for (us col = 0; col < COLS; col++) {
      use[row] = new_state(row, col);
    }
    if (row == 0)
      use = buf2;
    else if (row % 2 == 1) {
      use = buf1;
      memcpy(field + (row - 1) * COLS, buf1, sizeof(buf1));
    } else {
      use = buf2;
      memcpy(field + (row - 1) * COLS, buf2, sizeof(buf2));
    }
  }
  if (use == buf1)
    memcpy(field + (ROWS - 1) * COLS, buf2, sizeof(buf2));
  else
    memcpy(field + (ROWS - 1) * COLS, buf1, sizeof(buf1));
}

void draw() {
  system("clear");
  for (us c; c < COLS; c++) {
    for (us r; r < ROWS; r++) {
      if (field_val(r, c))
        putchar(alive_char);
    }
    putchar('\n');
  }
  usleep(500000);
}

void readfile(char *file_name) {
  FILE *file = fopen(file_name, "r");
  if (!file) {
    fprintf(stderr, "ERROR could not open file: %s", file_name);
    exit(EXIT_FAILURE);
  }
  char c = fgetc(file);
  size_t until_eol;
  size_t i = 0;
  while (c != EOF && i < ROWS * COLS - 1) {
    switch (c) {
    case '0':
      field[i] = 0;
      break;
    case '1':
      field[i] = 1;
      break;
    case '\n':
      until_eol = COLS - (ROWS * COLS % (i + 1));
      memset(field + i, 0, until_eol);
      break;
    }
    i++;
  }
  memset(field + i, 0, ROWS * COLS - i);
  fclose(file);
}

int main(int argc, char *argv[]) {
  ioctl(0, TIOCGWINSZ, &window_size);
  field = alloca(sizeof(bool) * COLS * ROWS);
  memset(field, 0, COLS * ROWS);
  while (1) {
    draw();
    update();
  }
  return EXIT_SUCCESS;
}
