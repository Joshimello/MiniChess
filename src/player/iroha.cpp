#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "../config.hpp"
#include "../state/state.hpp"
using namespace std;

State* root;
int plyr;

void read_board(ifstream& fin) {
  Board board;
  int player;
  fin >> player;

  plyr = player;

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

class TreeNode {
public:
  State* state;
  double t; //total score
  int n; //visits
  TreeNode* parent;
  vector<TreeNode*> children;
  TreeNode(State* s, int T, int N, TreeNode* p){
    state = s;
    t = T;
    n = N;
    parent = p;
  }
};

double UCB(int t, int n, int N){
  if(n == 0) return 1e9;
  double avg = t / static_cast<double>(n);
  return avg + 1.414 * sqrt(log(static_cast<double>(N)) / static_cast<double>(n));
}

TreeNode* Selection(TreeNode* node){
  while (node->children.size() > 0) {
    double maxucb = 0;
    TreeNode* selectedNode = node->children[0];
    for(TreeNode* child : node->children){
      double ucb = UCB(child->t, child->n, node->n);
      if(ucb > maxucb){
        maxucb = ucb;
        selectedNode = child;
      }
    }
    node = selectedNode;
  }
  return node;
}

TreeNode* Expansion(TreeNode* node){
  if(node->n == 0){
    return node;
  }
  else {
    node->state->get_legal_actions();
    for(auto &move : node->state->legal_actions){
      TreeNode* newNode = new TreeNode(node->state->next_state(move), 0, 0, node);
      node->children.push_back(newNode);
    }
    return node->children[0];
  }
}

double Rollout(State* state, int depth){
  if(depth == 0){
    int eval = state->evaluate();
    if(eval > 0) return 1.0;
    if(eval == 0) return 0.5;
    return 0.0;
  }
  if(state->game_state){
    if(state->game_state == WIN && state->player == plyr)
      return 1.0;
    if(state->game_state == DRAW)
      return 0.5;
    return 0.0;
  }
  state->get_legal_actions();
  int randomIndex = rand() % state->legal_actions.size();    
  Move randomMove = state->legal_actions[randomIndex];
  return Rollout(state->next_state(randomMove), depth-1);
}

void Backpropagation(TreeNode* node, double score){
  while(node != nullptr){
    node->n += 1;
    if(score > 0) node->t += score;
    node = node->parent;
  }
}

int BestMove(TreeNode* node){
  double max = 0.0;
  int bestmoveindex = 0;
  int i = 0;
  for(TreeNode* child : node->children){
    double avg = 0;
    if(child->n != 0){
      avg = child->t / static_cast<double>(child->n);
    }
    cout << avg << endl;
    if(avg > max){
      max = avg;
      bestmoveindex = i;
    }
    i++;
  }
  cout << "selected: " << bestmoveindex << endl;
  return bestmoveindex;
}

int MCTS(TreeNode* root, int iter){
  for(int i = 0; i < iter; i++){
    // printf("S");
    TreeNode* node = Selection(root);
    // printf("E");
    node = Expansion(node);
    // printf("R");
    double score = Rollout(node->state, 50);
    // printf("B");
    Backpropagation(node, score);
    // cout << "ROOT: " << root->t << "/" << root->n << endl;
  }
  // printf("X");
  return BestMove(root);
}

void write_valid_spot(ofstream& fout) {
  cout << "do smth" << endl;
  if(!root->legal_actions.size()){
    root->get_legal_actions();
  }

  TreeNode* rootNode = new TreeNode(root, 0, 0, nullptr);
  for(auto& move : root->legal_actions){
    TreeNode* newNode = new TreeNode(root->next_state(move), 0, 0, rootNode);
    rootNode->children.push_back(newNode);
    cout << move.first.first << " " << move.first.second << " - "\
         << move.second.first << " " << move.second.second << endl;
  }

  Move move = root->legal_actions[MCTS(rootNode, 10000)];

  fout << move.first.first << " " << move.first.second << " "\
       << move.second.first << " " << move.second.second << endl;
  
  fout.flush();
}

int main(int, char** argv) {
  srand(time(0));

  ifstream fin(argv[1]);
  ofstream fout(argv[2]);

  read_board(fin);
  write_valid_spot(fout);

  fin.close();
  fout.close();

  return 0;
}