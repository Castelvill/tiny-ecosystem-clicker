#include "simulation.hpp"
#include <string>

using std::string;

void update(Simulation & simulation){
    simulation.update();
}

void draw(const Simulation & simulation){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    simulation.draw();

    string tutorial = "1) algae "
        "2) soil "
        "3) gravel "
        "4) sand "
        "5) ostracods "
        "6) plants";

    DrawText(tutorial.c_str(), 5, 5, 20, BLACK);

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