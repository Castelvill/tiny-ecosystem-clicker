#include "aquarium.hpp"

Aquarium::Aquarium(){
    environment.position = {0, 0};
    environment.size = {0, 0};
    environment.maxWaterLevel = 0.0f;
}
Aquarium::Aquarium(Vector2 newPosition, Vector2 newSize){
    environment.position = newPosition;
    environment.size = newSize;
    environment.maxWaterLevel = newSize.x * newSize.y;
}
Aquarium::Aquarium(const Rectangle & newAquariumRect){
    environment.position = {newAquariumRect.x, newAquariumRect.y};
    environment.size = {newAquariumRect.width, newAquariumRect.height};
    environment.maxWaterLevel = environment.size.x * environment.size.y;
}

Aquarium::~Aquarium(){}

Vector2 Aquarium::getPosition() const {
    return environment.position;
}
Rectangle Aquarium::getRectangle() const {
    return {
        environment.position.x, environment.position.y,
        environment.size.x, environment.size.y
    };
}

void Aquarium::setPosition(Vector2 newPosition){
    environment.position = newPosition;
}

void Aquarium::spawnWaterDroplet(){
    waterDroplets.emplace_back();
}
void Aquarium::updateWaterDroplet(){
    for(auto dropletIt = waterDroplets.begin(); dropletIt != waterDroplets.end();){
        if(dropletIt->checkIfUnderwater(environment)){
            dropletIt = waterDroplets.erase(dropletIt);
            environment.waterLevel = std::min(environment.maxWaterLevel,
                environment.waterLevel + WATER_DROPLET_VALUE
            );
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
    plants.back().initSeed(GetMousePosition(), plants.size()-1, 0);
}

void Aquarium::spawnSeed(Vector2 position){
    plants.emplace_back(Plant());
    plants.back().initSeed(position, plants.size()-1, 0);
}

inline void growStemsAndRootsFromSeed(Plant & parentPlant, vector<Plant> & plants){
    plants.emplace_back(Plant(PlantPartType::root, parentPlant, plants.size(),
        {0, parentPlant.dna.stemGrowthRate}
    ));
    plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(),
        {0, -parentPlant.dna.stemGrowthRate}
    ));
}

inline void growLeavesFromStem(Plant & parentPlant, vector<Plant> & plants){
    const float parentAngle = vectorToAngle(parentPlant.velocity);
    //Leaves from the main stem
    //Right leaf
    float newAngle = parentAngle + parentPlant.dna.leafBranchingAngle;
    Vector2 newVelocity = angleToVector(newAngle) * parentPlant.dna.stemGrowthRate;
    plants.emplace_back(Plant(PlantPartType::leaf, parentPlant, plants.size(),
        newVelocity
    ));
    //Left leaf
    newAngle = parentAngle - parentPlant.dna.leafBranchingAngle;
    newVelocity = angleToVector(newAngle) * parentPlant.dna.stemGrowthRate;
    plants.emplace_back(Plant(PlantPartType::leaf, parentPlant, plants.size(),
        newVelocity
    ));
}

inline void growBranches(Plant & parentPlant, vector<Plant> & plants){
    int maxBranches = randomBetween(parentPlant.dna.numberOfStemBranches);
    float branchingAngle = 0; 
    const float parentAngle = vectorToAngle(parentPlant.velocity);
    //Main stem extension
    if(parentPlant.velocity.x == 0)
        plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(),
            parentPlant.velocity
        ));
    //Branches from the main stem
    for(int branchIdx = 1; branchIdx <= maxBranches; ++branchIdx){
        branchingAngle *= -1;
        if(branchIdx % 2 == 1){
            branchingAngle += parentPlant.dna.stemBranchingAngle;
            if(rand() % 2 == 1 && branchIdx == maxBranches)
                branchingAngle *= -1;
        }
            
        float newAngle = parentAngle + branchingAngle;
        Vector2 newVelocity = angleToVector(newAngle) * parentPlant.dna.stemGrowthRate;
        plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(),
            newVelocity
        ));
    }
}

inline void extendLeaf(Plant & parentPlant, vector<Plant> & plants){
    const float parentAngle = vectorToAngle(parentPlant.velocity);
    const float gravityForceAngle = 0.2f;
    float newAngle = parentAngle;
    if(parentPlant.velocity.x <= 0.0f)
        newAngle -= gravityForceAngle;
    else 
        newAngle += gravityForceAngle;
    Vector2 newVelocity = angleToVector(newAngle) * parentPlant.dna.stemGrowthRate;
    plants.emplace_back(Plant(PlantPartType::leaf, parentPlant, plants.size(),
        newVelocity
    ));
}

//Btw, you can merge it with growBranches by adding additional parameters
inline void growRootBranches(Plant & parentPlant, vector<Plant> & plants){
    int maxBranches = randomBetween(parentPlant.dna.numberOfRootBranches);
    float branchingAngle = 0; 
    const float parentAngle = vectorToAngle(parentPlant.velocity);
    //Main stem extension
    if(parentPlant.velocity.x == 0)
        plants.emplace_back(Plant(PlantPartType::root, parentPlant, plants.size(),
            parentPlant.velocity
        ));
    //Branches from the main stem
    for(int branchIdx = 1; branchIdx <= maxBranches; ++branchIdx){
        branchingAngle *= -1;
        if(branchIdx % 2 == 1){
            branchingAngle += parentPlant.dna.rootBranchingAngle;

            if(rand() % 2 == 1 && branchIdx == maxBranches)
                branchingAngle *= -1;
        }
        
        float newAngle = parentAngle + branchingAngle;
        Vector2 newVelocity = angleToVector(newAngle) * parentPlant.dna.rootGrowthRate;
        plants.emplace_back(Plant(PlantPartType::root, parentPlant, plants.size(),
            newVelocity
        ));
    }
}

//Parent must be a copy, because plants vector will grow here
void growPlants(Plant parentPlant, vector<Plant> & plants){
    switch (parentPlant.growthDecision){
        case GrowthDecision::growStemsAndRootsFromSeed:
            growStemsAndRootsFromSeed(parentPlant, plants);
            return;
        case GrowthDecision::growBranches:
            growBranches(parentPlant, plants);
            return;
        case GrowthDecision::growLeavesFromStem:
            growLeavesFromStem(parentPlant, plants);
            return;
        case GrowthDecision::growLeavesAndExtendStem:
            growLeavesFromStem(parentPlant, plants);
            plants.emplace_back(Plant(PlantPartType::stem, parentPlant, plants.size(),
                parentPlant.velocity, true
            ));
            return;
        case GrowthDecision::extendLeaf:
            extendLeaf(parentPlant, plants);
            return;
        case GrowthDecision::growRootBranches:
            growRootBranches(parentPlant, plants);
            return;
        default:
            return;
    }
} 
void Aquarium::updatePlants(){
    //Index-based iteration, because plants vector can grow when plants are updated.
    size_t currentSize = plants.size();

    //Reset present information about whole plants
    for(WholePlantData & wholePlantData : wholePlants){
        wholePlantData.waterStoredNow = 0.0f;
        wholePlantData.waterAbsorbedNow = 0.0f;
        wholePlantData.waterConsumedNow = 0.0f;
    }
    
    for(size_t idx = 0; idx < currentSize; ++idx){
        plants[idx].update(environment, substrate, &plants[plants[idx].parentPartIdx],
            plants[idx].plantIdx < wholePlants.size() ? &wholePlants[plants[idx].plantIdx] : nullptr
        );
        if(plants[idx].growthDecision != GrowthDecision::doNothing
            //&& plants[idx].waterStored >= plants[idx].dna.minWaterToDivide
        ){
            assert(plants[idx].plantIdx < wholePlants.size());
            if(!wholePlants[plants[idx].plantIdx].canGrow)
                continue;

            //When transfering seeds between aquariums, indexes will change - must update plantPartIdx.
            plants[idx].plantPartIdx = idx;
            const size_t previousSize = plants.size();
            growPlants(plants[idx], plants);
            plants[idx].growthDecision = GrowthDecision::doNothing;

            if(plants[idx].plantIdx < wholePlants.size())
                wholePlants[plants[idx].plantIdx].plantPartsCount += plants.size() - previousSize;
        }
    }

    for(size_t plantIdx = 0; plantIdx < wholePlants.size(); ++plantIdx){
        wholePlants[plantIdx].canGrow = wholePlants[plantIdx].waterConsumedNow
            <= wholePlants[plantIdx].waterAbsorbedNow;
        // printf("Plant %ld:\n\tparts: %ld\n\tabsorbed: %f\n\tconsumed: %f\n\tstored: %f\n\tsubstrate: %ld\n\tcan grow: %d\n",
        //     plantIdx, wholePlants[plantIdx].plantPartsCount, wholePlants[plantIdx].waterAbsorbedNow,
        //     wholePlants[plantIdx].waterConsumedNow, wholePlants[plantIdx].waterStoredNow,
        //     wholePlants[plantIdx].touchedSand + wholePlants[plantIdx].touchedGravel
        //     + wholePlants[plantIdx].touchedSoil, wholePlants[plantIdx].canGrow
        // );
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
int transferEntityType(Vector2 aquariumPos, Vector2 aquariumSize, vector<Plant> & outsideContainer,
    vector<Plant> & aquariumContainer, vector<WholePlantData> & wholePlants
){
    int transfersCount = 0;
    for(auto it = outsideContainer.begin(); it != outsideContainer.end();){
        if(collisionOfPointAndRectangle(it->pos, toRectangle(aquariumPos, aquariumSize))){
            it->pos = it->pos - aquariumPos;
            aquariumContainer.push_back(*it);
            it = outsideContainer.erase(it);
            ++transfersCount;

            aquariumContainer.back().plantIdx = wholePlants.size();
            wholePlants.emplace_back(WholePlantData());
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
            aquarium.environment.size, plants, aquarium.plants, aquarium.wholePlants
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
        aquariumIt.environment.waterLevel = std::max(
            0.0f, aquariumIt.environment.waterLevel - WATER_DROPLET_VALUE
        );
        return;
    }
}
void Aquarium::updateGameArea(const UserInterface & gui, vector<Aquarium> & aquariums,
    PlayerState & player
){
    Vector2 mousePosition = GetMousePosition();
    if(mousePosition.y > gui.GUI_HEIGHT && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)))  
    ){
        switch (gui.selectedInventorySlotIdx){
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
        DrawCircle(substrateIt.pos.x, substrateIt.pos.y, substrateIt.radius, substrateIt.color);
    }

    for(const Ostracod & ostracodIt : ostracods){
        //DrawCircleLines(ostracod.pos.x, ostracod.pos.y, ostracod.vision, BLACK); 
        DrawCircle(ostracodIt.pos.x, ostracodIt.pos.y, ostracodIt.radius, GRAY); 
    }
    
    for(const Plant & plantIt : plants){
        switch (plantIt.type){
            case PlantPartType::seed:
                DrawCircleV(plantIt.pos, plantIt.radius, plantIt.color);
                break;
            case PlantPartType::stem:
                DrawLineEx(plants[plantIt.parentPartIdx].pos, plantIt.pos, 5, plantIt.color);
                break;
            case PlantPartType::leaf:{
                Vector2 point = plants[plantIt.parentPartIdx].pos;
                
                if(plantIt.active || plantIt.currentLevel >= plantIt.dna.leafMaxLevel){
                    float lineAngle = vectorToAngle(getDirectionVector(point, plantIt.pos));
                    Vector2 leftPoint = point + angleToVector(lineAngle - 0.5*PI) * 5;
                    Vector2 rightPoint = point + angleToVector(lineAngle + 0.5*PI) * 5; 
                    DrawTriangle(plantIt.pos, rightPoint, leftPoint, plantIt.color);
                }
                else{
                    DrawLineEx(plants[plantIt.parentPartIdx].pos, plantIt.pos, 10, plantIt.color);
                }
            }
                break;
            case PlantPartType::root:
                DrawLineEx(plants[plantIt.parentPartIdx].pos, plantIt.pos, 3, plantIt.color);
                //DrawCircleV(plantIt.pos, Plant::ROOT_DRINKING_RANGE, { 230, 41, 55, 100 });
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
    DrawRectangle(0, environment.getWaterSurfaceY(), environment.size.x,
        environment.waterLevel / environment.size.x ,Environment::WATER_COLOR
    ); 

    EndMode2D();
}

void Aquarium::expand(Direction direction, float length){
    switch (direction){
        case Direction::left:
            environment.position.x -= length;
            environment.size.x += length;
            break;
        case Direction::up:
            environment.position.y -= length;
            environment.size.y += length;
            break;
        case Direction::right:
            environment.size.x += length;
            break;
        default:
            break;
    }
    environment.maxWaterLevel = environment.size.x * environment.size.y;
}

bool Aquarium::isExpansionDisabled() const {
    return substrate.size() > 0 || plants.size() > 0;
}