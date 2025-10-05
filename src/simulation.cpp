#include "global.h"
#include "helpers.h"
#include "solver.h"

using namespace std;
using namespace CELL_LABELS;

//stores the probability of each cell being a mine
map<pair<int,int>, double> PROB_MAP;

//when given an init sate, return the resuling state
struct Node{
    multimap<int, vector<pair<int,int>>> state;
    pair<int, int> pos;
    bool mine;
    int layer;
    vector<Node*> next;
};

//make a functor to compare nodes based on layer. Make into min heap by layer
struct NodeCmp{
    bool operator()(Node* a, Node* b) {
        return a->layer > b->layer;             //a has less priority if it is bigger - min heap
    }
}; 

//create a new node with the resulting cells from the previous state
Node* create_node(multimap<int, vector<pair<int,int>>> init_state){
    Node* newNode = new Node();
    newNode->state = init_state;
    newNode -> pos = {0,0};
    newNode -> layer =0;
    newNode ->next = {};
    newNode -> mine = false;
    return newNode;
}

//recursively clear the tree by DFS
void clear_node(Node* node){
    if (node == nullptr) return;
    for (auto &n: node->next){
        clear_node(n);
    }
    delete node;
}

vector<pair<int,int>> find_frontier(multimap<int, vector<pair<int,int>>> &state){
    vector<pair<int,int>> frontier;
    for (auto entry: state){
        if (entry.first < entry.second.size() && entry.first > 0){         //if a move cannot be determined (ex. 1 = {(1,1), (2,1), (3,1)})
            for (auto p: entry.second){
                if (GRID_GIVEN[p.first][p.second] == UNREVEALED){   //if not already solved
                    frontier.push_back(make_pair(p.first,p.second));
                }
            }
        }
    }
    sort(frontier.begin(), frontier.end());
    auto last_unique = unique(frontier.begin(), frontier.end());
    frontier.erase(last_unique, frontier.end());

    return frontier;
}

bool is_valid(multimap<int, vector<pair<int,int>>> state){
    for (auto entry: state){
        if (entry.first > entry.second.size()){     //when there are mines than cells, invalid (ex. 3 = {(1,1), (2,1)})
            return false;
        }
    }
    return true;
}

//when given a pos on the frontier, search to find probability of mine by expanding by nodes to n depth
void tree_sim(pair<int,int> cords, int &valid_mine, int &valid_safe, int depth){

    for (int assume_mine = 0; assume_mine <=1; assume_mine++){
        //stores all the states to explore. Min heap by layer
        priority_queue<Node*, vector<Node*>, NodeCmp> cell_states;

        Node* root = create_node(CELLS);
        root->layer =0;
        root -> pos = cords;
        root -> mine = (assume_mine ==0) ? true: false;
        cell_states.push(root);

        while (!cell_states.empty()){
            Node* curr_node = cell_states.top();
            cell_states.pop();

            //gets the current state
            multimap<int, vector<pair<int,int>>> curr_state = curr_node->state;
            pair<int,int> curr_pos = curr_node->pos;
            bool is_mine = curr_node->mine;

            if (curr_node->layer > depth){
                continue;
            }

            //changes the curr_state to what happens when we assume the pos is a mine or safe
            if (is_mine) update_mine(curr_pos, curr_state, true);
            else update_move({curr_pos}, curr_state);

            while (!no_moves_left(curr_state)){
                find_moves(curr_state, true);
            }

            if (!is_valid(curr_state)){
                continue;      
            }

            //printCells(curr_state);
            vector<pair<int,int>> frontier = find_frontier(curr_state);

            if (frontier.empty()){
                if (is_mine) valid_mine++;
                else valid_safe++;
                continue;
            }

            for (auto &p: frontier){
                for (int k=0; k<=1; k++){
                    Node* n = create_node(curr_state);
                    n->layer = curr_node->layer +1;
                    n->pos = p;
                    n->mine = (k==0) ? true: false;

                    cell_states.push(n);
                    curr_node->next.push_back(n);
                }
            }
        }

        clear_node(root);
        root = nullptr;
    }
}

void monte_carlo_sim(multimap<int, vector<pair<int,int>>> state,
                     pair<int,int> pos,
                     bool assume_mine,
                     int &valid_mine,
                     int &valid_safe,
                     int k)
{
    k--;
    if (k <= 0) return;

    if (assume_mine) update_mine(pos, state, true);
    else update_move({pos}, state);

    // Expand deterministically until no forced moves left
    while (!no_moves_left(state)) {
        find_moves(state, true);
    }

    if (!is_valid(state)) return;

    vector<pair<int,int>> frontier = find_frontier(state);

    // Terminal condition: no frontier left
    if (frontier.empty()) {
        if (assume_mine) valid_mine++;
        else valid_safe++;
        return;
    }

    // Pick a random frontier cell to expand
    shuffle(frontier.begin(), frontier.end(), rng);
    pair<int,int> next = frontier[0];

    // Randomly assume mine or safe (50/50 choice)
    bool next_is_mine = (uniform_int_distribution<int>(0,1)(rng) == 1);
    monte_carlo_sim(state, next, next_is_mine, valid_mine, valid_safe, k);
}



void run_simulation(){
    vector<pair<int,int>> frontier = find_frontier(CELLS);
    PROB_MAP.clear();

    // Limit number of simulations based on size of frontier
    int num_runs = max(50, 3000 / (int)pow(log(max(2, (int)frontier.size())), 3));
    int depth = 15; 
    cout << "RUNNING: " << num_runs << " SIMULATIONS PER CELL.\n"<<endl;

    for (auto &p : frontier) {
        int valid_mine = 0;
        int valid_safe = 0;

        cout << "Simulating for (" << p.first << "," << p.second << ") ...";

        if (frontier.size() <= 12) {
            // Small frontier → do exact tree search
            int depth = 15;
            tree_sim(p, valid_mine, valid_safe, depth);
        } else {

            // Simulate mine/safe 
            uniform_int_distribution<int> dist(0, 1);

            for (int i = 0; i < num_runs; i++) {
                bool assume_mine = (dist(rng) == 1);
                monte_carlo_sim(CELLS, p, assume_mine, valid_mine, valid_safe, depth);
            }
        }

        double prob_mine = 0.0;
        if (valid_mine + valid_safe > 0) {
            prob_mine = (double)valid_mine / (valid_mine + valid_safe);
        }

        cout << "Probability: "<< prob_mine * 100 << "%" << endl;

        PROB_MAP[p] = prob_mine;
    }
}