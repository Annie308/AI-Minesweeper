#pragma once
#include "global.h"

void run_simulation();
std::vector<std::pair<int,int>> find_frontier(std::multimap<int, std::vector<std::pair<int,int>>> state);