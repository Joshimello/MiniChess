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

vector<Board> gen_child(Board board, vector<Move> actions, int player){

  // cout << "Parent board:" << endl;
  // for(int i = 0 ; i < 6; i++){
  //   for(int j = 0; j < 5; j++){
  //     cout << (int)board.board[0][i][j] << ' ';
  //   }
  //   cout << endl;
  // }

  vector<Board> child_boards;
  for(auto& move : actions){
    Board new_board = board;

    int from_x = move.first.first;
    int from_y = move.first.second;
    int to_x = move.second.first;
    int to_y = move.second.second;

    // cout << from_x << from_y << ' ' << to_x << to_y << endl;

    char temp = new_board.board[player][from_x][from_y];
    new_board.board[player][from_x][from_y] = new_board.board[player][to_x][to_y];
    new_board.board[player][to_x][to_y] = temp;

    child_boards.push_back(new_board);
  }

  // cout << "Children boards:" << endl;
  // for(auto& board : child_boards){
  //   for(int i = 0 ; i < 6; i++){
  //     for(int j = 0; j < 5; j++){
  //       cout << (int)board.board[0][i][j] << ' ';
  //     }
  //     cout << endl;
  //   }
  //   cout << endl;
  // }

  return child_boards;
}

int get_move_val(State state, int depth, int A, int B){
  if(depth == 0){
    int eval = state.evaluate();
    return eval;
  }

  if(state.player == 0){
    int maxeval = -1e9;
    state.get_legal_actions();
    vector<Board> child_boards = gen_child(state.board, state.legal_actions, 0);
    for(auto& board : child_boards){
      int eval = get_move_val(State(board, 1), depth-1, A, B);
      maxeval = max(maxeval, eval);
      A = max(A, eval);
      if(B <= A) break;
    }
    return maxeval;
  }

  else {
    int mineval = 1e9;
    state.get_legal_actions();
    vector<Board> child_boards = gen_child(state.board, state.legal_actions, 1);
    for(auto& board : child_boards){
      int eval = get_move_val(State(board, 0), depth-1, A, B);
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

  vector<Board> child_boards = gen_child(root->board, root->legal_actions, 0);
  for(auto& child_board : child_boards){
    State child_state(child_board, 1);

    int eval = get_move_val(child_state, 5, -1e9, 1e9);

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
