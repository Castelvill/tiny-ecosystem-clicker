#include "gameState.hpp"

inline void update(GameState & gameState){
    gameState.update();
}

inline void draw(const GameState & gameState){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    gameState.draw();

    EndDrawing();
}

inline void spawnPlantsForTesting(GameState & gameState){
    Vector2 pos = {50, 250};
    Vector2 size = {50, 50};
    for(int y = 0; y < 2; ++y){
        for(int x = 0; x < 6; ++x){
            Vector2 diff = {
                x * 100.0f,
                y * 150.0f
            };
            gameState.aquariums.push_back(Aquarium(pos + diff, size));
            gameState.gameArea.spawnSeed(pos + diff + size/2);
        }
    }
}

inline void gameLoop(){
    GameState gameState;

    //spawnPlantsForTesting(gameState);

    //Exit the game loop if the user closed the game window or pressed the ESC key
    while(!WindowShouldClose()){
        update(gameState);
        draw(gameState);
    }
}

inline void setupRaylib(){
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tiny Ecosystem Clicker");
    SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    SetTargetFPS(60);
}

int main(){
    setupRaylib();

    gameLoop();

    CloseWindow();

    return 0;
}