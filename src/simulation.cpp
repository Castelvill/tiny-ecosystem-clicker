#include "simulation.hpp"

Simulation::Simulation(){}

void Simulation::spawnAlgae(){
    if(IsKeyDown(SPAWN_ALGAE_BUTTON)){
        algaes.emplace_back();
    }
}
void Simulation::updateAlgaes(){
    for(Algae & algaeIt : algaes){
        algaeIt.update(ecosystem.waterSurfaceY);
    }
}

void Simulation::spawnSoil(){
    if(IsKeyDown(SPAWN_SOIL_BUTTON)){
        substrate.emplace_back(SubstrateType::soil);
    }
}
void Simulation::spawnSand(){
    if(IsKeyDown(SPAWN_SAND_BUTTON)){
        substrate.emplace_back(SubstrateType::sand);
    }
}
void Simulation::spawnGravel(){
    if(IsKeyDown(SPAWN_GRAVEL_BUTTON)){
        substrate.emplace_back(SubstrateType::gravel);
    }
}
void Simulation::updateSubstrate(){
    for(size_t substrateIdx = 0; substrateIdx < substrate.size(); ++substrateIdx){
        substrate[substrateIdx].update(ecosystem, substrate, substrateIdx);
    }
}

void Simulation::spawnOstracods(){
    if(IsKeyDown(SPAWN_OSTRACODS_BUTTON)){
        ostracods.emplace_back();
        ++totalOstracods;
    }
}
void Simulation::updateOstracods(){
    aliveOstracods = 0;
    for(Ostracod & ostracodIt : ostracods){
        ostracodIt.update(ecosystem, algaes, substrate, aliveOstracods);
    }
    //printf("Alive: %lu / %lu\n", aliveOstracods, totalOstracods);
}

void Simulation::update(){
    updateAlgaes();
    updateSubstrate();
    updateOstracods();

    spawnAlgae();
    spawnSoil();
    spawnGravel();
    spawnSand();
    spawnOstracods();
}

void Simulation::draw() const {
    DrawRectangle(0, ecosystem.waterSurfaceY, SCREEN_WIDTH, ecosystem.waterLevel, BLUE); 
    for(const Algae & algaeIt : algaes){
        DrawCircle(algaeIt.pos.x, algaeIt.pos.y, algaeIt.radius, GREEN); 
    }
    for(const Substrate & substrateIt : substrate){
        switch (substrateIt.type){
        case SubstrateType::soil:
            DrawCircle(substrateIt.pos.x, substrateIt.pos.y, substrateIt.radius, BLACK);
            break;
        case SubstrateType::sand:
            DrawCircle(substrateIt.pos.x, substrateIt.pos.y, substrateIt.radius, YELLOW);
            break;
        case SubstrateType::gravel:
            DrawCircle(substrateIt.pos.x, substrateIt.pos.y, substrateIt.radius, DARKGRAY);
            break;
        default:
            break;
        }
    }
    for(const Ostracod & ostracodIt : ostracods){
        //DrawCircleLines(ostracod.pos.x, ostracod.pos.y, ostracod.vision, BLACK); 
        DrawCircle(ostracodIt.pos.x, ostracodIt.pos.y, ostracodIt.radius, GRAY); 
    }
}