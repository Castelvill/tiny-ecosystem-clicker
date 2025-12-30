#include "ecosystem.hpp"

void update(Ecosystem & ecosystem){
    ecosystem.update();
}

void draw(const Ecosystem & ecosystem){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    ecosystem.draw();

    //Temporary text
    DrawText("Press 1 to spawn algae\nPress 2 to spawn sand\nPress 3 to spawn ostracod", 5, 5, 20, BLACK);

    EndDrawing();
}

void mainLoop(){
    Ecosystem ecosystem;
    ecosystem.init();
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update(ecosystem);
        draw(ecosystem);
    }
}

int main(){
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Tiny Ecosystem Clicker");
    SetTargetFPS(60);

    mainLoop();

    CloseWindow(); //Close window and OpenGL context

    return 0;
}