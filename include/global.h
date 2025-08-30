#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <unordered_map>
#include <map>
#include <stack>
#include <SFML/Graphics.hpp>

namespace CELLS_LABELS{
    const int UNREVEALED = -2;
    const int MINE = -1;
    const int EXPLODED = -3;
    const int EMPTY = 0;
}

extern std::vector<std::vector<int>> GRID;
extern const int GRID_SIZE;
extern int MINESNUM;
extern bool RUNNING;
extern int MOVES;
extern std::multimap<int, std::vector<std::pair<int,int>>> cells;  
extern bool GAMEOVER;
extern std::mt19937 rng;