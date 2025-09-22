#include "global.h"
#include "solver.h"
#include "game.h"

using namespace std;
using namespace CELL_LABELS;

int count_mines(){
    int mines_count=0;
    for (auto vec: GRID_GIVEN){
        for (auto p: vec){
            if (p == MINE){
                mines_count ++;
            }
        }
    }
    return mines_count;
}

bool no_moves_left(multimap<int, vector<pair<int,int>>> state) {
    for (auto const &entry : state) {
        int count = entry.first;
        int size  = (int)entry.second.size();
        if (size == 0) continue;

        // Case 1: all cells are mines
        if (count == size) {
            for (auto const& p : entry.second) {
                if (find(mines_marked.begin(), mines_marked.end(), p) == mines_marked.end()) {
                    return false; 
                }
            }
        }

        // Case 2: all cells are safe
        if (count == 0) {
            for (auto const& p : entry.second) {
                if (pos_revealed.find(p) == pos_revealed.end()) {
                    return false; 
                }
            }
        }
    }
    return true; 
}


//find if a is a subset of b
bool is_subset(const vector<pair<int,int>>& a, const vector<pair<int,int>> &b){
    for (auto &p:a){
        if (find(b.begin(), b.end(), p) == b.end()) return false;
    }
    return true;
}

void printCells(multimap<int, vector<pair<int,int>>> const state){
    cout <<"Cells: \n";
    for (auto &entry: state){
        cout << entry.first <<": ";
        for (auto &p: entry.second){
            cout << "("<<p.first<<","<<p.second<<") ";
        }
        cout << endl;
    }
}

void printGridGiven(){
    cout <<"Grid Given: \n";
    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j< GRID_SIZE; j++){
            cout << GRID_GIVEN[i][j] <<" ";
        }
        cout << endl;
    }
}   