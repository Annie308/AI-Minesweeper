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

    vector<pair<int, vector<pair<int,int>>>> to_add;

    for (auto itr = state.begin(); itr != state.end(); ) {
        auto &vec = itr->second;
        auto it = find(vec.begin(), vec.end(), pos);

        if (it != vec.end()) {
            // make a fresh copy and erase the mine
            auto new_vec = vec;
            new_vec.erase(remove(new_vec.begin(), new_vec.end(), pos), new_vec.end());

            int new_key = max(0, itr->first - 1); // avoid negative keys
            itr = state.erase(itr);

            // only reinsert if non-empty
            if (!new_vec.empty()) {
                to_add.emplace_back(new_key, move(new_vec));
            }
        }
        else ++itr;
    }
    for (auto &vec : to_add) {
        state.emplace(vec);
    }
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

void make_random_move(){
    vector<pair<int,int>> avail_moves;
    vector<pair<int, int>> to_remove;

    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j< GRID_SIZE; j++){
            if (GRID_GIVEN[i][j] == UNREVEALED){
                avail_moves.push_back({i,j});
            }
        }
    }

    if (!avail_moves.empty()) {
        uniform_int_distribution<int> range(0, avail_moves.size()-1);       //finds random move in al available moves
        int x = range(rng);
        int row = avail_moves[x].first;
        int col = avail_moves[x].second;
        make_move(row, col);
        to_remove.push_back({row,col});
    }
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
        printCells(CELLS);
        apply_logic();
        find_moves(CELLS, false);
        find_actions();                 //as more cells are revealed we update our knowledge
        printCells(CELLS);

        if (no_moves_left(CELLS)){
            cout <<"no more moves! Simulating..."<<endl;
            run_simulation();
        }

        cout <<"mines found: "<<count_mines()<<endl;
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
