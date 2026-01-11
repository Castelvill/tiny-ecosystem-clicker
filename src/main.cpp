#include "simulation.hpp"

struct GameState{
    Simulation simulation;
    PlayerState player;
    UserInterface gui;
};

void update(GameState & gameState){
    gameState.gui.update(gameState.player);
    gameState.simulation.update(gameState.gui);
}

void draw(const GameState & gameState){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    gameState.simulation.draw();
    gameState.gui.draw(gameState.player);

    EndDrawing();
}

void mainLoop(){
    GameState gameState;
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update(gameState);
        draw(gameState);
    }
}

int main(){
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tiny Ecosystem Clicker");
    SetTargetFPS(60);

    mainLoop();

    CloseWindow();

    return 0;
}