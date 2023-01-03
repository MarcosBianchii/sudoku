#include "sudoku.h"

void print_help() {
      int default_difficulty = 35;
      FILE *fp = fopen("files/settings.txt", "r");
      if (fp) {
            char line[20];
            int value = 0;
            while (fscanf(fp, "%[^=]=%i\n", line, &value) != EOF) {
                  if (strcmp(line, "default_difficulty") == 0)
                        default_difficulty = value;
            }

            fclose(fp);
      }

      printf(\
GREEN"\n> "WHITE"/sudoku "GREEN"[difficulty / command]\n"WHITE\
"\n"\
"Difficulty: "YELLOW"0 TO 80 "BLACK"(default: %i)\n"\
"\n"\
YELLOW"Controls: \n"\
WHITE"WASD "BLACK"&&"WHITE" ARROWS"BLACK"\t-> "GREEN"Move cursor.\n"\
WHITE"0 "BLACK"TO"WHITE" 9"BLACK"\t\t-> "GREEN"Sets cell number.\n"\
WHITE"E "BLACK"&&"WHITE" +"BLACK"\t\t-> "GREEN"Increment number.\n"\
WHITE"Q "BLACK"&&"WHITE" -"BLACK"\t\t-> "GREEN"Decrement number.\n"\
WHITE"R "BLACK"&&"WHITE" ."BLACK"\t\t-> "GREEN"Reset board.\n"\
WHITE"X "BLACK"&&"WHITE" END KEY"BLACK"\t-> "GREEN"Exit.\n"\
WHITE"H "BLACK"&&"WHITE" HOME KEY"BLACK"\t-> "GREEN"Toggle hints.\n"\
"\n"\
YELLOW"Commands: \n"\
BLACK"-"WHITE"help"BLACK"           -> "GREEN"shows this help.\n"\
BLACK"-"WHITE"version"BLACK"        -> "GREEN"shows game version.\n"\
BLACK"-"WHITE"toggle"BLACK"-"WHITE"hints"BLACK"   -> "GREEN"toggle hints.\n"\
BLACK"-"WHITE"set"BLACK"-"WHITE"difficulty"BLACK" -> "GREEN"set default difficulty.\n\n", default_difficulty);
}

void print_version() {
      printf(YELLOW"version: 1.0\n"WHITE);
}

void toggle_hints() {
      FILE *fp = fopen("files/settings.txt", "r");
      if (!fp) {
            throw_err("Could not open settings file");
            return;
      }

      settings_t settings = (settings_t) {
            .default_difficulty = 35,
            .hints_enabled = 1
      };

      char line[20];
      int value = 0;
      while (fscanf(fp, "%[^=]=%i\n", line, &value) != EOF) {
            if (strcmp(line, "default_difficulty") == 0)
                  settings.default_difficulty = value;
            else if (strcmp(line, "hints_enabled") == 0)
                  settings.hints_enabled = value;
      }

      fclose(fp);
      fp = fopen("files/settings.txt", "w");

      fprintf(fp, "default_difficulty=%i\n", settings.default_difficulty);
      fprintf(fp, "hints_enabled=%i\n", !settings.hints_enabled);
      fclose(fp);

      printf(YELLOW"Hints are now turned %s\n"WHITE, !settings.hints_enabled ? "on" : "off");
}

void set_difficulty(int new_difficulty) {
      FILE *fp = fopen("files/settings.txt", "r");
      if (!fp) {
            throw_err("Could not open settings file");
            return;
      }

      settings_t settings = (settings_t) {
            .default_difficulty = new_difficulty,
            .hints_enabled = 1,
      };

      char line[20];
      int value = 0;
      while (fscanf(fp, "%[^=]=%i\n", line, &value) != EOF) {
            if (strcmp(line, "hints_enabled") == 0)
                  settings.hints_enabled = value;
      }

      fclose(fp);
      fp = fopen("files/settings.txt", "w");

      fprintf(fp, "default_difficulty=%i\n", settings.default_difficulty);
      fprintf(fp, "hints_enabled=%i\n", settings.hints_enabled);
      fclose(fp);

      printf(YELLOW"Difficulty set to %i\n"WHITE, settings.default_difficulty);
}

bool check_args(int argc, char *argv[]) {
      bool is_command[argc];
      memset(is_command, 0, argc * sizeof(bool));
      for (int i = 0; i < argc; i++)
            if (argv[i][0] == '-')
                  is_command[i] = true;

      for (int i = 0; i < argc; i++) {
            if (is_command[i]) {
                  if (strcmp(argv[i], "-help") == 0)
                        print_help();
                  else if (strcmp(argv[i], "-version") == 0)
                        print_version();
                  else if (strcmp(argv[i], "-toggle-hints") == 0)
                        toggle_hints();
                  else if (strcmp(argv[i], "-set-difficulty") == 0)
                        if (i + 1 < argc)
                              set_difficulty(atoi(argv[i + 1]));
                        else  throw_err("Must specify difficulty");
                  else
                        throw_err("Invalid command");

                  return false;
            }

            else {
                  int dif_arg = atoi(argv[i]);
                  if (dif_arg < 0 || dif_arg > 80) {
                        throw_err("Invalid difficulty");
                        return false;
                  }
            }
      }

      return true;
}

int rand_num() {
      return (rand() % BOARD_SIZE) + 1;
}

bool is_valid(short board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
      short num = board[row][col];
      for (int x = 0; x < BOARD_SIZE; x++) {
            if (x != row && board[x][col] == num)
                  return false;
            if (x != col && board[row][x] == num)
                  return false;
      }

      short row_start = row - row % 3;
      short col_start = col - col % 3;
      for (int i = row_start; i < row_start + 3; i++)
            for (int j = col_start; j < col_start + 3; j++)
                  if (i != row && j != col && board[i][j] == num)
                        return false;
      
      return true;
}

bool solve_sudoku(short board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
      if (row == BOARD_SIZE - 1 && col == BOARD_SIZE)
            return true;

      if (col == BOARD_SIZE) {
            row++;
            col = 0;
      }

      if (board[row][col] != 0)
            return solve_sudoku(board, row, col + 1);

      for (int num = 1; num <= BOARD_SIZE; num++) {
            board[row][col] = num;
            if (is_valid(board, row, col) && solve_sudoku(board, row, col + 1))
                  return true;
      }

      board[row][col] = 0;
      return false;
}

void load_positions(char starting_pos[BOARD_SIZE][BOARD_SIZE], short board[BOARD_SIZE][BOARD_SIZE], int difficulty) {
      int count = 0;
      while (count < difficulty) {
            int row = rand_num() - 1;
            int col = rand_num() - 1;
            if (starting_pos[row][col] == 0) {
                  starting_pos[row][col] = 1;
                  count++;
            }
      }
}

void board_init(game_t *game, short board[BOARD_SIZE][BOARD_SIZE], int difficulty) {
      for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                  do board[i][j] = rand_num();
                  while (!is_valid(board, i, j));

      for (int x = 3; x < BOARD_SIZE; x++) {
            do board[x][0] = rand_num();
            while (!is_valid(board, x, 0));

            do board[0][x] = rand_num();
            while (!is_valid(board, 0, x));

            do board[x][x] = rand_num();
            while (!is_valid(board, x, x));
      }

      if (!solve_sudoku(board, 0, 0))
            board_init(game, board, difficulty);
      
      load_positions(game->starting_pos, board, difficulty);
      for (int i = 0; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++) {
                  if (game->starting_pos[i][j] == 0)
                        board[i][j] = 0; 
                  if (board[i][j] != 0)
                        game->nums_count[board[i][j]-1]++;
            }
}

void get_colors(game_t *game) {
      FILE *fp = fopen("files/colors.txt", "r");
      char things_color[20];
      int color;

      colors_t colors = (colors_t) {
            .cursor = 3,
            .lines = 1,
            .plus = 1,
            .open_cells = 8,
            .closed_cells = 5,
            .hint_cells = 4,
            .hud_up = 3,
            .hud_down = 1,
            .hud_text = 8,
      };

      if (!fp) {
            game->colors = colors;
            return;
      }

      while (fscanf(fp, "%[^=]=%i\n", things_color, &color) != EOF) {
            if (strcmp(things_color, "cursor") == 0)            colors.cursor = color;
            else if (strcmp(things_color, "lines") == 0)        colors.lines = color;
            else if (strcmp(things_color, "plus") == 0)         colors.plus = color;
            else if (strcmp(things_color, "open_cells") == 0)   colors.open_cells = color;
            else if (strcmp(things_color, "closed_cells") == 0) colors.closed_cells = color;
            else if (strcmp(things_color, "hint_cells") == 0)   colors.hint_cells = color;
            else if (strcmp(things_color, "hud_up") == 0)       colors.hud_up = color;
            else if (strcmp(things_color, "hud_down") == 0)     colors.hud_down = color;
            else if (strcmp(things_color, "hud_text") == 0)     colors.hud_text = color;
      }

      fclose(fp);
      game->colors = colors;
}

void settings_init(game_t *game, char *difficulty) {
      FILE *fp = fopen("files/settings.txt", "r");
      if (!fp) {
            game->hints = true;
            game->difficulty = 35;
            return;
      }

      settings_t settings = (settings_t) {
            .default_difficulty = 35,
            .hints_enabled = 1,
      };

      char setting[20];
      int value = 0;
      while (fscanf(fp, "%[^=]=%i\n", setting, &value) != EOF) {
            if (strcmp(setting, "default_difficulty") == 0) settings.default_difficulty = value;
            else if (strcmp(setting, "hints_enabled") == 0) settings.hints_enabled = value;
      }

      fclose(fp);
      game->hints = settings.hints_enabled;

      if (difficulty)
            game->difficulty = atoi(difficulty);
      else  game->difficulty = settings.default_difficulty;
}

game_t *game_new(char *difficulty) {
      game_t *game = calloc(1, sizeof(game_t));
      if (!game) return NULL;

      game->cursor.row = 4;
      game->cursor.col = 4;

      get_colors(game);
      settings_init(game, difficulty);
      board_init(game, game->board, game->difficulty);
      return game;
}

void throw_err(char *text) {
      printf(RED"ERROR: "WHITE"%s\n", text);
}

void print_game(game_t game, short board[BOARD_SIZE][BOARD_SIZE], short *nums_count) {
      for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++)
                  printf("%i ", board[i][j]);
            printf("\n");
      }

      printf("\nCounts:\n");
      for (int i = 0; i < BOARD_SIZE; i++)
            printf("%i ", i+1);
      printf("\n");
      for (int i = 0; i < BOARD_SIZE; i++)
            printf("%i ", nums_count[i]);
      printf("\n");

      printf("\nHints: %s\n", game.hints ? "enabled" : "disabled");
      printf("Difficulty: %i\n", game.difficulty);
      printf("Cursor: %i %i\n\n", game.cursor.row, game.cursor.col);
      printf("Starting positions:\n");
      for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++)
                  printf("%i ", game.starting_pos[i][j]);
            printf("\n");
      }

      printf("\n");
}

bool end(game_t game) {
      for (int i = 0; i < BOARD_SIZE; i++)
            if (game.nums_count[i] != 9)
                  return false;
      
      for (int i = 0; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++)
                  if (!is_valid(game.board, i, j))
                        return false;

      return true;
}

void init_pairs()
{
      init_pair(1, COLOR_BLACK,   -1);
      init_pair(2, COLOR_RED,     -1);
      init_pair(3, COLOR_GREEN,   -1);
      init_pair(4, COLOR_YELLOW,  -1);
      init_pair(5, COLOR_BLUE,    -1);
      init_pair(6, COLOR_MAGENTA, -1);
      init_pair(7, COLOR_CYAN,    -1);
      init_pair(8, COLOR_WHITE,   -1);
}

void curses_init()
{
      initscr();
      noecho();
      raw();
      start_color();
      use_default_colors();
      curs_set(0);
      keypad(stdscr, TRUE);
      init_pairs();
}

void print_board(game_t game) {
      colors_t colors = game.colors;
      int x, y, lines_count = 0;
      getmaxyx(stdscr, y, x);
      short board_x = x/2 - 11;
      short board_y = y/2 - 11;
      short hud_x = board_x;
      short hud_y = board_y + 13;
      erase();
      for (int i = 0; i < BOARD_SIZE; i++) {
            move(board_y+i+1+lines_count, board_x);
            for (int j = 0; j < BOARD_SIZE; j++) {
                  if (game.starting_pos[i][j] == 1)
                        attron(COLOR_PAIR(colors.closed_cells));
                  else  attron(COLOR_PAIR(colors.open_cells));
                  if (game.board[game.cursor.row][game.cursor.col]
                  ==  game.board[i][j] && game.board[i][j] != 0
                  &&  game.hints)
                        attron(COLOR_PAIR(colors.hint_cells));
                  if (!is_valid(game.board, i, j))
                        attron(COLOR_PAIR(2));
                  if (i == game.cursor.row && j == game.cursor.col)
                        attron(COLOR_PAIR(colors.cursor));
                  if (game.board[i][j] != 0)
                        printw("%i ", game.board[i][j]);
                  else if (i == game.cursor.row && j == game.cursor.col)
                        addstr("0 ");
                  else addstr("  ");
                  if ((j+1) % 3 == 0 && j != BOARD_SIZE-1) {
                        attron(COLOR_PAIR(colors.lines));
                        addstr("| ");
                  }
            }
            
            move(board_y+i+2+lines_count, board_x);
            if ((i+1) % 3 == 0 && i != BOARD_SIZE-1) {
                  lines_count++;
                  for (int j = 0; j < BOARD_SIZE; j++) {
                        attron(COLOR_PAIR(colors.lines));
                        addstr("- ");
                        if ((j+1) % 3 == 0 && j != BOARD_SIZE-1) {
                              attron(COLOR_PAIR(colors.plus));
                              addstr("+ ");
                        }
                  }
            }
      }

      move(hud_y, hud_x);
      for (int i = 0; i < BOARD_SIZE; i++) {
            attron(COLOR_PAIR(colors.hud_up));
            printw("%i ", i+1);
            if ((i+1) % 3 == 0 && i != BOARD_SIZE-1) {
                  attron(COLOR_PAIR(colors.lines));
                  addstr("| ");
            }
      }
      
      move(hud_y+1, hud_x);
      attron(COLOR_PAIR(8));
      for (int i = 0; i < BOARD_SIZE; i++) {
            int num = 9 - game.nums_count[i];
            if (num == 0)
                  attron(COLOR_PAIR(3));
            else if (num < 0) {
                  attron(COLOR_PAIR(2));
                  num *= -1;
            } else attron(COLOR_PAIR(colors.hud_down));

            printw("%i ", num);
            if ((i+1) % 3 == 0 && i != BOARD_SIZE-1) {
                  attron(COLOR_PAIR(colors.lines));
                  addstr("| ");
            }
      }
      
      move(hud_y+2, hud_x);
      attron(COLOR_PAIR(colors.hud_text));
      addstr("SUDOKU             ");
      if (game.difficulty < 10) addch(' ');
      printw("%i", game.difficulty);

      refresh();  
}

bool cell_is_changeable(char starting_pos[BOARD_SIZE][BOARD_SIZE], int row, int col) {
      return starting_pos[row][col] == 0;
}

bool make_play(game_t *game, int c) {
      switch (tolower(c)) {
      case 'w': case KEY_UP:
            game->cursor.row--;
            if (game->cursor.row < 0)
                  game->cursor.row = BOARD_SIZE-1;
            break;
      case 's': case KEY_DOWN:
            game->cursor.row++;
            if (game->cursor.row >= BOARD_SIZE)
                  game->cursor.row = 0;
            break;
      case 'a': case KEY_LEFT:
            game->cursor.col--;
            if (game->cursor.col < 0)
                  game->cursor.col = BOARD_SIZE-1;
            break;
      case 'd': case KEY_RIGHT:
            game->cursor.col++;
            if (game->cursor.col >= BOARD_SIZE)
                  game->cursor.col = 0;
            break;
      case '1': case '2': case '3':
      case '4': case '5': case '6':
      case '7': case '8': case '9':
            if (cell_is_changeable(game->starting_pos, game->cursor.row, game->cursor.col)) {
                  if (game->board[game->cursor.row][game->cursor.col] != 0)
                        game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]--;
                  game->board[game->cursor.row][game->cursor.col] = c - '0';
                  game->nums_count[c - '0' - 1]++;
            }

            break;
      case '0':
            if (cell_is_changeable(game->starting_pos, game->cursor.row, game->cursor.col)) {
                  game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]--;
                  game->board[game->cursor.row][game->cursor.col] = 0;
            }

            break;
      case 'e': case '+':
            if (cell_is_changeable(game->starting_pos, game->cursor.row, game->cursor.col)) {
                  if (game->board[game->cursor.row][game->cursor.col] != 0)
                        game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]--;
                  game->board[game->cursor.row][game->cursor.col]++;
                  if (game->board[game->cursor.row][game->cursor.col] >= 10)
                        game->board[game->cursor.row][game->cursor.col] = 0;
                  if (game->board[game->cursor.row][game->cursor.col] != 0)
                        game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]++;
            }

            break;
      case 'q': case '-':
            if (cell_is_changeable(game->starting_pos, game->cursor.row, game->cursor.col)) {
                  if (game->board[game->cursor.row][game->cursor.col] != 0)
                        game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]--;
                  game->board[game->cursor.row][game->cursor.col]--;
                  if (game->board[game->cursor.row][game->cursor.col] <= -1)
                        game->board[game->cursor.row][game->cursor.col] = 9;
                  if (game->board[game->cursor.row][game->cursor.col] != 0)
                        game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]++;
            }

            break;
      case 'f': case '*':
            if (!cell_is_changeable(game->starting_pos, game->cursor.row, game->cursor.col))
                  break;

            int prev_value = game->board[game->cursor.row][game->cursor.col];
            for (int i = prev_value; i <= 10; i++) {
                  if (i == 10) {
                        game->board[game->cursor.row][game->cursor.col] = 0;
                        break;
                  }

                  game->board[game->cursor.row][game->cursor.col] = i;
                  if (is_valid(game->board, game->cursor.row, game->cursor.col)) {
                        if (prev_value != 0)
                              game->nums_count[prev_value - 1]--;

                       if (i != 0)
                              game->nums_count[game->board[game->cursor.row][game->cursor.col] - 1]++;
                        game->board[game->cursor.row][game->cursor.col] = i;
                        break;
                  }
            }

            break;
      case 'h': case KEY_HOME:
            game->hints = !game->hints;
            break;
      case 'r': case '.':
            for (int i = 0; i < BOARD_SIZE; i++)
                  for (int j = 0; j < BOARD_SIZE; j++)
                        if (cell_is_changeable(game->starting_pos, i, j)
                        &&  game->board[i][j] != 0) {
                              game->nums_count[game->board[i][j] - 1]--;
                              game->board[i][j] = 0;
                        }
            break;
      case 'x': case KEY_END:
            return false;
      }

      return true;
}

void win_screen(game_t game) {
      game.colors = (colors_t) {
            .cursor = 3,
            .lines = 3,
            .plus = 3,
            .open_cells = 3,
            .closed_cells = 3,
            .hint_cells = 3,
            .hud_up = 3,
            .hud_down = 3,
            .hud_text = 3,
      };

      print_board(game);
      getch();
}

void game_loop(game_t *game) {
      while (!end(*game)) {
            print_board(*game);
            if (!make_play(game, getch()))
                  return;
      }

      win_screen(*game);
}