#pragma once

/*Board Size, Don't change!*/
#define BOARD_H 6
#define BOARD_W 5

/*You can modify these things in development for fast testing*/
/*When TA run your program, we will use default settings (123, 10, 50)*/
#define RANDOM_SEED 123
#define timeout 10
#define MAX_STEP 50


/*Which character/words for pieces*/
/* By default, the pieces are '♟', '♜', '♞', '♝', '♛', '♚' from unicode*/
// #define PIECE_STR_LEN 1
// const char PIECE_TABLE[2][7][5] = {
//   {" ", "♟", "♜", "♞", "♝", "♛", "♚"},
//   {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
// };
// #define PIECE_STR_LEN 2
// const char PIECE_TABLE[2][7][5] = {
//   {"  ", "wP", "wR", "wK", "wB", "wQ", "wK"},
//   {"  ", "bP", "bR", "bK", "bB", "bQ", "bK"},
// };
#define PIECE_STR_LEN 1
const char PIECE_TABLE[2][7][30] = {
  {" ", "P", "R", "H", "B", "Q", "K"},
  {" ", "\033[1;31mP\033[0m", "\033[1;31mR\033[0m", "\033[1;31mH\033[0m", "\033[1;31mB\033[0m", "\033[1;31mQ\033[0m", "\033[1;31mK\033[0m"},
};