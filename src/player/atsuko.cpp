#include <iostream>
#include <fstream>
#include "../config.hpp"
#include "../state/state.hpp"
using namespace std;

State* root;

void read_board(ifstream& fin) {
  Board board;
  int player;
  fin >> player;

  for (int pl=0; pl<2; pl++) {
    for (int i=0; i<BOARD_H; i++) {
      for (int j=0; j<BOARD_W; j++) {
        int c; fin >> c;
        // cout << c << " ";
        board.board[pl][i][j] = c;
      }
      // cout << endl;
    }
  }
  root = new State(board, player);
  root->get_legal_actions();
}

void write_valid_spot(ofstream& fout) {
  while(true) {

    if(!root->legal_actions.size()){
      root->get_legal_actions();
    }

    auto actions = root->legal_actions;
    auto move = actions[0];

    fout << move.first.first << " " << move.first.second << " "\
         << move.second.first << " " << move.second.second << endl;
    
    fout.flush();
    break;
  }
}

int main(int, char** argv) {
  ifstream fin(argv[1]);
  ofstream fout(argv[2]);

  read_board(fin);
  write_valid_spot(fout);

  fin.close();
  fout.close();
  return 0;
}
