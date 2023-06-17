#include <iostream>
#include <fstream>
#include <utility>
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
        board.board[pl][i][j] = c;
      }
    }
  }

  root = new State(board, player);
  root->get_legal_actions();
}

int get_move_val(State* state, int depth, int A, int B){
  if(depth == 0){
    return state->evaluate();
  }

  if(state->player == 0){
    int maxeval = -1e9;
    for(auto& move : state->legal_actions){
      int eval = get_move_val(state->next_state(move), depth-1, A, B);
      maxeval = max(maxeval, eval);
      A = max(A, eval);
      if(B <= A) break;
    }
    return maxeval;
  }

  else {
    int mineval = 1e9;
    for(auto& move : state->legal_actions){
      int eval = get_move_val(state->next_state(move), depth-1, A, B);
      mineval = min(mineval, eval);
      B = min(B, eval);
      if(B <= A) break;
    }
    return mineval;
  }

}

void write_valid_spot(ofstream& fout) {
  if(!root->legal_actions.size()){
    root->get_legal_actions();
  }

  int besteval = -1e9;
  int moveindex = 0;
  int bestmoveindex = 0;

  cout << "Evals: ";

  for(auto& move : root->legal_actions){
    int eval = get_move_val(root->next_state(move), 6, -1e9, 1e9);
    cout << eval << ' ';

    if(eval > besteval){
      besteval = eval;
      bestmoveindex = moveindex;
    }

    moveindex++;
  }

  cout << endl;

  Move move = root->legal_actions[bestmoveindex];

  cout << besteval << ' ' << bestmoveindex << endl;
  cout << move.first.first << " " << move.first.second << " "\
       << move.second.first << " " << move.second.second << endl;

  fout << move.first.first << " " << move.first.second << " "\
       << move.second.first << " " << move.second.second << endl;
  
  fout.flush();
}

int main(int, char** argv) {
  ifstream fin(argv[1]);
  ofstream fout(argv[2]);

  cout << "_______________________" << endl;

  read_board(fin);
  write_valid_spot(fout);

  cout << "_______________________" << endl;

  fin.close();
  fout.close();

  return 0;
}
