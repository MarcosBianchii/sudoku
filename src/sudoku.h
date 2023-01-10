#ifndef _SUDOKU_
#define _SUDOKU_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>

#define BOARD_SIZE 9

#define BLACK  "\033[0;30m" // 1
#define RED    "\033[0;31m" // 2
#define GREEN  "\033[0;32m" // 3
#define YELLOW "\033[0;33m" // 4
#define BLUE   "\033[0;34m" // 5
#define PURPLE "\033[0;35m" // 6
#define CYAN   "\033[0;36m" // 7
#define WHITE  "\033[0;37m" // 8

typedef struct Pos {
      short row;
      short col;
} pos_t;

typedef struct Settings {
      int default_difficulty;
      int hints_enabled;
} settings_t;

typedef struct Board_Colors {
      int cursor;
      int lines;
      int plus;
      int open_cells;
      int closed_cells;
      int hint_cells;
      int hud_up;
      int hud_down;
      int hud_text;
} colors_t;

typedef struct Game {
      char starting_pos[BOARD_SIZE][BOARD_SIZE];
      short board[BOARD_SIZE][BOARD_SIZE];
      short solved[BOARD_SIZE][BOARD_SIZE];
      short nums_count[BOARD_SIZE];
      short difficulty;
      colors_t colors;
      pos_t cursor;
      bool hints;
} game_t;

bool check_args(int argc, char *argv[]);
void throw_err(char *text);
game_t *game_new(char *difficulty);
void curses_init();
void game_loop(game_t *game);

void print_game(game_t game, short board[BOARD_SIZE][BOARD_SIZE], short nums_count[BOARD_SIZE]);

#endif // _SUDOKU_