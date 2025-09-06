#pragma once


extern std::vector<std::vector<int>> grid_given;
extern std::vector<std::pair<int,int>> mines_marked;
void actions(int row, int col);
void solve();
void printGridGiven();
void initialize();
bool is_subset(const std::vector<std::pair<int,int>>& a, const std::vector<std::pair<int,int>> &b);
void update_move(std::vector<std::pair<int,int>> to_remove, std::multimap<int, std::vector<std::pair<int,int>>> &state);
void mark_and_update_mine(std::pair<int,int> pos, std::multimap<int, std::vector<std::pair<int,int>>> &state, bool simulate);
void find_moves(std::multimap<int, std::vector<std::pair<int,int>>> &state, bool simulate);
void find_actions();