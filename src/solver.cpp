#include "game.h"
#include "global.h"
#include "helpers.h"
#include "render.h"

using namespace std;
using namespace CELLS_LABELS;

//when given an empty minesweeper grid and the revealed positions after the initial blast, the ai should be able to solve
//after every safe move, upudate the grid and also add new revealed markers to pos_revealed

multimap<int, vector<pair<int,int>>> cells;  
map<pair<int,int>, bool> safe;
vector<vector<int>> grid_given(GRID_SIZE, vector<int>(GRID_SIZE, UNREVEALED));
vector<pair<int,int>> mine_cords;
vector<pair<int,int>> mines_marked;
bool move_made = false;

void initialize(){
    for (auto &p: pos_revealed){
        grid_given[p.first.first][p.first.second] = p.second;
    }
}

void actions(int row, int col){     //given a marker position, return all possible actions
    vector<pair<int,int>> acts;

    for_each_neighbour(row, col, [&](int nr, int nc){
        auto it = pos_revealed.find({nr, nc});          //if its neighbours havent been revealed
        if (it == pos_revealed.end() && grid_given[nr][nc] != EMPTY && grid_given[nr][nc]){
            acts.push_back({nr, nc});
        }                           
    });
    //gets all the cells that surround the marker
    if (!acts.empty()) cells.insert({grid_given[row][col], acts});
}

void mark_and_update_mine(int row, int col){
    grid_given[row][col] = MINE;          //mark as mine
    mines_marked.push_back({row,col});
    vector<pair<int, vector<pair<int,int>>>> to_add;

    //find every instance of the mine erase the move
    //since row, col is a mine, we must also decrease the mine count
    for (auto itr = cells.begin(); itr != cells.end();){
        auto &vec = itr->second;
        auto pos = find(vec.begin(), vec.end(), make_pair(row,col));
        if (pos != vec.end()){
            auto new_vec = vec;
            new_vec.erase(remove(new_vec.begin(), new_vec.end(), make_pair(row,col)), new_vec.end());
            int new_key = itr->first-1;
            to_add.push_back({new_key, new_vec});
            itr = cells.erase(itr);
        }
        else{
            ++itr;  //continue the loop
        }
    }
    for (auto vec: to_add){
        if (!vec.second.empty()){
        cells.emplace(vec);
        }
    }
}

void update_move(vector<pair<int,int>> to_remove){
    if (!to_remove.empty()){            //remove the move i just made
        for (auto &entry: cells){
            auto &vec = entry.second;
            for (auto &mv: to_remove){
                vec.erase(
                    remove(vec.begin(), vec.end(), mv), vec.end()
                );
        }
        }
    }
}
//finds a safe move to move
void find_moves(){
    vector<pair<int,int>> to_remove;
    vector<pair<int,int>> mine_pos;
    move_made = false;

    auto range = cells.equal_range(0);
    for (auto itr = cells.begin(); itr!= cells.end(); itr++){
        for (auto &p: itr->second){
            auto [row,col] = p;
            if (itr->first == (int)itr->second.size()){
                mine_pos.push_back({row, col}); 
            }
            if (itr->first==0){
                make_move(row, col);
                to_remove.push_back({row, col});
                move_made = true;
            }
        }
    }

    update_move(to_remove);

    for (auto &p: mine_pos){
        mark_and_update_mine(p.first, p.second);
    }
}

void make_random_move(){
    vector<pair<int,int>> avail_moves;
    vector<pair<int, int>> to_remove;

    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j< GRID_SIZE; j++){
            if (grid_given[i][j] == UNREVEALED){
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
    
    for (auto it = cells.begin(); it!= cells.end(); it++){
        cpy_cells.push(make_pair(it->first, it->second));
    }

    while (!cpy_cells.empty()){
        vector<pair<int,int>> a = cpy_cells.top().second;
        int a_count = cpy_cells.top().first; 

        cpy_cells.pop();

        for (auto it = cells.begin(); it!= cells.end(); ){
            int b_count = it->first;
            vector<pair<int,int>> b = it->second;
            vector<pair<int,int>> diff;
            int new_count = b_count - a_count;

            if (is_subset(a,b) && it->second.size() != a.size() && it->first > 0){
                for (auto &p: b){  //push back what you cant find
                    if (find(a.begin(), a.end(), p) ==a.end()){
                        diff.push_back(p);
                    }
                }

                if (!diff.empty()){to_add.emplace_back(make_pair(new_count, diff)); }

                it = cells.erase(it);
            }else{
                ++it;
            }   
        }
    }
    //add all the new entries
    for (auto &s: to_add){
        cells.emplace(s.first, move(s.second));
    }
}

void solve(){
    if (count_mines() < MINESNUM){
        //finds all possible actions when given the revealed positions
        for (auto &p: pos_revealed){
            actions(p.first.first, p.first.second);
        }
  
        find_moves();

        if (!move_made) apply_logic();

        if (no_moves_left()){
            cout <<"No more moves, making random move..."<<endl;
            make_random_move();
        }
        clean_cells();
    }
    for (auto &p: pos_revealed){
        grid_given[p.first.first][p.first.second] = GRID[p.first.first][p.first.second];
    }

    if (count_mines() == MINESNUM){
        RUNNING = false;
        GAMEOVER = true;
        cout <<"Solved in "<<MOVES<<" moves! Found all "<<MINESNUM<<" mines.";    
    }
}

//if count == label and all the contents are mines
//flag each mine and then remove the cords from every entry
