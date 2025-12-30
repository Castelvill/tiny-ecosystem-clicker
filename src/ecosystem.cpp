#include "ecosystem.hpp"

void Ecosystem::init(){
    waterLevel = startingWaterLevel;
    waterSurfaceY = screenHeight - waterLevel;
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

void Ecosystem::updateOstracods(){
    for(size_t current = 0; current < ostracods.size(); ++current){
        ostracods[current].update(*this);
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