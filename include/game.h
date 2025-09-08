#pragma once
#include "global.h"

extern std::map<std::pair<int,int>, int> pos_revealed;  

std::stack<std::pair<int,int>> find_empty_cells();
void make_move(int row, int col);
void initGame(int x, int y);