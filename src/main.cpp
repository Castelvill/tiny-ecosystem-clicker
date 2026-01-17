#include "aquarium.hpp"

struct GameState{
    Aquarium gameArea;
    vector<Aquarium> aquariums;
    PlayerState player;
    UserInterface gui;
};

void update(GameState & gameState){
    gameState.gui.update(gameState.player);
    gameState.gameArea.updateGameArea(gameState.gui, gameState.aquariums);
    for(Aquarium & aquarium : gameState.aquariums)
        aquarium.update();
    gameState.gameArea.update();
}

void draw(const GameState & gameState){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    for(const Aquarium & aquarium : gameState.aquariums)
        aquarium.draw();
    gameState.gameArea.drawEntities();

    gameState.gui.draw(gameState.player);

    EndDrawing();
}

void mainLoop(){
    GameState gameState;
    gameState.aquariums.push_back(Aquarium({50, 200}, {100, 100}));
    gameState.aquariums.push_back(Aquarium({200, 250}, {100, 100}));
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