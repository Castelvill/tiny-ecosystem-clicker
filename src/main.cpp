#include <stdlib.h>
#include <time.h>
#include <vector>
#include <limits>
#include <iostream>
#include <math.h>
#include "raylib.h"

using std::vector;

const int screenWidth = 800;
const int screenHeight = 450;
const int startingWaterLevel = screenHeight * 0.8;
const float gravity = 0.1f;
const float minSandFall = 0.5f;
const float maxParticleRadius = 10.0f;

class Entity{
public:
    bool active = true;
    Vector2 pos;
    Vector2 velocity;
    float radius;
};

class Ecosystem{
    float waterLevel = startingWaterLevel;
    float waterSurfaceY = screenHeight - waterLevel;
    vector<Entity> algaes;
    vector<Entity> sand;
public:
    void init();
    void updateAlgaes();
    void updateSand();
    void spawnAlgae();
    void spawnSand();
    void update();
    void draw() const;
};

void Ecosystem::init(){

}

float getDistance(Vector2 pos1, Vector2 pos2){
    return std::sqrt(std::pow(pos1.x-pos2.x, 2) + std::pow(pos1.y-pos2.y, 2));
}

void Ecosystem::updateAlgaes(){
    for(Entity & algae : algaes){
        //Gravity and buoyancy
        if(algae.pos.y < waterSurfaceY){
            algae.velocity.y += gravity * (algae.radius / maxParticleRadius);
        }
        else{
            algae.velocity.y = - (algae.pos.y - waterSurfaceY) * 0.01
                * (algae.radius / maxParticleRadius);
        }
        algae.pos.y += algae.velocity.y;
    }
}
void Ecosystem::updateSand(){
    for(size_t current = 0; current < sand.size(); ++current){
        if(!sand[current].active){
            continue;
        }

        //Gravity and buoyancy
        if(sand[current].pos.y < waterSurfaceY){
            sand[current].velocity.y += gravity * (sand[current].radius / maxParticleRadius);
        }
        else{
            float buoyancy = 1.0f - (sand[current].pos.y - waterSurfaceY) / waterLevel;
            buoyancy += minSandFall;

            if(sand[current].velocity.y == 0){
                sand[current].velocity.y = buoyancy;
            }
            sand[current].velocity.y = std::min(buoyancy, sand[current].velocity.y);
        }

        //When sand particles hit the ground, stop them.
        if(sand[current].pos.y + sand[current].radius / 2 >= screenHeight){
            sand[current].velocity.y = 0.0f;
            sand[current].active = false;
        }

        //Check simple collisions between falling (active) and settled sand particles
        if(sand[current].velocity.y > 0){
            for(size_t other = 0; other < sand.size(); ++other){
                if(other == current || sand[other].active)
                    continue;

                float distance = getDistance(sand[current].pos, sand[other].pos);
                if(distance > sand[current].radius + sand[other].radius)
                    continue;

                if(std::abs(sand[current].pos.x - sand[other].pos.x) < 1.0f){
                    sand[current].velocity.y = 0.0f;
                    sand[current].active = false;
                }
                else if(sand[current].pos.x < sand[other].pos.x)
                    sand[current].pos.x -= 1.0f;
                else
                    sand[current].pos.x += 1.0f;
            }
        }

        sand[current].pos.y += sand[current].velocity.y;
    }
}

void Ecosystem::spawnAlgae(){
    if(IsKeyDown(KEY_ONE)){
        algaes.emplace_back();
        algaes.back().pos = GetMousePosition();
        algaes.back().radius = 5 + rand() % 6;
    }
}

void Ecosystem::spawnSand(){
    if(IsKeyDown(KEY_TWO)){
        sand.emplace_back();
        sand.back().pos = GetMousePosition();
        sand.back().radius = 5 + rand() % 6;
    }
}

void Ecosystem::update(){
    updateAlgaes();

    updateSand();

    spawnAlgae();

    spawnSand();

    // Test: Water evaporation
    // waterLevel -= 0.1f;
    // waterSurfaceY = screenHeight - waterLevel;
}

void Ecosystem::draw() const {
    DrawRectangle(0, waterSurfaceY, screenWidth, waterLevel, BLUE); 
    for(const Entity & algae : algaes){
        DrawCircle(algae.pos.x, algae.pos.y, algae.radius, GREEN); 
    }
    for(const Entity & tinyRock : sand){
        DrawCircle(tinyRock.pos.x, tinyRock.pos.y, tinyRock.radius, YELLOW); 
    }
}

void update(Ecosystem & ecosystem){
    ecosystem.update();
}

void draw(const Ecosystem & ecosystem){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    ecosystem.draw();

    //Temporary text
    DrawText("Press 1 to spawn algae, press 2 to spawn sand", 5, 5, 20, BLACK);

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