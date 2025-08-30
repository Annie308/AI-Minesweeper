#include "global.h"
#include "game.h"
#include "solver.h"

using namespace std;
using namespace CELLS_LABELS;

const int SCREEN_HEIGHT = 1200;
const int SCREEN_WIDTH = 1200;
const int TILE_SIZE = SCREEN_HEIGHT/GRID_SIZE;
bool RUNNING = true;
bool GAMEOVER = false;

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
            auto it = pos_revealed.find({i,j});
            if (it != pos_revealed.end()){          //display if it has been revealed
                string content = (GRID[i][j] ==EMPTY ) ? " ": to_string(GRID[i][j]);
                tiles[i*GRID_SIZE+j].setString(content);
                tiles[i*GRID_SIZE+j].reveal();
            }
            else if (it != pos_revealed.end()){
                tiles[i*GRID_SIZE+j].setString(" ");
            }
            if (grid_given[i][j] == EXPLODED){
                tiles[i*GRID_SIZE+j].explode();
            }
            auto pos = find(mines_marked.begin(), mines_marked.end(), make_pair(i,j));
            if (pos != mines_marked.end()){
                tiles[i*GRID_SIZE+j].flag();
            } 
        }
    }

}

void run(){
    sf::Font font;

    if (!font.openFromFile("C:\\Users\\annie\\Documents\\CppProjects\\aIMinesweeper\\Roboto.ttf")){
        cerr<< "Failed to load font\n";
        return;
    }

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT)), "window");
    window.setFramerateLimit(30);


    bool first_press = true;

     
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