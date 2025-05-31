#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>
#include <float.h>
#include "./mcts_yz.h"

#define pii pair<int, int>
#define puu pair<unsigned int, unsigned int>
using namespace std;

/****Action****/
string Action::get_key() {
    return to_string(x) + to_string(y) + to_string(n) + to_string(dir);
}


/****GameState****/
GameState::GameState(int user_state[12][12], int sheep_state[12][12], char turn) : turn(turn) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            this->user_state[i][j] = (user_state[i][j] +1) + '0';
            this->sheep_state[i][j] = sheep_state[i][j];
        }
    }
}

GameState::GameState(const GameState* state) : turn(state->turn) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            this->user_state[i][j] = state->user_state[i][j];
            this->sheep_state[i][j] = state->sheep_state[i][j];
        }
    }
}

GameState::~GameState() {}

vector<Action> GameState::get_actions() {
    vector<Action> actions;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (user_state[i][j] == turn && sheep_state[i][j] > 1) {
                for(int k = 0; k < 8; k++){
                    int x = i + directions8[k][0], y = j + directions8[k][1];
                    if(x >= 0 && x < 12 && y >= 0 && y < 12 && user_state[x][y]  == '1'){
                        for(int l = 1; l < sheep_state[i][j]; l++){
                            actions.push_back(Action(i, j, l, k));
                        }
                    }
                }
            }
        }
    }
    return actions;
}

vector<Action> GameState::get_inipos_action(){
    // vector cause duplicate actions (I directly modify this)
    unordered_map<string, Action> actions;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (user_state[i][j] == '1'){ // avaliable position
                for(int k = 0; k < 4; k++){
                    int x = i + directions4[k][0], y = j + directions4[k][1];
                    if(x >= 0 && x < 12 && y >= 0 && y < 12 && user_state[x][y]  == '0'){ // 0 for wall
                    Action new_action = (Action(i, j, 16, k));
                    actions[new_action.get_key()] = new_action;
                    }
                }            
            }
        }
    }
    vector<Action> actions_ret;
    for(auto action: actions){
        actions_ret.push_back(action.second);
    }

    return actions_ret;
}

string GameState::get_key() {
    /*
    This function returns the key of the state for the map
    */
    string key = "";
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            key += user_state[i][j];
            key += to_string(sheep_state[i][j]);
        }
    }
    return key;
}

void GameState::print_gamer_map(){
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 12; j++){
            cout << ' ' <<  user_state[i][j];
        }

        cout << endl;
    }
}

void GameState::print_sheep_map(){
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 12; j++){
            cout << " " + to_string(sheep_state[i][j]);
        }

        cout << endl;
    }
}

void GameState::next_state(Action& action) {
    /*
    This function update the state of the game according to the action
    */
    // deal with turns that does not have further actions
    if (action.x == -1) {
        turn = (char)((int)turn + 1);
        if(turn == '6')
            turn = '2';
        return;
    }
    int x = action.x, y = action.y, n = action.n, dir = action.dir;
    sheep_state[x][y] -= n;
    x += directions8[dir][0];
    y += directions8[dir][1];
    while(1){
        int new_x = x + directions8[dir][0], new_y = y + directions8[dir][1];
        if(new_x < 0 || new_x >= 12 || new_y < 0 || new_y >= 12 || user_state[new_x][new_y] != '1'){
            break;
        }
        x = new_x;
        y = new_y;
    }

    sheep_state[x][y] += n;
    user_state[x][y] = turn;
    turn = (char)((int)turn + 1);
    if(turn == '6')
        turn = '2';
}

void GameState::after_inipos_state(Action& action){
    /*
    This function update the state of the game according to the decided initial position
    */
    if (action.x == -1) {
        return;
    }

    int x = action.x, y = action.y, n = action.n, dir = action.dir;

    sheep_state[x][y] += n;
    user_state[x][y] = turn;
    turn = (char)((int)turn + 1);
    if(turn == '6')
        turn = '2';
}

bool GameState::is_terminal() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (sheep_state[i][j] > 1) {
                for(int k = 0; k < 8; k++){
                    if(user_state[i + directions8[k][0]][j + directions8[k][1]] == '1'){
                        return false;
                    }
                }
            }
            
        }
    }
    return true;
}

bool GameState::is_winner(char turn) {
    double turn_score = 0, new_score = 0;
    int temp_turn_score = 0, temp_new_score = 0;
    for (int j = 0; j < 12; j++){
        for (int k = 0; k < 12; k++){
            if(user_state[j][k] == turn){
                turn_score += pow(recursive_calculate(j, k, turn), 1.25);
            }
        }
    }
    temp_turn_score = (int)(turn_score+0.5);
    for (int i = 0; i < 4; i++){
        if (i + 2 == turn - '0'){
            continue;
        }

        char tmp_turn = i + 2 + '0';
        for (int j = 0; j < 12; j++){
            for (int k = 0; k < 12; k++){
                if(user_state[j][k] == tmp_turn){
                    new_score += pow(recursive_calculate(j, k, tmp_turn), 1.25);
                }
            }
        }
        temp_new_score = (int)(new_score+0.5);
        if(temp_new_score > temp_turn_score){
            // cout << "winner is : " << tmp_turn <<endl; /////
            return false;
        }
    }
    return true;
}

int GameState::recursive_calculate(int x, int y, char person){
    int score = 0;
    user_state[x][y] = '0';
    for(int i = 0; i < 4; i++){
        int new_x = x + directions4[i][0], new_y = y + directions4[i][1];
        if(new_x >= 0 && new_x < 12 && new_y >= 0 && new_y < 12 && user_state[new_x][new_y] == person){
            score += recursive_calculate(new_x, new_y, person);
        }
    }
    return score + 1;
}


/****MCTSNode****/
// Add a child node to this node
MCTSNode* MCTSNode::add_child(const GameState& state, const Action& action) {
    // "this" should be a pointer
    // if the turn is yours, save the pointer to the map using the key of the state + action
    MCTSNode* child = new MCTSNode(state, this, action);
    children.push_back(child);

    return child;
}

// Select a child node based on UCB criteria
MCTSNode* MCTSNode::select_child() {
    double max_ucb = -1;
    MCTSNode* selected_child = nullptr;
    for (MCTSNode* child : children) {
        double ucb = calculate_ucb(child);
        if (ucb > max_ucb) {
            max_ucb = ucb;
            selected_child = child;
        }
    }
    return selected_child;
}

double MCTSNode::calculate_ucb(MCTSNode* node) {
    if (node->visits == 0) {
        return DBL_MAX; // Ensure unvisited nodes are prioritized
    }
    return (node->score / node->visits) + sqrt(2 * log(this->visits) / node->visits);
}

/****MCTS_agent****/
MCTSNode* MCTS_agent::select_node(MCTSNode* node) {
    while (!node->children.empty()) {
        node = node->select_child();
    }
    return node;
}

void MCTS_agent::expand_node(MCTSNode* node) {
    node->state.turn = player_turn; // To get player action
    vector<Action> actions = node->state.get_actions(); // Get all possible 

    // cout << "selected state: " << endl; /////
    // node->state.print_sheep_map(); /////
    // cout << endl;
    // node->state.print_gamer_map(); /////
    
    if (actions.empty()) {
        node->visits++; // avoid keeping choose this node
        return ; // No valid actions to expand
    }

    // add child to selected node
    for(auto action_t: actions){
        GameState state_copy = node->state; // Avoid modify the current node's state
        state_copy.next_state(action_t); // Update the state with the chosen action_t
        MCTSNode* child_node = node->add_child(state_copy, action_t); // Add a child node with the updated state

        // update node map
        string StateKey = state_copy.get_key();
        string ActionKey = action_t.get_key();
        node_map[StateKey + ActionKey] = child_node;
    }
    
    // random select a child to rollout
    int child_index = ((int)rand()) % node->children.size(); 
    MCTSNode* selected_child = node->children[child_index];

    int rollout_result = rollout(selected_child->state);
    // cout << "rollout result: " << rollout_result << endl; /////
    backpropagate(selected_child, rollout_result);
}

int MCTS_agent::rollout(GameState state) {
    while (!state.is_terminal()) {
        vector<Action> actions = state.get_actions();
        if (actions.empty()) {
            Action non_action(-1, -1, -1, -1);
            state.next_state(non_action); // Skip the move if no valid actions
        } else {
            Action random_action = actions[rand() % actions.size()]; // Choose a random action
            state.next_state(random_action); // Update the state with the chosen action
        }
    }
    if (state.is_winner(player_turn)) {
        return 1; // Return 1 for a win
    } else {
        return 0; // Return 0 for a loss (or draw ?)
    }
}

// origin rollout need every player puts their sheep on the map
int MCTS_agent::inipos_rollout(GameState state){
    // random opponent sheep placement 
    for (int i = 0; i < 3; i++){
        vector<Action> ini_pos_action = state.get_inipos_action();
        Action random_action = ini_pos_action[rand() % ini_pos_action.size()];
        state.after_inipos_state(random_action);
    }

    while (!state.is_terminal()) {
        vector<Action> actions = state.get_actions();
        if (actions.empty()) {
            Action non_action(-1, -1, -1, -1);
            state.next_state(non_action); // Skip the move if no valid actions
        } else {
            Action random_action = actions[rand() % actions.size()]; // Choose a random action
            state.next_state(random_action); // Update the state with the chosen action
        }
    }

    if (state.is_winner(player_turn)) {
        return 1; // Return 1 for a win
    } else {
        return 0; // Return 0 for a loss (or draw ?)
    }
}

void MCTS_agent::backpropagate(MCTSNode* node, int result) {
    while (node != nullptr) {
        node->visits++;
        node->score += result;
        node = node->parent;
    }
}

Action MCTS_agent::get_best_action(MCTSNode* root) {
    double max_score = -1;
    Action best_action;
    for (MCTSNode* child : root->children) {
        double average_score = child->score / child->visits;
        // cout << "score: " << child->score << " visits: " << child->visits << endl;/////
        // cout << "average score: " << average_score << endl; /////
        // cout << "action: " << child->action.get_key() << endl; /////
        if (average_score > max_score) {
            max_score = average_score;
            best_action = child->action;
        }
    }
    // cout << "max_score: " << max_score << endl; /////
    return best_action;
}

// Delete the entire MCTS tree
void MCTS_agent::delete_tree(MCTSNode* node) {
    if (node == nullptr) {
        return;
    }
    for (MCTSNode* child : node->children) {
        delete_tree(child);
    }
    delete node;
}

MCTS_agent::MCTS_agent(int max_iter, int max_seconds, char player_turn) : max_iter(max_iter), max_seconds(max_seconds), player_turn(player_turn) {
    node_map = unordered_map<string, MCTSNode*>();
}

MCTS_agent::~MCTS_agent() {
    node_map.clear();
}

Action MCTS_agent::decide_inipos(GameState& state){
    Action non_action = Action(-1, -1, -1, -1);
    MCTSNode* root = new MCTSNode(state, nullptr, non_action);
    
    vector<Action> ini_pos_action = state.get_inipos_action();
    // add child to MCTS root
    for(auto action_t: ini_pos_action){
        GameState state_copy = root->state; // Avoid modify the current node's state
        state_copy.after_inipos_state(action_t); // Update the state with the chosen action_t
        MCTSNode* child_node = root->add_child(state_copy, action_t); // Add a child node with the updated state

        // update node map
        string StateKey = state_copy.get_key();
        string ActionKey = action_t.get_key();
        node_map[StateKey + ActionKey] = child_node;
    }
    
    // Perform MCTS iterations
    for (int iter = 0; iter < max_iter; iter++) {
        // Choose a node using UCB
        MCTSNode* selected_node = select_node(root);
        // cout << "selected node action: " << selected_node->action.get_key() <<endl;/////
        // cout << "selected node visit and score: " << selected_node->visits << " " << selected_node->visits << endl; /////

        // Expand the selected node by adding a child node (and rollout + backpropagate)
        int rollout_result = inipos_rollout(selected_node->state);
        backpropagate(selected_node, rollout_result);
    }

    Action best_action = get_best_action(root);

    return best_action;
}

// find the initial placement from middle
Action MCTS_agent::middle_inipos(int mapState[12][12]){
    int col = 12;
    int row = 12;

    int x = ceil(col/2);
    int y = ceil(row/2);
    int dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    bool visit[12][12] = {false};
    
    queue<pair<int, int>> q;
    q.push({x, y});
    visit[x][y] = true;
    while (!q.empty()){
        pair<int, int> current;
        current = q.front();
        q.pop();

        for (int i = 0; i < 4; i++){
            int dx = current.first + dir[i][0];
            int dy = current.second + dir[i][1];

            if (dx > 0 && dx < col && dy > 0 && dy < row){
                if (mapState[dx][dy] == -1){
                    Action inipos(current.first - 1 , current.second - 1, 16, 1);
                    return inipos;
                }

                else if (mapState[dx][dy] == 0 && visit[dx][dy] == false){
                    q.push({dx, dy});
                    visit[dx][dy] = true;
                }
            }
        }
    }
}

Action MCTS_agent::decide_step(GameState& state) {
    if (state.is_terminal()) {
        return Action(-1, -1, -1, -1);
    }

    /*
    modify here
    have an empty node (root)
    1. set the state of the root to the current state
    2. find the actions
    3. for each action, find the place of the child node using the map and put it into the root (remember to add the number of visit in root)
    */

    // set a subtree root to find the action for given state
    Action non_action = Action(-1, -1, -1, -1);
    MCTSNode* sub_root = new MCTSNode(state, nullptr, non_action);

    for (auto child: sub_root->children){        
        sub_root->visits += child->visits; // add child visits to temp root -> to calculate UCB
    }

    vector<Action> actions = sub_root->state.get_actions();
    string RootStateKey = sub_root->state.get_key();
    for (auto action: actions){
        string ActionKey = action.get_key();

        MCTSNode* finded_child = node_map[RootStateKey + ActionKey];
        sub_root->children.push_back(finded_child);
    }

    // Perform MCTS iterations
    for (int iter = 0; iter < max_iter; iter++) {
        // Choose a node using UCB
        MCTSNode* selected_node = select_node(sub_root);

        // Expand the selected node by adding a child node (and rollout + backpropagate)
        expand_node(selected_node);
    }

    sub_root->visits = 0; // update sub_root visits
    for (auto child: sub_root->children){        
        sub_root->visits += child->visits; // add child visits to temp root -> to calculate UCB
    }

    Action best_action = get_best_action(sub_root);

    delete sub_root;

    return best_action;
}
