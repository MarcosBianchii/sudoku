#include "sudoku.h"

int main(int argc, char *argv[]) {
      char *difficulty = NULL;
      if (argc > 1)
            difficulty = argv[1];

      if (!check_args(argc, argv))
            return 1;

      srand((unsigned)time(NULL));
      game_t *game = game_new(difficulty);
      if (!game) {
            throw_err("Could not allocate memory for game");
            return 1;
      }

      curses_init();
      game_loop(game);
      endwin();

      free(game);
      return 0;
}