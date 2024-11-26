#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef unsigned short us;
char *field;

struct winsize window_size;
#define COLS window_size.ws_col
#define ROWS window_size.ws_row
char *clear_str;

char alive[] = "\u2588";
// sleep time between frames in micro seconds
#define SLEEP_TIME 250000

char field_val(us row, us col) { return field[row * COLS + col]; }

char new_state(us row, us col) {
  char sum = 0;
  bool is_top = false, is_bottom = false, is_left = false, is_right = false;
  if (row == 0)
    is_top = true;
  else if (row == ROWS - 1)
    is_bottom = true;
  if (col == 0)
    is_left = true;
  else if (col == COLS - 1)
    is_right = true;

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
      use[col] = new_state(row, col);
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
  fputs(clear_str, stdout);
  for (us r = 0; r < ROWS; r++) {
    for (us c = 0; c < COLS; c++) {
      if (field_val(r, c))
        fputs(alive, stdout);
      else
        putchar(' ');
    }
    putchar('\n');
  }
}

void readfile(char *file_name) {
  FILE *file = fopen(file_name, "r");
  if (!file) {
    fprintf(stderr, "ERROR opening file \"%s\": %s\n", file_name,
            strerror(errno));
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
      until_eol = COLS - (i + 1) % COLS;
      i += until_eol;
      break;
    }
    i++;
    c = fgetc(file);
  }
  fclose(file);
}

void get_clear() {
  FILE *fp = popen("clear", "r");
  assert(fp);
  char tmp[255];
  fgets(tmp, sizeof(tmp), fp);
  clear_str = malloc(strlen(tmp));
  strcpy(clear_str, tmp);
  pclose(fp);
}

int main(int argc, char *argv[]) {
  if (argc == 1 || argc == 2 && !strcmp(argv[1], "-h") ||
      argc == 2 && !strcmp(argv[1], "--help")) {
    printf("Usage: %s initial_state_file\n", argv[0]);
    return EXIT_FAILURE;
  }
  get_clear();
  ioctl(0, TIOCGWINSZ, &window_size);
  field = alloca(sizeof(bool) * COLS * ROWS);
  memset(field, 0, COLS * ROWS);
  readfile(argv[1]);
  while (1) {
    draw();
    update();
    usleep(SLEEP_TIME);
  }
  free(clear_str);
  return EXIT_SUCCESS;
}
