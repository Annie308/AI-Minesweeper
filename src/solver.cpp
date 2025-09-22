#include "game.h"
#include "global.h"
#include "helpers.h"
#include "render.h"
#include "simulation.h"

using namespace std;
using namespace CELL_LABELS;

//when given an empty minesweeper grid and the revealed positions after the initial blast, the ai should be able to solve
//after every safe move, upudate the grid and also add new revealed markers to pos_revealed

multimap<int, vector<pair<int,int>>> CELLS;  
map<pair<int,int>, bool> safe;
vector<pair<int,int>> mine_cords;
vector<pair<int,int>> mines_marked;
bool move_made = false;

void initialize(){
    for (auto &p: pos_revealed){
        auto [row,col] = p.first;
        GRID_GIVEN[row][col] = p.second;
    }
    find_actions();
}

void find_actions(){  
    for (int row = 0; row < GRID_SIZE; row++){
        for (int col = 0; col < GRID_SIZE; col++){
            if (GRID_GIVEN[row][col] <= 0) continue;   //if its not a marker continue
            vector<pair<int,int>> acts;

            for_each_neighbour(row, col, [&](int nr, int nc){
                if (GRID_GIVEN[nr][nc] == UNREVEALED || GRID_GIVEN[nr][nc] == MINE){
                    acts.push_back({nr, nc});
                }                           
            });
            //gets all the cells that surround the marker
            if (!acts.empty()) CELLS.insert({GRID_GIVEN[row][col], acts});
        }
    }
}

void update_mine(pair<int,int> pos, multimap<int, vector<pair<int,int>>> &state, bool simulate){
    auto [row,col] = pos;

    if (!simulate) {
        GRID_GIVEN[row][col] = MINE;
        mines_marked.push_back({row,col});
    }

    multimap<int, vector<pair<int,int>>> new_state;

    for (auto &entry : state) {
        int clue = entry.first;
        auto cells = entry.second;

        // remove this mine from the cell list if present
        auto it = find(cells.begin(), cells.end(), pos);
        if (it != cells.end()) {
            cells.erase(it);
            clue = max(0, clue - 1); // decrement clue count
        }

        if (!cells.empty()) {
            new_state.emplace(clue, move(cells));
        }
    }

    state.swap(new_state); // replace with cleaned state
}


void update_move(vector<pair<int,int>> to_remove, multimap<int, vector<pair<int,int>>> &state){
    for (auto &entry: state){
        auto &vec = entry.second;
        for (auto &mv: to_remove){
            vec.erase(
                remove(vec.begin(), vec.end(), mv), vec.end()
            );
    }
    }
}
//finds a safe move to move
void find_moves(multimap<int, vector<pair<int,int>>> &state, bool simulate){
    vector<pair<int,int>> to_remove;
    vector<pair<int,int>> mine_pos;
    move_made = false;

    auto range = state.equal_range(0);
    for (auto itr = state.begin(); itr!= state.end(); itr++){
        for (auto &p: itr->second){
            auto [row,col] = p;
            if (itr->first == (int)itr->second.size()){
                mine_pos.push_back({row, col}); 
            }
        }
    }

    for (auto &p: mine_pos){
        if (simulate) update_mine(p, state, true);
        else update_mine(p, state, false);
    }

    for (auto itr = state.begin(); itr != state.end(); itr++) {
        if (itr->first == 0) {
            for (auto &p: itr->second) {
                if (!simulate) make_move(p.first, p.second);
                to_remove.push_back(p);
                move_made = true;
            }
        }
    }
    
    update_move(to_remove, state);
}

void apply_logic(){
    vector<pair<int, vector<pair<int,int>>>> to_add;
    stack<pair<int, vector<pair<int,int>>>> cpy_cells;
    
    for (auto it = CELLS.begin(); it!= CELLS.end(); it++){
        cpy_cells.push(make_pair(it->first, it->second));
    }

    while (!cpy_cells.empty()){
        vector<pair<int,int>> a = cpy_cells.top().second;
        int a_count = cpy_cells.top().first; 

        cpy_cells.pop();

        for (auto it = CELLS.begin(); it!= CELLS.end();){
            int b_count = it->first;

            vector<pair<int,int>> b = it->second;
            vector<pair<int,int>> diff;
            int new_count = b_count - a_count;

            if (is_subset(a,b) && it->second.size() != a.size() && new_count >=0){   
                for (auto &p: b){  //push back what you cant find
                    if (find(a.begin(), a.end(), p) ==a.end()){
                        diff.push_back(p);
                    }
                }

                if (!diff.empty()){to_add.emplace_back(make_pair(new_count, diff)); }

                it = CELLS.erase(it);
            }else{
                ++it;
            }   
        }
    }
    //add all the new entries
    for (auto &s: to_add){
        CELLS.emplace(s.first, move(s.second));
    }
}

void solve(){
    if (count_mines() < MINESNUM){
        //finds all possible actions when given the revealed positions

        printGridGiven();
        apply_logic();
        find_moves(CELLS, false);
        find_actions();                 //as more cells are revealed we update our knowledge
        apply_logic();
        
        if (no_moves_left(CELLS)){
            cout <<"no more moves! Simulating..."<<endl;
            run_simulation();
        } 
        cout <<"Mines found: "<<count_mines()<<endl;
    }

    //check win condition
    if (count_mines() == MINESNUM){
        RUNNING = false;
        GAMEOVER = true;
        cout <<"Solved in "<<MOVES<<" moves! Found all "<<MINESNUM<<" mines.";    
    }
}

//if count == label and all the contents are mines
//flag each mine and then remove the cords from every entry
