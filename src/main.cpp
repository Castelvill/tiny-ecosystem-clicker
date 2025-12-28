#include "raylib.h"

void update(){

}

void draw(){
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawText("Hello World", 190, 200, 20, BLACK);

    EndDrawing();
}


void mainLoop(){
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update();
        draw();
    }
}

int main(){
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "hello");
    SetTargetFPS(60);

    mainLoop();

    CloseWindow(); //Close window and OpenGL context

    return 0;
}