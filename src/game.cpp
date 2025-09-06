#include "global.h"
#include "helpers.h"
#include "game.h"

using namespace std;
using namespace CELLS_LABELS;

const int GRID_SIZE = 9;
int MINESNUM = 20;
int MOVES =0;

vector<vector<int>> GRID(GRID_SIZE, vector<int>(GRID_SIZE, UNREVEALED));         //-2 = empty, -1 = mines, otherwise (0-5) markers for mines
deque<pair<int,int>> blast_pos;
map<pair<int,int>, int> pos_revealed;                        //positions that the ai can see
uniform_int_distribution<int> dist(0,GRID_SIZE-1); 

//when given blasting centre, blast using bfs
void blast(int row, int col){
    blast_pos.push_back({row, col});
    uniform_int_distribution<int> dist(GRID_SIZE*2, GRID_SIZE*3);  //randomise blast count and limit blast area
    int blast_count = dist(rng);                                                 

    while (!blast_pos.empty() && blast_count >0){
        pair<int,int> fill_cords = blast_pos.front(); blast_pos.pop_front();
        row = fill_cords.first; col = fill_cords.second;
        
        if (GRID[row][col] == UNREVEALED) {
            GRID[row][col] = EMPTY;                            //if not relvealed yet mark it blasted
            blast_count--;
        
        //call the template to find valid neighours. If the neighbour is valid, the template will call the lamdba function
        for_each_neighbour(row, col, [&](int nr, int nc){
            blast_pos.push_back({nr, nc});
        });
        }
    }
}

void place_mines(){      
    int mines_placed =0;

    while (mines_placed < MINESNUM){
        int rand_row = dist(rng);
        int rand_col = dist(rng);
        if (GRID[rand_row][rand_col] == UNREVEALED){
            GRID[rand_row][rand_col] = MINE;
            mines_placed ++;
        }
    }
    //label mines
    for (int row =0; row <GRID_SIZE; row++){
        for (int col=0; col <GRID_SIZE; col++){
            if (GRID[row][col] ==MINE){
                for_each_neighbour(row, col, [&](int nr, int nc){
                if (GRID[nr][nc] == UNREVEALED || GRID[nr][nc] ==EMPTY) GRID[nr][nc] = 1;                   //if not checked yet its 1 mine
                else if (GRID[nr][nc] >= 1) GRID[nr][nc] ++; 
            });
        }
        }
    }
}

void cleanUp(){
    stack<pair<int,int>> empty_cords = find_empty_cells();
    while (!empty_cords.empty()){
        auto [row, col] = empty_cords.top(); empty_cords.pop();
        GRID[row][col] = EMPTY;
                    
        for_each_neighbour(row, col, [&](int nr, int nc){
            if (GRID[nr][nc] == UNREVEALED){
                empty_cords.push({nr, nc}); 
            } 
        });   
    }
}

void find_visible_cells(){
    for (int row=0; row< GRID_SIZE; row++){
        for (int col=0; col< GRID_SIZE; col++){
            if (GRID[row][col] == EMPTY){
                pos_revealed[{row,col}] = EMPTY;
                for_each_neighbour(row, col, [&](int nr, int nc){
                pos_revealed[{nr, nc}] = GRID[nr][nc];
            });
            }
        }
    }
    
}

stack<pair<int,int>> find_empty_cells(){
    stack<pair<int,int>> empty_cords;     //stores the empty cords for cleanup
    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j< GRID_SIZE; j++){
            if (GRID[i][j] == EMPTY){
                empty_cords.push({i,j});
            }
        }
    }
    return empty_cords;
}

void initGame(int x, int y){
    blast(x,y);
    place_mines();
    cleanUp();
    find_visible_cells();
    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j< GRID_SIZE; j++){
            cout << GRID[i][j] <<" ";
        }
        cout << endl;
    }
}

//allows the ai to make a move and update the grid
void make_move(int row, int col){
    if (row >=0 &&row <GRID_SIZE && col >=0 && col < GRID_SIZE){
        if (GRID[row][col] == MINE){
            cout <<"GAME OVER! YOU LOST! ";
            grid_given[row][col] = EXPLODED;
            RUNNING = false;
            GAMEOVER = true;
        }
        else{
            if(GRID[row][col] == UNREVEALED) {
                GRID[row][col] =EMPTY;
                //blast neighbours if empty
                cleanUp();
                find_visible_cells();
            }   
            pos_revealed[{row,col}] = GRID[row][col];
            grid_given[row][col] = GRID[row][col];
        }
    }
    MOVES++;
    return;
}

