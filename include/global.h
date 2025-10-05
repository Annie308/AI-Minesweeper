#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <unordered_map>
#include <map>
#include <stack>
#include <SFML/Graphics.hpp>
#include <functional>

namespace CELL_LABELS{
    const int UNREVEALED = -2;
    const int MINE = -1;
    const int EXPLODED = -3;
    const int EMPTY = 0;

    inline bool is_marker(int label){
        return label > 0;
    }
}

extern int GRID_SIZE;
extern std::vector<std::vector<int>> GRID;
extern std::vector<std::vector<int>> GRID_GIVEN;
extern std::vector<std::vector<double>> PROB_GRID; 
extern int MINESNUM;
extern bool RUNNING;
extern int MOVES;
extern std::multimap<int, std::vector<std::pair<int,int>>> CELLS;  
extern bool GAMEOVER;
extern std::mt19937 rng;
extern std::map<std::pair<int,int>, double> PROB_MAP;