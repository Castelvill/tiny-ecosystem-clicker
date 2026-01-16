#include "simulation.hpp"

Simulation::Simulation(){}

Simulation::~Simulation(){}

void Simulation::spawnAlgae(){
    algaes.emplace_back();
}
void Simulation::updateAlgaes(){
    for(Algae & algaeIt : algaes){
        algaeIt.update(environment.waterSurfaceY);
    }
}

void Simulation::spawnSoil(){
    substrate.emplace_back(SubstrateType::soil);
}
void Simulation::spawnSand(){
    substrate.emplace_back(SubstrateType::sand);
}
void Simulation::spawnGravel(){
    substrate.emplace_back(SubstrateType::gravel);
}
void Simulation::updateSubstrate(){
    for(size_t substrateIdx = 0; substrateIdx < substrate.size(); ++substrateIdx){
        substrate[substrateIdx].update(environment, substrate, substrateIdx);
    }
}

void Simulation::spawnOstracod(){
    ostracods.emplace_back();
    ++totalOstracods;
}
void Simulation::updateOstracods(){
    aliveOstracods = 0;
    for(Ostracod & ostracodIt : ostracods){
        ostracodIt.update(environment, algaes, substrate, aliveOstracods);
    }
    //printf("Alive: %lu / %lu\n", aliveOstracods, totalOstracods);
}

void Simulation::spawnSeed(){
    plants.emplace_back(Plant());
    plants.back().initSeed(GetMousePosition(), plants.size()-1);
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

void Simulation::update(const UserInterface & gui){
    updateAlgaes();
    updateSubstrate();
    updateOstracods();
    updatePlants();

    Vector2 mousePosition = GetMousePosition();
    if(IsMouseButtonPressed(0)
        && mousePosition.y > gui.GUI_HEIGHT
        //Temporary guardrails until proper aquariums are added
        && mousePosition.x < SCREEN_WIDTH 
        && mousePosition.y < SCREEN_HEIGHT
    ){
        switch (gui.selectedInventorySlotIdx){
            case InventorySlots::slotWater:
                if(environment.waterLevel < MAX_WATER_LEVEL){
                    ++environment.waterLevel;
                    environment.updateWaterSurfaceY();
                }
                break;
            case InventorySlots::slotRemoveWater: 
                if(environment.waterLevel > MIN_WATER_LEVEL){
                    --environment.waterLevel;
                    environment.updateWaterSurfaceY();
                }
                break;
            case InventorySlots::slotAlgae: 
                spawnAlgae();
                break;
            case InventorySlots::slotSoil: 
                spawnSoil();
                break;
            case InventorySlots::slotGravel: 
                spawnGravel();
                break;
            case InventorySlots::slotSand: 
                spawnSand();
                break;
            case InventorySlots::slotOstracod: 
                spawnOstracod();
                break;
            case InventorySlots::slotSeed: 
                spawnSeed();
                break;
            default:
                break;
        }
    }
}

void Simulation::draw() const {
    //Water
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