#pragma once


extern std::vector<std::vector<int>> grid_given;
extern std::vector<std::pair<int,int>> mines_marked;
void actions(int row, int col);
void solve();
void printGridGiven();
void initialize();
bool is_subset(const std::vector<std::pair<int,int>>& a, const std::vector<std::pair<int,int>> &b);