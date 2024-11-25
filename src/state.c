#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  snake_t* snake = malloc(sizeof(snake_t));
  game_state_t* game_state = malloc(sizeof(game_state_t));

  snake->head_row = 2;
  snake->head_col = 4;
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->live = true;

  unsigned int fruit_row = 2;
  unsigned int fruit_col = 9;

  game_state->num_snakes = 1;
  game_state->snakes = snake;
  game_state->num_rows = 18;

  unsigned int num_cols = 21;
  char** board = malloc(sizeof(char*) * game_state->num_rows);

  for (unsigned int i = 0; i < game_state->num_rows; i++) {
    board[i] = malloc(sizeof(char) * (num_cols + 1));
    if (i == 0 || i == game_state->num_rows - 1)
      strcpy(board[i], "####################\n");
    else
      strcpy(board[i], "#                  #\n");
  }

  board[fruit_row][fruit_col] = '*';
  board[snake->head_row][snake->head_col] = 'D';
  board[snake->tail_row][snake->tail_col] = 'd';
  board[snake->tail_row][snake->tail_col + 1] = '>';

  game_state->board = board;

  return game_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  free(state->snakes);
  for (unsigned int i = 0; i < state->num_rows; i++)
    free(state->board[i]);
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_rows; i++)
    fprintf(fp, "%s", state->board[i]);

  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd')
    return true;

  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
    return true;

  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>')
    return true;

  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^')
    return 'w';

  if (c == '<')
    return 'a';

  if (c == 'v')
    return 's';

  if (c == '>')
    return 'd';

  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W')
    return '^';

  if (c == 'A')
    return '<';

  if (c == 'S')
    return 'v';

  if (c == 'D')
    return '>';

  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.

  if (c == 'v' || c == 's' || c == 'S')
    return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W')
    return cur_row - 1;

  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
    return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A')
    return cur_col - 1;

  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  unsigned int cur_row = snake.head_row;
  unsigned int cur_col = snake.head_col;
  unsigned int next_row = get_next_row(cur_row, state->board[cur_row][cur_col]);
  unsigned int next_col = get_next_col(cur_col, state->board[cur_row][cur_col]);

  return state->board[next_row][next_col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes + snum;
  unsigned int cur_row = snake->head_row;
  unsigned int cur_col = snake->head_col;
  char head = state->board[cur_row][cur_col];
  unsigned int next_row = get_next_row(cur_row, head);
  unsigned int next_col = get_next_col(cur_col, head);

  state->board[next_row][next_col] = head;
  state->board[cur_row][cur_col] = head_to_body(head);
  snake->head_row = next_row;
  snake->head_col = next_col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes + snum;
  unsigned int cur_row = snake->tail_row;
  unsigned int cur_col = snake->tail_col;
  char tail = state->board[cur_row][cur_col];
  unsigned int next_row = get_next_row(cur_row, tail);
  unsigned int next_col = get_next_col(cur_col, tail);

  state->board[next_row][next_col] = body_to_tail(state->board[next_row][next_col]);
  state->board[cur_row][cur_col] = ' ';
  snake->tail_row = next_row;
  snake->tail_col = next_col;
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    snake_t *snake = state->snakes + i;
    char head = state->board[snake->head_row][snake->head_col];
    unsigned int next_head_row = get_next_row(snake->head_row, head);
    unsigned int next_head_col = get_next_col(snake->head_col, head);

    if (state->board[next_head_row][next_head_col] == '*') {
      update_head(state, i);
      add_food(state);
    }
    else if (state->board[next_head_row][next_head_col] == ' ') {
      update_head(state, i);
      update_tail(state, i);
    }
    else {
      state->board[snake->head_row][snake->head_col] = 'x';
      snake->live = false;
      continue;
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  size_t buffer_size = 8;
  char* buffer = malloc(sizeof(char) * buffer_size);
  if (buffer == NULL)
    return NULL;

  if (!fgets(buffer, (int) buffer_size, fp))
    return NULL;

  size_t buffer_len = strlen(buffer);
  while (buffer_len > 0 && buffer[buffer_len - 1] != '\n') {
    char* new_buffer = realloc(buffer, buffer_size * 2);
    if (new_buffer == NULL)
      return NULL;

    buffer_size *= 2;
    buffer = new_buffer;

    fgets(buffer + buffer_len, (int) (buffer_size - buffer_len), fp);
    buffer_len = strlen(buffer);
  }

  return buffer;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  size_t row_size = 8;
  char** board = malloc(sizeof(char*) * row_size);

  unsigned int i = 0;
  char* line;

  line = read_line(fp);
  while (line != NULL) {
    if (i >= row_size) {
      char** new_board = realloc(board, sizeof(char*) * row_size * 2);
      if (new_board == NULL)
        return NULL;

      board = new_board;
      row_size *= 2;
    }

    size_t len = strlen(line);
    board[i] = malloc(sizeof(char) * (len + 1));
    strcpy(board[i], line);
    free(line);
    i += 1;
    line = read_line(fp);
  }

  if (i > 2) {
    game_state_t* state = malloc(sizeof(game_state_t));
    state->board = board;
    state->num_rows = i;
    state->snakes = NULL;
    state->num_snakes = 0;
    return state;
  }

  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // find head from tail, because snakes are indexed by tail position
  int cnt = 0;
  char **board = state->board;

  for (unsigned int i = 0; i < state->num_rows; i ++)
    for (size_t j = 0; j < strlen(board[i]); j ++) {
      if (!is_tail(board[i][j]))
        continue;

      if (cnt == snum) {
        // find head from tail
        unsigned int x = i;
        unsigned int y = (unsigned int) j;
        char cur_chr = state->board[i][j];
        while (!is_head(cur_chr)) {
          x = get_next_row(x, cur_chr);
          y = get_next_col(y, cur_chr);
          cur_chr = state->board[x][y];
        }
        state->snakes[snum].head_row = x;
        state->snakes[snum].head_col = y;
        return;
      }
      cnt += 1;
    }

  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  unsigned  snakes_size = 1;
  snake_t *snakes = malloc(sizeof(snake_t) * snakes_size);

  unsigned int snum = 0;
  char **board = state->board;
  for (unsigned i = 0; i < state->num_rows; i++)
    for (size_t j = 0; j < strlen(board[i]); j ++) {
      if (!is_tail(board[i][j]))
        continue;

      if (snum >= snakes_size) {
        snake_t* new_snakes = realloc(snakes, sizeof(snake_t) * snakes_size * 2);
        if (new_snakes == NULL)
          return NULL;

        snakes = new_snakes;
        snakes_size *= 2;
      }

      // find head from tail
      unsigned int x = i;
      unsigned int y = (unsigned int) j;
      char cur_chr = state->board[i][j];
      while (!is_head(cur_chr)) {
        x = get_next_row(x, cur_chr);
        y = get_next_col(y, cur_chr);
        cur_chr = state->board[x][y];
      }

      snakes[snum].head_row = x;
      snakes[snum].head_col = y;
      snakes[snum].tail_row = i;
      snakes[snum].tail_col = (unsigned int) j;
      snakes[snum].live = true;

      snum += 1;
    }
  state->snakes = snakes;
  state->num_snakes = snum;

  return state;
}
