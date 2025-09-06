#include "global.h"
#include "helpers.h"
#include "solver.h"

using namespace std;
using namespace CELLS_LABELS;

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
        return a->layer < b->layer;             //a has more priority if it is smaller
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
                auto it = find(mines_marked.begin(), mines_marked.end(), make_pair(p.first, p.second));
                if (it == mines_marked.end() && grid_given[p.first][p.second] != MINE){   //if not already marked
                    frontier.push_back(make_pair(p.first,p.second));
                }
            }
        }
    }
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

bool game_over(multimap<int, vector<pair<int,int>>> state){
    int mines_found =0;
    for (auto entry: state){
        if (entry.first ==0 && entry.second.empty()) continue;          //if empty, continue
        if (entry.first ==0 && !entry.second.empty()) return false;   //if there are still safe moves, not over

        if (entry.first == entry.second.size() && entry.first >0){     //if all the cells are mines
            for (auto &p: entry.second){
                auto it = find(mines_marked.begin(), mines_marked.end(), make_pair(p.first, p.second));
                if (it == mines_marked.end()){   ///if not already marked
                    mines_found += entry.first;
                }
            }
        }
    }
    //if (mines_found + count_mines() < MINESNUM) return false;   //if not all mines found
    return true;
}

//when given a pos on the frontier, search to find probability of mine by expanding
void simulate(pair<int,int> cords){
    //stores all the nodes for branching. Min heap using BFS
    int valid_mine =0;
    int valid_safe =0;

    for (int assume_mine = 0; assume_mine <=1; assume_mine++){
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

            if (curr_node->layer > 3){
                //cout <<"Depth reached: "<<curr_node->layer<<" . Aborting..."<<endl;
                continue;
            }

            //changes the curr_state to what happens when we assume the pos is a mine or safe
            if (is_mine) mark_and_update_mine(curr_pos, curr_state, true);
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
                    //cout <<"Expanding to ("<<p.first<<","<<p.second<<") at layer "<<n->layer<<endl;
                }
            }

        }

        clear_node(root);
        root = nullptr;
    }
    int total = valid_mine + valid_safe;
    double prob = (double)valid_mine / total;

    cout <<"valid mine: "<<valid_mine<<", valid safe: " << valid_safe<<"total count: "<<total<<", prob: "<<prob<<endl;
}

void run_simulation(){
    vector<pair<int,int>> frontier = find_frontier(CELLS);
    for (auto &p: frontier){
        cout <<"Simulating for ("<<p.first<<","<<p.second<<")"<<endl;
        cout <<grid_given[p.first][p.second]<<endl;
        simulate(p);

        /* map<double, pair<int,int>> likely_mines = simulate(p);

        for (auto it = likely_mines.rbegin(); it != likely_mines.rend(); ++it) {
            // iterates largest key → smallest key
            cout << "Probability of (" << it->second.first << "," << it->second.second << ") being a mine: " << it->first * 100 << "%" << endl;
        }

        pair<int,int> best_guess = likely_mines.rbegin()->second; 
        //mark_and_update_mine(best_guess, CELLS, false); */
    }
}