#include "simulation.hpp"

Simulation::Simulation(){}

void Simulation::spawnAlgae(){
    if(IsKeyDown(KEY_ONE)){
        algaes.emplace_back();
    }
}
void Simulation::updateAlgaes(){
    for(Algae & algaeIt : algaes){
        algaeIt.update(ecosystem.waterSurfaceY);
    }
}

void Simulation::spawnSand(){
    if(IsKeyDown(KEY_TWO)){
        sand.emplace_back();
    }
}
void Simulation::updateSand(){
    for(size_t sandIdx = 0; sandIdx < sand.size(); ++sandIdx){
        sand[sandIdx].update(ecosystem, sand, sandIdx);
    }
}

void Simulation::spawnOstracods(){
    if(IsKeyDown(KEY_THREE)){
        ostracods.emplace_back();
        ++totalOstracods;
    }
}
void Simulation::updateOstracods(){
    aliveOstracods = 0;
    for(Ostracod & ostracodIt : ostracods){
        ostracodIt.update(ecosystem, algaes, sand, aliveOstracods);
    }
    printf("Alive: %lu / %lu\n", aliveOstracods, totalOstracods);
}

void Simulation::update(){
    updateAlgaes();
    updateSand();
    updateOstracods();

    spawnAlgae();
    spawnSand();
    spawnOstracods();
}

void Simulation::draw() const {
    DrawRectangle(0, ecosystem.waterSurfaceY, SCREEN_WIDTH, ecosystem.waterLevel, BLUE); 
    for(const Algae & algaeIt : algaes){
        DrawCircle(algaeIt.pos.x, algaeIt.pos.y, algaeIt.radius, GREEN); 
    }
    for(const Sand & sandIt : sand){
        DrawCircle(sandIt.pos.x, sandIt.pos.y, sandIt.radius, YELLOW); 
    }
    for(const Ostracod & ostracodIt : ostracods){
        //DrawCircleLines(ostracod.pos.x, ostracod.pos.y, ostracod.vision, BLACK); 
        DrawCircle(ostracodIt.pos.x, ostracodIt.pos.y, ostracodIt.radius, DARKGRAY); 
    }
}