#include "global.h"
#include "helpers.h"
#include "solver.h"

using namespace std;
using namespace CELL_LABELS;

vector<vector<double>> PROB_GRID(GRID_SIZE, vector<double>(GRID_SIZE, -1));  //stores the probability of each cell being a mine

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

vector<pair<int,int>> find_frontier(multimap<int, vector<pair<int,int>>> state){
    vector<pair<int,int>> frontier;
    for (auto entry: state){
        if (entry.first < entry.second.size() && entry.first > 0){         //if we can't determine a move (ex. 1 = {(1,1), (2,1), (3,1)})
            for (auto p: entry.second){
                if (GRID_GIVEN[p.first][p.second] != MINE){   //if not already marked
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

//when given a pos on the frontier, search to find probability of mine by expanding
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


void monte_carlo_sim(multimap<int, vector<pair<int,int>>> state, pair<int,int> pos, bool assume_mine, int &valid_mine, int &valid_safe, int k){ 
    k--;
    if (k <= 0) return;

    if (assume_mine) update_mine(pos, state, true);
    else update_move({pos}, state);

    while (!no_moves_left(state)){
        find_moves(state, true);
    }

    if (!is_valid(state)) return;

    vector<pair<int,int>> frontier = find_frontier(state);

    if (frontier.empty()){
        if (assume_mine) valid_mine++;
        else valid_safe++;
        return;
    }
    else{
        shuffle(frontier.begin(), frontier.end(), rng); 
        for (int i=0; i< min(1, (int)frontier.size()); i++){     //expand only 2 random cells
            monte_carlo_sim(state, frontier[i], true, valid_mine, valid_safe,k);
            monte_carlo_sim(state, frontier[i], false, valid_mine, valid_safe,k);
        }
    }
}


void run_simulation(){
    vector<pair<int,int>> frontier = find_frontier(CELLS);
    for (auto &p: frontier){
        int valid_mine =0;
        int valid_safe =0;

        cout <<"Simulating for ("<<p.first<<","<<p.second<<")"<<endl;

        if (frontier.size() <=12) {
            int depth = 15;
            tree_sim(p, valid_mine, valid_safe, depth);
        }else {
            int k = 5000/(log(frontier.size())*log(frontier.size()));       //number of simulations depends on frontier size
            cout <<"Num of simulations: "<<k<<endl;
            monte_carlo_sim(CELLS, p, true, valid_mine, valid_safe, k);
            monte_carlo_sim(CELLS, p, false, valid_mine, valid_safe, k);
        }

        double prob_mine = (double)valid_mine / (valid_mine + valid_safe);
        cout <<"Probability of ("<<p.first<<","<<p.second<<") being a mine: "<< prob_mine * 100 << "%" <<endl;
        PROB_GRID[p.first][p.second] = prob_mine;
    }
}