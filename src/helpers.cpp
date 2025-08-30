#include "global.h"
#include "solver.h"

using namespace std;
using namespace CELLS_LABELS;

int count_mines(){
    int mines_count=0;
    for (auto vec: grid_given){
        for (auto p: vec){
            if (p == MINE){
                mines_count ++;
            }
        }
    }
    return mines_count;
}

bool no_moves_left(){
    for (auto const &entry: cells){
        int count = entry.first;
        int size  = (int)entry.second.size(); 
        if (count == size){
            for (auto &p: entry.second){
                auto it = find(mines_marked.begin(), mines_marked.end(), make_pair(p.first, p.second));
                if (it == mines_marked.end()){   ///if there are mines to be marked
                    return false;
                }
            }
        }
        if (count ==0 && !entry.second.empty()){
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
    for (auto it = cells.begin(); it != cells.end(); ){
        if (it->first ==0 && it->second.empty()){
            it = cells.erase(it);
        }
        else if (it->first == (int)it->second.size()){
            it = cells.erase(it);
        }
        else{
            ++it;
        }
    }
}

void printCells(){
    cout <<"Cells: \n";
    for (auto &entry: cells){
        cout << entry.first <<": ";
        for (auto &p: entry.second){
            cout << "("<<p.first<<","<<p.second<<") ";
        }
        cout << endl;
    }
}