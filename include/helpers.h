#pragma once

#include "global.h"
#include "solver.h"

constexpr int DR[8] = {-1,-1,-1,0,0,1,1,1};     //offsets
constexpr int DC[8] = {-1, 0,1,-1,1,0,-1,1};

//make a template to pass the valid neighbours.
template <typename Func> 
void for_each_neighbour(int row, int col, Func func){
    for (int k = 0; k<8; k++){
        int nrow = row + DR[k];
        int ncol = col +DC[k];

        if (nrow >=0 && nrow < GRID_SIZE && 
            ncol >=0 && ncol < GRID_SIZE){
            func(nrow, ncol);       //run the function that the user calls with these values
        }
    }
}

void printCells(std::multimap<int, std::vector<std::pair<int,int>>> const state);
int count_mines();
bool no_moves_left(std::multimap<int, std::vector<std::pair<int,int>>> const state);
void clean_cells();
void printGridGiven();

