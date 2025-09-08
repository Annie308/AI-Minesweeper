#include "global.h"
#include "solver.h"

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

bool no_moves_left(multimap<int, vector<pair<int,int>>> const state){
    for (auto const &entry: state){
        int count = entry.first;
        int size  = (int)entry.second.size(); 
        if (count == size && size >0){
            for (auto p: entry.second){
                auto it = find(mines_marked.begin(), mines_marked.end(), p);
                if (it == mines_marked.end()){      //there are still moves if you didn't mark it yet
                    return false;
                }
            }
        }

        if (count ==0 && size > 0){
            return false;
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

void clean_cells(){
    for (auto it = CELLS.begin(); it != CELLS.end(); ){
        //if count = 0 and no more moves, erase
        if (it->first ==0 && it->second.empty()){
            it = CELLS.erase(it);
        }
        else{
            ++it;
        }
    }
    for (auto &entry: CELLS){
        auto &vec = entry.second;
        for (auto &mv: mines_marked){
            auto it = find(vec.begin(), vec.end(), mv);
            
            if (it != vec.end()){
                vec.erase(remove(vec.begin(), vec.end(), mv), vec.end());
            }
        }
    }
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