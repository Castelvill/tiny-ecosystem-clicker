#include "simulation.hpp"

void update(Simulation & simulation){
    simulation.update();
}

void draw(const Simulation & simulation){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    simulation.draw();

    DrawText("Press 1 to spawn algae\nPress 2 to spawn sand\nPress 3 to spawn ostracod", 5, 5, 20, BLACK);

    EndDrawing();
}

void mainLoop(){
    Simulation simulation;
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update(simulation);
        draw(simulation);
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