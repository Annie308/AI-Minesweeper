#include "global.h"
#include "game.h"
#include "solver.h"

using namespace std;
using namespace CELL_LABELS;

const int SCREEN_HEIGHT = 1200;
const int SCREEN_WIDTH = 1200;
bool RUNNING = false;
bool GAMEOVER = false;

//default values
int GRID_SIZE=9;
int MINESNUM=10;
int TILE_SIZE = SCREEN_HEIGHT/GRID_SIZE;

class Tile{
private:
    sf::RectangleShape shape;
    sf::Text text;

public:
    Tile(int row, int col, sf::Font &font): text(font){
        shape.setFillColor(sf::Color(72,72,72));
        shape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        shape.setOutlineColor(sf::Color(44,44,44));
        shape.setOutlineThickness(5);
        shape.setPosition(sf::Vector2f(col*TILE_SIZE, row*TILE_SIZE));

        text.setCharacterSize(40);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(col*TILE_SIZE, row*TILE_SIZE));
    }

    void reveal(){
        shape.setFillColor(sf::Color(107,107,107));
    }

    void flag(){
        shape.setFillColor(sf::Color::Red);
        text.setString(" ");
    }

    void explode(){
        text.setString("!");
        text.setFillColor(sf::Color::White);
        shape.setFillColor(sf::Color::Black);
    }

    void setString(string c){
        text.setString(c);
    }

    void draw(sf::RenderWindow &window){
        window.draw(shape);
        window.draw(text);
    }
};

class Gameover{
private:
    sf::Text text;
public:
    Gameover(sf::Font& font) : text(font){
        string msg = "GAMEOVER!";
        text.setString(msg);
        text.setCharacterSize(70);
        
        text.setOrigin(text.getGlobalBounds().getCenter());
        text.setPosition(sf::Vector2f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2));
    }

    void draw(sf::RenderWindow &window2){
        window2.draw(text);
    }
};
vector<Tile> tiles;

void render_grid(){
    for (int i=0; i< GRID_SIZE; i++){
        for (int j=0; j<GRID_SIZE; j++){
            if (GRID_GIVEN[i][j] == EMPTY){
                tiles[i*GRID_SIZE+j].setString("");
                tiles[i*GRID_SIZE+j].reveal();
            }
            if (is_marker(GRID_GIVEN[i][j])){
                tiles[i*GRID_SIZE+j].reveal();
                string content = to_string(GRID_GIVEN[i][j]);
                tiles[i*GRID_SIZE+j].setString(content);
            }
            auto it = PROB_MAP.find({i,j});
            if (it != PROB_MAP.end()){
                string content = to_string((int)(PROB_MAP[{i,j}]*100)) + "%";
                tiles[i*GRID_SIZE+j].setString(content);
            }

            if (GRID_GIVEN[i][j] == EXPLODED) tiles[i*GRID_SIZE+j].explode();
            if (GRID_GIVEN[i][j] == MINE) tiles[i*GRID_SIZE+j].flag();

   
        }
    }

}

void run(){
    sf::Font font;

    if (!font.openFromFile("C:\\Users\\annie\\Documents\\CppProjects\\aIMinesweeper\\Roboto.ttf")){
        cerr<< "Failed to load font\n";
        return;
    }
    /* ================= GETTING USER INPUT =============*/

    bool first_press = true;
    int grid_size;
    int mines_num;

    cout <<" ================ WELCOME TO MINESWEEPER ================ \n\n";
    cout <<" | The game will start after you enter the grid size and number of mines. |\n\n";
    cout << "| The AI will attempt to solve the game! |\n";

    do{
        cout <<"Enter grid size (min 5, max 30): ";
        cin >> grid_size;

    }while(!grid_size || grid_size <5 || grid_size >30);

    GRID_SIZE = grid_size;
    TILE_SIZE = SCREEN_HEIGHT/GRID_SIZE;
    
    // Reinitialize grids with new size
    GRID.clear();
    GRID_GIVEN.clear();
    GRID.resize(GRID_SIZE, vector<int>(GRID_SIZE, UNREVEALED));
    GRID_GIVEN.resize(GRID_SIZE, vector<int>(GRID_SIZE, UNREVEALED));

    do{
        cout <<"Enter number of mines (min 1, max "<< GRID_SIZE*GRID_SIZE -9 <<"): ";
        cin >> mines_num;
    }while(!mines_num || mines_num <1 || mines_num > GRID_SIZE*GRID_SIZE -9);

    MINESNUM = mines_num;

    TILE_SIZE = SCREEN_HEIGHT/GRID_SIZE;
   
    cout <<"\nGrid size: "<<GRID_SIZE<<"x"<<GRID_SIZE<<", Mines: "<<MINESNUM<<" Tile size: "<<TILE_SIZE<<endl;

    cout <<"\nGame started! Click on a tile to make your first move.\n";

    RUNNING = true;


    /* ============= STARTING GAME ==================*/

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT)), "window");
    window.setFramerateLimit(30);
     
    for (int row=0; row<GRID.size(); row++){
        for (int col=0; col<GRID[0].size(); col++){
            tiles.emplace_back(row,col, font);
        }
    }
    while (RUNNING){
        while (auto event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                RUNNING = false;
            }

            if (event->is<sf::Event::MouseButtonPressed>()){
                auto [x ,y] = sf::Mouse::getPosition(window);
                x /=TILE_SIZE; y /=TILE_SIZE;
                
                if (first_press){
                    initGame(x,y);
                    initialize();
                }
                
                if (!first_press) solve();

                render_grid();
                first_press = false;
            }
        }

    window.clear();
    for (auto &tile: tiles){
        tile.draw(window);
    }
    window.display();
    }

    sf::RenderWindow window2(sf::VideoMode(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT)),"Game over");
    window2.setFramerateLimit(30);
    Gameover gameover(font);

    while (GAMEOVER){
        while (auto event = window2.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                GAMEOVER = false;
            }
        }
        render_grid();

        window2.clear();
        for (auto &tile: tiles){
            tile.draw(window2);
        }
        gameover.draw(window2);
        window2.display();
    }
}