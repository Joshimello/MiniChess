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
  long long score;
  int visits;
  TreeNode* parent;
  vector<TreeNode*> children;
  TreeNode(State* S, long long s, int v, TreeNode* p){
    state = S;
    score = s;
    visits = v;
    parent = p;
  }
};

float UCB(int score, int visits, int p_visits){
  if(visits == 0) return 1e9;
  float avg = (float)score / (float)visits;
  return avg + 1.414 * (float)sqrt(log((float)p_visits) / (float)visits);
}

TreeNode* Selection(TreeNode* node){
  while (node->children.size() > 0) {
    float maxucb = -1e9;
    TreeNode* selectedNode;
    for(TreeNode* child : node->children){
      float ucb = UCB(child->score, child->visits, node->visits);
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
  if(node->visits == 0){
    return node;
  }
  else {
    float maxucb = -1e9;
    TreeNode* selectedNode;
    node->state->get_legal_actions();
    for(Move &move : node->state->legal_actions){
      TreeNode* child = new TreeNode(node->state->next_state(move), 0, 0, node);
      node->children.push_back(child);
      float ucb = UCB(child->score, child->visits, node->visits);
      if(ucb > maxucb){
        maxucb = ucb;
        selectedNode = child;
      }
    }
    return Expansion(selectedNode);
  }
}

int Rollout(State* state, int depth){
  if(depth == 0){
    int eval = state->evaluate();
    if(eval > 0) return 1;
    if(eval == 0) return 0;
    return -1;
  }
  if(state->game_state){
    if(state->game_state == WIN && state->player == plyr)
      return 1;
    if(state->game_state == WIN && state->player == !plyr)
      return -1;
    if(state->game_state == DRAW)
      return 0;
    if(state->game_state == NONE && state->player == plyr)
      return 1;
    if(state->game_state == NONE && state->player == !plyr)
      return -1;
  }
  state->get_legal_actions();
  int randomIndex = rand() % state->legal_actions.size();    
  Move randomMove = state->legal_actions[randomIndex];
  return Rollout(state->next_state(randomMove), depth-1);
}

void Backpropagation(TreeNode* node, int score){
  while(node != nullptr){
    node->visits += 1;
    node->score += score;
    node = node->parent;
  }
}

int BestMove(TreeNode* node){
  float max = 0.0;
  int bestmoveindex = 0;
  int i = 0;
  for(TreeNode* child : node->children){
    float avg = (float)child->score / (float)child->visits;
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
    TreeNode* node = Selection(root);
    node = Expansion(node);
    int score = Rollout(node->state, 1e9);
    Backpropagation(node, score);
    // cout << "ROOT: " << root->score << "/" << root->visits << endl;
  }
  // printf("X");
  return BestMove(root);
}

void write_valid_spot(ofstream& fout) {
  if(!root->legal_actions.size()){
    root->get_legal_actions();
  }

  TreeNode* rootNode = new TreeNode(root, 0, 0, nullptr);
  for(auto& move : root->legal_actions){
    TreeNode* newNode = new TreeNode(root->next_state(move), 0, 0, rootNode);
    rootNode->children.push_back(newNode);
    // cout << move.first.first << " " << move.first.second << " - "\
         << move.second.first << " " << move.second.second << endl;
  }

  Move move = root->legal_actions[MCTS(rootNode, 100000)];

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