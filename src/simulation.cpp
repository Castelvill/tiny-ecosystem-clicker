#include "simulation.hpp"

Simulation::Simulation(){}

void Simulation::spawnAlgae(){
    if(IsKeyDown(SPAWN_ALGAE_BUTTON)){
        algaes.emplace_back();
    }
}
void Simulation::updateAlgaes(){
    for(Algae & algaeIt : algaes){
        algaeIt.update(environment.waterSurfaceY);
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
        substrate[substrateIdx].update(environment, substrate, substrateIdx);
    }
}

void Simulation::spawnOstracod(){
    if(IsKeyDown(SPAWN_OSTRACODS_BUTTON)){
        ostracods.emplace_back();
        ++totalOstracods;
    }
}
void Simulation::updateOstracods(){
    aliveOstracods = 0;
    for(Ostracod & ostracodIt : ostracods){
        ostracodIt.update(environment, algaes, substrate, aliveOstracods);
    }
    //printf("Alive: %lu / %lu\n", aliveOstracods, totalOstracods);
}

void Simulation::spawnSeed(){
    if(IsKeyReleased(SPAWN_SEED_BUTTON)){
        plants.emplace_back(Plant());
        plants.back().initSeed(GetMousePosition(), plants.size()-1);
    }
}

//Parent must be a copy, because plants vector will grow
void growPlants(Plant parentPlant, vector<Plant> & plants){
    switch (parentPlant.type){
        case PlantPartType::seed:
            plants.emplace_back(Plant(PlantPartType::root, parentPlant, plants.size(), 1, 1));
            plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(), 1, 1));
            break;
        case PlantPartType::stem:{
            int maxBranches = randomBetween(parentPlant.dna.numberOfStemBranches);
            for(int branchIdx = 1; branchIdx <= maxBranches; ++branchIdx)
                plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(),
                    maxBranches, branchIdx
                ));
            break;
        }   
        default:
            break;
    }
} 
void Simulation::updatePlants(){
    //Index-based iteration, because plants vector can grow when plants are updated.
    size_t currentSize = plants.size();
    for(size_t idx = 0; idx < currentSize; ++idx){
        plants[idx].update(environment, substrate);
        if(plants[idx].growNewParts){
            plants[idx].growNewParts = false;
            growPlants(plants[idx], plants);
        }
    }
}

void Simulation::update(){
    updateAlgaes();
    updateSubstrate();
    updateOstracods();
    updatePlants();

    spawnAlgae();
    spawnSoil();
    spawnGravel();
    spawnSand();
    spawnOstracod();
    spawnSeed();
}

void Simulation::draw() const {
    DrawRectangle(0, environment.waterSurfaceY, SCREEN_WIDTH, environment.waterLevel, BLUE); 
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
    for(const Plant & plantIt : plants){
        switch (plantIt.type){
            case PlantPartType::seed:
                DrawCircleV(plantIt.pos, plantIt.radius, LIME);
                break;
            case PlantPartType::stem:
                DrawLineEx(plants[plantIt.parentIdx].pos, plantIt.pos, 5, GREEN);
                break;
            case PlantPartType::root:
                DrawLineEx(plants[plantIt.parentIdx].pos, plantIt.pos, 5, YELLOW);
                break;
            default:
                break;
        }
    }
}