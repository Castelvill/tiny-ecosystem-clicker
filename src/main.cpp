#include "aquarium.hpp"
#include "shelf.hpp"

struct GameState{
    Aquarium gameArea;
    vector<Aquarium> aquariums;
    vector<Shelf> shelves;
    PlayerState player;
    UserInterface gui;
    void draw() const;
};

void GameState::draw() const {
    gui.drawBackground();

    for(const Aquarium & aquarium : aquariums)
        aquarium.draw();
    
    gameArea.drawEntities();

    for(const Shelf & shelf : shelves)
        shelf.draw();

    gui.draw(player);
}

void update(GameState & gameState){
    gameState.gui.update(gameState.player);
    gameState.gameArea.updateGameArea(gameState.gui, gameState.aquariums, gameState.player);
    for(Aquarium & aquarium : gameState.aquariums)
        aquarium.update();
    gameState.gameArea.update();
}

void draw(const GameState & gameState){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    gameState.draw();

    EndDrawing();
}

void mainLoop(){
    GameState gameState;
    gameState.shelves.push_back(Shelf({40, 300}, {120, 10}));
    gameState.aquariums.push_back(Aquarium({50, 200}, {100, 100}));

    gameState.shelves.push_back(Shelf({190, 350}, {120, 10}));
    gameState.aquariums.push_back(Aquarium({200, 250}, {100, 100}));

    gameState.shelves.push_back(Shelf({340, 400}, {120, 10}));
    gameState.aquariums.push_back(Aquarium({350, 300}, {100, 100}));
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update(gameState);
        draw(gameState);
    }
}

int main(){
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tiny Ecosystem Clicker");
    SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    SetTargetFPS(60);

    mainLoop();

    CloseWindow();

    return 0;
}