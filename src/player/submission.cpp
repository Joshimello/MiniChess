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

int get_move_val(State* state, int depth, int A, int B, int me){
  if(depth == 0){
    return state->evaluate();
  }

  if(me){
    int maxeval = -1e9;
    for(auto& move : state->legal_actions){
      int eval = get_move_val(state->next_state(move), depth-1, A, B, !me);
      maxeval = max(maxeval, eval);
      A = max(A, eval);
      if(B <= A) break;
    }
    return maxeval;
  }

  else {
    int mineval = 1e9;
    for(auto& move : state->legal_actions){
      int eval = get_move_val(state->next_state(move), depth-1, A, B, !me);
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

  for(auto& move : root->legal_actions){
    int eval = get_move_val(root->next_state(move), 6, -1e9, 1e9, 0);
    if(eval > besteval){
      besteval = eval;
      bestmoveindex = moveindex;
    }
    moveindex++;
  }

  Move move = root->legal_actions[bestmoveindex];
  fout << move.first.first << " " << move.first.second << " "\
       << move.second.first << " " << move.second.second << endl;
  
  fout.flush();
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