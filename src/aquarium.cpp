#include "aquarium.hpp"

Aquarium::Aquarium(){
    environment.position = {0, 0};
    environment.size = {0, 0};
}
Aquarium::Aquarium(Vector2 newPosition, Vector2 newSize){
    environment.position = newPosition;
    environment.size = newSize;
}
Aquarium::Aquarium(const Rectangle & newAquariumRect){
    environment.position = {newAquariumRect.x, newAquariumRect.y};
    environment.size = {newAquariumRect.width, newAquariumRect.height};
}

Aquarium::~Aquarium(){}

Rectangle Aquarium::getRectangle() const {
    return {
        environment.position.x, environment.position.y,
        environment.size.x, environment.size.y
    };
}

void Aquarium::spawnWaterDroplet(){
    waterDroplets.emplace_back();
}
void Aquarium::updateWaterDroplet(){
    for(auto dropletIt = waterDroplets.begin(); dropletIt != waterDroplets.end();){
        if(dropletIt->checkIfUnderwater(environment)){
            dropletIt = waterDroplets.erase(dropletIt);
            if(environment.waterLevel < environment.size.y)
                ++environment.waterLevel;
        }
        else{
            dropletIt->update(environment);
            ++dropletIt;
        }   
    }
}

void Aquarium::spawnAlgae(){
    algaes.emplace_back();
}
void Aquarium::updateAlgaes(){
    for(Algae & algaeIt : algaes){
        algaeIt.update(environment.getWaterSurfaceY());
    }
}

void Aquarium::spawnSoil(){
    substrate.emplace_back(SubstrateType::soil);
}
void Aquarium::spawnSand(){
    substrate.emplace_back(SubstrateType::sand);
}
void Aquarium::spawnGravel(){
    substrate.emplace_back(SubstrateType::gravel);
}
void Aquarium::updateSubstrate(){
    for(size_t substrateIdx = 0; substrateIdx < substrate.size(); ++substrateIdx){
        substrate[substrateIdx].update(environment, substrate, substrateIdx);
    }
}

void Aquarium::spawnOstracod(){
    ostracods.emplace_back();
}
void Aquarium::updateOstracods(){
    for(Ostracod & ostracodIt : ostracods){
        ostracodIt.update(environment, algaes, substrate);
    }
}

void Aquarium::spawnSeed(){
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
void Aquarium::updatePlants(){
    //Index-based iteration, because plants vector can grow when plants are updated.
    size_t currentSize = plants.size();
    for(size_t idx = 0; idx < currentSize; ++idx){
        plants[idx].update(environment, substrate);
        if(plants[idx].growNewParts){
            //When transfering seeds between aquariums, indexes will change.
            plants[idx].plantIdx = idx;

            plants[idx].growNewParts = false;
            growPlants(plants[idx], plants);
        }
    }
}

void Aquarium::update(){
    updateWaterDroplet();
    updateAlgaes();
    updateSubstrate();
    updateOstracods();
    updatePlants();
}

template<typename T>
void removeEntityOutsideTheScreen(vector<T> & entities, float currentScreenHeight){
    for(auto it = entities.begin(); it != entities.end();){
        if(it->pos.y > currentScreenHeight)
            it = entities.erase(it);
        else
            ++it;
    }
}
void Aquarium::removeEntitiesOutsideTheScreen(float currentScreenHeight){
    removeEntityOutsideTheScreen(waterDroplets, currentScreenHeight);
    removeEntityOutsideTheScreen(algaes, currentScreenHeight);
    removeEntityOutsideTheScreen(substrate, currentScreenHeight);
    removeEntityOutsideTheScreen(ostracods, currentScreenHeight);
    removeEntityOutsideTheScreen(plants, currentScreenHeight);
}

template<typename T>
int transferEntityType(Vector2 aquariumPos, Vector2 aquariumSize, vector<T> & outsideContainer,
    vector<T> & aquariumContainer
){
    int transfersCount = 0;
    for(auto it = outsideContainer.begin(); it != outsideContainer.end();){
        if(collisionOfPointAndRectangle(it->pos, toRectangle(aquariumPos, aquariumSize))){
            it->pos = it->pos - aquariumPos;
            aquariumContainer.push_back(*it);
            it = outsideContainer.erase(it);
            ++transfersCount;
        }   
        else
            ++it;
    }
    return transfersCount;
}
void Aquarium::transferEntitiesToOtherAquariums(vector<Aquarium> & aquariums, PlayerState & player){
    int transfersCount = 0;
    for(Aquarium & aquarium : aquariums){
        transfersCount += transferEntityType(aquarium.environment.position,
            aquarium.environment.size, waterDroplets, aquarium.waterDroplets
        );
        transfersCount += transferEntityType(aquarium.environment.position,
            aquarium.environment.size, algaes, aquarium.algaes
        );
        transfersCount += transferEntityType(aquarium.environment.position,
            aquarium.environment.size, substrate, aquarium.substrate
        );
        transfersCount += transferEntityType(aquarium.environment.position,
            aquarium.environment.size, ostracods, aquarium.ostracods
        );
        transfersCount += transferEntityType(aquarium.environment.position,
            aquarium.environment.size, plants, aquarium.plants
        );
    }
    if(transfersCount > 0)
        player.addExperience(transfersCount);
}
void Aquarium::removeWater(vector<Aquarium> & aquariums){
    Vector2 mousePosition = GetMousePosition();
    for(Aquarium & aquariumIt : aquariums){
        if(!detectButtonCollision(mousePosition, aquariumIt.environment.position,
            aquariumIt.environment.size
        ))
            continue;
        if(aquariumIt.environment.waterLevel > 0)
            --aquariumIt.environment.waterLevel;
        return;
    }
}
void Aquarium::updateGameArea(const UserInterface & gui, vector<Aquarium> & aquariums,
    PlayerState & player
){
    Vector2 mousePosition = GetMousePosition();
    if(IsMouseButtonPressed(0) && mousePosition.y > gui.GUI_HEIGHT){
        switch (gui.selectedInventorySlotIdx){
            case InventorySlots::slotAquarium:
                //TODO?
                break;
            case InventorySlots::slotWater:
                spawnWaterDroplet();
                break;
            case InventorySlots::slotRemoveWater:
                removeWater(aquariums);
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

    removeEntitiesOutsideTheScreen(gui.screenSize.y);

    transferEntitiesToOtherAquariums(aquariums, player);
}

void Aquarium::drawAquarium() const {
    //Background
    DrawRectangleV({0, 0}, environment.size, { 200, 200, 200, 200 });

    //Borders
    DrawLineEx({0, 0}, {0, 0 + environment.size.y},
        3, BLACK
    );
    DrawLineEx({0 + environment.size.x, 0}, {0 + environment.size.x, environment.size.y},
        3, BLACK
    );
    DrawLineEx({0, 0 + environment.size.y}, {0 + environment.size.x, environment.size.y},
        3, BLACK
    );
}

void Aquarium::drawEntities() const {
    for(const WaterDroplet & dropletIt : waterDroplets){
        DrawCircle(dropletIt.pos.x, dropletIt.pos.y, dropletIt.radius, Environment::WATER_COLOR); 
    }

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

void Aquarium::draw() const {
    BeginMode2D((Camera2D){
        .offset = environment.position,
        .target = (Vector2){0, 0},
        .rotation = 0.0f,
        .zoom = 1.0f
    });

    drawAquarium();

    drawEntities();

    //Water
    DrawRectangle(0, environment.getWaterSurfaceY(), environment.size.x, environment.waterLevel,
        Environment::WATER_COLOR
    ); 

    EndMode2D();
}

void Aquarium::expand(Direction direction, float length){
    switch (direction){
        case Direction::left:
            environment.position.x -= length;
            environment.size.x += length;
            return;
        case Direction::up:
            environment.position.y -= length;
            environment.size.y += length;
            return;
        case Direction::right:
            environment.size.x += length;
            return;
        default:
            return;
    }
}