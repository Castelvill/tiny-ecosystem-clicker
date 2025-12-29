#include "utilities.hpp"
#include <time.h>
#include <vector>
#include <limits>
#include <iostream>

using std::vector;

const int screenWidth = 800;
const int screenHeight = 450;
const int startingWaterLevel = screenHeight * 0.8;
const float gravity = 0.1f;
const float minSandFall = 0.5f;
const float maxParticleRadius = 10.0f;
const float speedLimit = 1.0f;
const float saturationFromAlgae = 20.0f;

#define VEC2(X,Y) (Vector2){X,Y}

class Entity{
public:
    bool active = true;
    Vector2 pos;
    Vector2 velocity;
    float radius;
};

class Ostracod: public Entity{
    constexpr static Vector2 sizeRange = VEC2(2.0f, 8.0f);
    constexpr static Vector2 visionRange = VEC2(180.0f, 200.0f);
    constexpr static Vector2 reactionTimeRange = VEC2(5.0f, 20.0f);
    constexpr static Vector2 speedRange = VEC2(0.4f, 0.8f);
public:
    constexpr static float baseSaturation = 30.0f;
    float saturation = 0.0f;
    float vision = 0.0f;
    float reactionTime = 0.0f;
    float reactionCooldown = 0.0f;
    float speed = 0.0f;
    Ostracod();
};

Ostracod::Ostracod(){
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    saturation = baseSaturation;
    radius = randomBetween(Ostracod::sizeRange.x, Ostracod::sizeRange.y);
    vision = randomBetween(Ostracod::visionRange.x, Ostracod::visionRange.y);
    reactionTime = randomBetween(Ostracod::reactionTimeRange.x, Ostracod::reactionTimeRange.y);
    speed = randomBetween(Ostracod::speedRange.x, Ostracod::speedRange.y);
}

class Ecosystem{
    float waterLevel = startingWaterLevel;
    float waterSurfaceY = screenHeight - waterLevel;
    vector<Entity> algaes;
    vector<Entity> sand;
    vector<Ostracod> ostracods;
public:
    void init();

    void spawnAlgae();
    void updateAlgaes();

    void spawnSand();
    void updateSand();

    Entity* findFirstVisibleAlgae(const Ostracod & ostracod);

    void spawnOstracods();
    void updateOstracods();

    void update();
    void draw() const;
};

void Ecosystem::init(){

}



void Ecosystem::spawnAlgae(){
    if(IsKeyDown(KEY_ONE)){
        algaes.emplace_back();
        algaes.back().pos = GetMousePosition();
        algaes.back().radius = 5 + rand() % 6;
    }
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

void Ecosystem::spawnSand(){
    if(IsKeyDown(KEY_TWO)){
        sand.emplace_back();
        sand.back().pos = GetMousePosition();
        sand.back().radius = 5 + rand() % 6;
    }
}
void settleSand(Entity & sand){
    sand.velocity.y = 0.0f;
    sand.active = false;
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
        if(sand[current].pos.y + sand[current].radius >= screenHeight){
            settleSand(sand[current]);
            continue;
        }

        //Check simple collisions between falling (active) and settled sand particles
        if(sand[current].velocity.y > 0){
            bool collisionOnTheLeft = false;
            bool collisionOnTheRight = false;
            for(size_t other = 0; other < sand.size(); ++other){
                //Ignore itself and other falling sand particles
                if(other == current || sand[other].active)
                    continue;

                float distance = getDistance(sand[current].pos, sand[other].pos);
                if(distance > sand[current].radius + sand[other].radius)
                    continue;

                if(sand[other].pos.x <= sand[current].pos.x)
                    collisionOnTheLeft = true;
                else
                    collisionOnTheRight = true;

                if(collisionOnTheLeft && collisionOnTheRight)
                    break;
            }

            if(collisionOnTheLeft && collisionOnTheRight){
                settleSand(sand[current]);
                continue;
            }
            else if(collisionOnTheLeft)
                sand[current].pos.x += 1.0f;
            else if(collisionOnTheRight)
                sand[current].pos.x -= 1.0f;
        }

        sand[current].pos.y += sand[current].velocity.y;
    }
}

void Ecosystem::spawnOstracods(){
    if(IsKeyDown(KEY_THREE)){
        ostracods.emplace_back();
    }
}
Entity* Ecosystem::findFirstVisibleAlgae(const Ostracod & ostracod){
    for(Entity & algea : algaes){
        float distance = getDistance(ostracod.pos, algea.pos);
        if(distance <= ostracod.vision)
            return &algea;
    }
    return nullptr;
}
void Ecosystem::updateOstracods(){
    for(size_t current = 0; current < ostracods.size(); ++current){
        bool underwater = ostracods[current].pos.y >= waterSurfaceY;

        if(ostracods[current].active){
            ostracods[current].saturation -= 0.1f;
            if(ostracods[current].saturation <= 0){
                ostracods[current].active = false;
                continue;
            }

            //AI
            if(ostracods[current].reactionCooldown > 0)
                ostracods[current].reactionCooldown -= 0.1f;
            else{
                ostracods[current].reactionCooldown = ostracods[current].reactionTime;

                float speed = ostracods[current].speed;
                if(!underwater)
                    speed *= 0.1;
                Vector2 movement = VEC2(0.0f, 0.0f);

                Entity* visibleAlgae = nullptr;
                if(ostracods[current].saturation <= Ostracod::baseSaturation)
                    visibleAlgae = findFirstVisibleAlgae(ostracods[current]);

                if(visibleAlgae == nullptr){ //Move randomly
                    movement = VEC2(
                        randomBetween(-speed, speed),
                        randomBetween(-speed, speed)
                    );
                }
                else{ //Move to algae
                    movement = VEC2(
                        visibleAlgae->pos.x - ostracods[current].pos.x,
                        visibleAlgae->pos.y - ostracods[current].pos.y
                    );
                    if(movement.x > 0)
                        movement.x = std::min(movement.x, speed);
                    if(movement.x < 0)
                        movement.x = std::max(movement.x, -speed);
                    if(movement.y > 0)
                        movement.y = std::min(movement.y, speed);
                    if(movement.y < 0)
                        movement.y = std::max(movement.y, -speed);
                }

                ostracods[current].velocity.x += movement.x;
                ostracods[current].velocity.y += movement.y;
            }
        }
        else{
            if(ostracods[current].velocity.x > 0.1f || ostracods[current].velocity.x < -0.1f)
                ostracods[current].velocity.x *= 0.99f;
            else
                ostracods[current].velocity.x = 0;
        }

        //Gravity and buoyancy
        if(!underwater)
            ostracods[current].velocity.y += gravity * (ostracods[current].radius / maxParticleRadius);
        else{
            float buoyancy = 1.0f - (ostracods[current].pos.y - waterSurfaceY) / waterLevel;
            buoyancy += minSandFall;

            if(ostracods[current].velocity.y == 0){
                ostracods[current].velocity.y = buoyancy;
            }
            ostracods[current].velocity.y = std::min(buoyancy, ostracods[current].velocity.y);
        }

        if(ostracods[current].velocity.x == 0 && ostracods[current].velocity.y == 0)
            continue;

        //Limit velocity
        if(ostracods[current].velocity.x > 0)
            ostracods[current].velocity.x = std::min(ostracods[current].velocity.x, speedLimit);
        if(ostracods[current].velocity.x < 0)
            ostracods[current].velocity.x = std::max(ostracods[current].velocity.x, -speedLimit);
        if(ostracods[current].velocity.y > 0)
            ostracods[current].velocity.y = std::min(ostracods[current].velocity.y, speedLimit);
        if(ostracods[current].velocity.y < 0)
            ostracods[current].velocity.y = std::max(ostracods[current].velocity.y, -speedLimit);

        Vector2 nextPosition = VEC2(
            ostracods[current].pos.x + ostracods[current].velocity.x,
            ostracods[current].pos.y + ostracods[current].velocity.y
        );

        //Stop ostracods from leaving the aquarium
        //Floor
        if(nextPosition.y + ostracods[current].radius >= screenHeight){
            ostracods[current].velocity.y = 0.0f;
        }
        //Right wall
        if(nextPosition.x + ostracods[current].radius >= screenWidth){
            ostracods[current].velocity.x = 0.0f;
        }
        //Left wall
        if(nextPosition.x - ostracods[current].radius <= 0){
            ostracods[current].velocity.x = 0.0f;
        }

        //Eat one algae
        if(ostracods[current].active){
            for(auto algae = algaes.begin(); algae != algaes.end();){
                float distance = getDistance(ostracods[current].pos, algae->pos);
                if(distance <= ostracods[current].radius + algae->radius){
                    algae = algaes.erase(algae);
                    ostracods[current].saturation += saturationFromAlgae;
                    break;
                }
                else{
                    ++algae;
                }
            }
        }

        ostracods[current].pos.x += ostracods[current].velocity.x;
        ostracods[current].pos.y += ostracods[current].velocity.y;
    }
}

void Ecosystem::update(){
    updateAlgaes();
    updateSand();
    updateOstracods();

    spawnAlgae();
    spawnSand();
    spawnOstracods();

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
    for(const Ostracod & ostracod : ostracods){
        //DrawCircleLines(ostracod.pos.x, ostracod.pos.y, ostracod.vision, BLACK); 
        DrawCircle(ostracod.pos.x, ostracod.pos.y, ostracod.radius, DARKGRAY); 
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