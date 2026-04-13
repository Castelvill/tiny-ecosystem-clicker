#include "aquarium.hpp"

Sector::Sector(){};

Sector::Sector(Vector2 newPosition, size_t newColumn, size_t newRow){
    position = newPosition;
    column = newColumn;
    row = newRow;
}
bool Sector::canSpawnAlgae(){
    return useLowDetail ? algaeCount < MAX_LOW_DETAIL_ALGAE_SIZE : algaeCount < MAX_ALGAE_SIZE;
}

Rectangle Sector::toRectangle() const {
    return {position.x, position.y, SIZE.x, SIZE.y};
}

void SectorController::updateCoverage(Vector2 containerSize){
    if(sectors.empty()){
        rows = 1;
        columns = 1;
        sectors.emplace_back(Sector({0, 0}, 0, 0));
        size = Sector::SIZE;
    }
    if(size.x < containerSize.x){
        const int sizeDifference = containerSize.x - size.x;
        size_t columnsToAdd = std::ceil(sizeDifference / Sector::SIZE.x);
        for(size_t columnCounter = 0; columnCounter < columnsToAdd; ++columnCounter){
            for(size_t rowCounter = 0; rowCounter < rows; ++rowCounter){
                sectors.emplace_back(Sector({
                    columns * Sector::SIZE.x,
                    rowCounter * Sector::SIZE.y
                }, columns, rowCounter));
                //Connect new sector with the one added in the previous iteration
                if(rowCounter != 0){
                    sectors.back().neighbourIndexes.emplace_back(sectors.size() - 2);
                    sectors[sectors.size() - 2].neighbourIndexes.emplace_back(sectors.size() - 1);
                }
                //Connect new sector with a neighbouring sector to the left
                for(size_t sectorIdx = 0; sectorIdx < sectors.size(); ++sectorIdx){
                    if(sectors.back().column - 1 == sectors[sectorIdx].column
                        && sectors.back().row == sectors[sectorIdx].row
                    ){
                        sectors.back().neighbourIndexes.emplace_back(sectorIdx);
                        sectors[sectorIdx].neighbourIndexes.emplace_back(sectors.size() - 1);
                        break;
                    }
                }
            }
            ++columns;
            size.x += Sector::SIZE.x;
        }
    }
    if(size.y < containerSize.y){
        const int sizeDifference = containerSize.y - size.y;
        size_t rowsToAdd = std::ceil(sizeDifference / Sector::SIZE.y);
        for(size_t rowCounter = 0; rowCounter < rowsToAdd; ++rowCounter){
            for(size_t columnCounter = 0; columnCounter < columns; ++columnCounter){
                sectors.emplace_back(Sector({
                    columnCounter * Sector::SIZE.x,
                    rows * Sector::SIZE.y
                }, columnCounter, rows));
                //Connect new sector with the one added in the previous iteration
                if(columnCounter != 0){
                    sectors.back().neighbourIndexes.emplace_back(sectors.size() - 2);
                    sectors[sectors.size() - 2].neighbourIndexes.emplace_back(sectors.size() - 1);
                }
                //Connect new sector with a neighbouring sector above
                for(size_t sectorIdx = 0; sectorIdx < sectors.size(); ++sectorIdx){
                    if(sectors.back().column == sectors[sectorIdx].column
                        && sectors.back().row - 1 == sectors[sectorIdx].row
                    ){
                        sectors.back().neighbourIndexes.emplace_back(sectorIdx);
                        sectors[sectorIdx].neighbourIndexes.emplace_back(sectors.size() - 1);
                        break;
                    }
                }
            }
            ++rows;
            size.y += Sector::SIZE.y;
        }
    }
}
void SectorController::switchToLowDetailAlgaeMode(Sector & sectorIt){
    if(!sectorIt.useLowDetail && sectors.size() > LOW_DETAIL_SECTOR_LIMIT
        && sectorIt.algaeCount >= Sector::MAX_ALGAE_SIZE
    )
        sectorIt.useLowDetail = true;
    else if(sectorIt.algaeCount <= 1)
        sectorIt.useLowDetail = false;
}

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
    sectorController.updateCoverage(environment.size);
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
size_t Aquarium::getAlgaeSize() const {
    return algaes.size();
}

void Aquarium::setPosition(Vector2 newPosition){
    environment.position = newPosition;
}

void Aquarium::spawnWaterDroplet(){
    waterDroplets.emplace_back();
}
void Aquarium::updateWaterDroplet(){
    const float waterDropletValue = std::max(NORMAL_WATER_DROPLET_VALUE,
        float(environment.maxWaterLevel * WATER_DROPLET_VALUE_FACTOR)
    );
    for(auto dropletIt = waterDroplets.begin(); dropletIt != waterDroplets.end();){
        if(dropletIt->checkIfUnderwater(environment)){
            dropletIt = waterDroplets.erase(dropletIt);
            environment.waterLevel = std::min(environment.maxWaterLevel,
                environment.waterLevel + waterDropletValue
            );
        }
        else{
            dropletIt->update(environment);
            ++dropletIt;
        }   
    }
}

void Aquarium::spawnAlgae(){
    //algaes.emplace_back(rand() % 2 == 0 ? AlgaeType::floating_algae : AlgaeType::glass_algae);
    algaes.emplace_back(AlgaeType::glass_algae);
}

inline Vector2 findSpawnPosition(const SectorController & sectorController,
    const Sector & sector, Vector2 aquariumSize, float waterSurfaceY
){
    //Sector mesh can be bigger than the aquarium.
    //Sectors on the edges must be clipped to the aquarium size. 
    Vector2 reducedPosition = sector.position;
    Vector2 reducedSize = Sector::SIZE;
    if(sector.column == sectorController.columns - 1)
        reducedSize.x -= sectorController.size.x - aquariumSize.x;
    if(sector.row == sectorController.rows - 1)
        reducedSize.y -= sectorController.size.y - aquariumSize.y;

    //Don't spawn algae above the water.
    if(sector.position.y < waterSurfaceY){
        reducedSize.y -= waterSurfaceY - sector.position.y;
        reducedPosition.y = waterSurfaceY;
    }

    return {
        randomBetween(reducedPosition.x, reducedPosition.x + reducedSize.x),
        randomBetween(reducedPosition.y, reducedPosition.y + reducedSize.y)
    };
}

AlgaeSpawnInfo Aquarium::findSpawnLocation(Sector & sectorIt, size_t sectorIdx){
    sectorController.switchToLowDetailAlgaeMode(sectorIt);
    
    //Spawn algae inside the current sector
    if(sectorIt.canSpawnAlgae()){
        ++sectorIt.algaeCount;
        return AlgaeSpawnInfo {
            .canSpawn = true,
            .spawnPosition = findSpawnPosition(sectorController, sectorIt, environment.size,
                environment.getWaterSurfaceY()),
            .sectorIdx = sectorIdx,
            .useLowDetail = sectorIt.useLowDetail
        };
    }

    //Spawn algae inside a neighbour of the current sector.
    //Shuffle neighbour indexes vector with the Fisher–Yates method.
    for(size_t i = sectorIt.neighbourIndexes.size(); i > 1; --i){
        std::swap(sectorIt.neighbourIndexes[i - 1], sectorIt.neighbourIndexes[rand() % i]);
    }
    for(const size_t & neighbourIdx : sectorIt.neighbourIndexes){
        Sector & neighbour = sectorController.sectors[neighbourIdx];
        if(!neighbour.canSpawnAlgae())
            continue;
        
        ++neighbour.algaeCount;
        return AlgaeSpawnInfo {
            .canSpawn = true,
            .spawnPosition = findSpawnPosition(sectorController, neighbour, environment.size,
                environment.getWaterSurfaceY()),
            .sectorIdx = neighbourIdx
        };
    }

    return AlgaeSpawnInfo{};
}

bool Aquarium::reproduceAlgae(size_t algaeIdx){
    Algae & currentAlgae = algaes[algaeIdx];
    if(!currentAlgae.alive || !currentAlgae.active || currentAlgae.timeToReproduce > 0)
        return false;

    AlgaeSpawnInfo spawnInfo;

    if(currentAlgae.isPlayerMade){
        //Find the sector where the current algae can spawn its child
        for(size_t sectorIdx = 0; sectorIdx < sectorController.sectors.size(); ++sectorIdx){
            Sector & sectorIt = sectorController.sectors[sectorIdx];
            if(!collisionOfPointAndRectangle(currentAlgae.pos, sectorIt.toRectangle()))
                continue;

            spawnInfo = findSpawnLocation(sectorIt, sectorIdx);
            
            break;
        }
    }
    else{
        Sector & sectorIt = sectorController.sectors[currentAlgae.sectorIdx];
        spawnInfo = findSpawnLocation(sectorIt, currentAlgae.sectorIdx);
    }
    
    if(!spawnInfo.canSpawn)
        return false;
    
    currentAlgae.resetReproduction();
    algaes.emplace_back(currentAlgae.type);
    algaes.back().reproduce(spawnInfo.spawnPosition, environment, spawnInfo.useLowDetail);
    algaes.back().sectorIdx = spawnInfo.sectorIdx;
    algaes.back().isPlayerMade = false;
    return true;
}
//Erase dead algae or turn it into substrate
void Aquarium::removeDeadAlgae(){
    for(auto algaeIt = algaes.begin(); algaeIt != algaes.end();){
        if(!algaeIt->alive){
            if(!algaeIt->isPlayerMade && algaeIt->sectorIdx < sectorController.sectors.size())
                --sectorController.sectors[algaeIt->sectorIdx].algaeCount;

            if(randomBetween(0.0f, 1.0f) < Algae::chanceToTurnIntoSubstrate){
                substrate.emplace_back(SubstrateType::deadAlgae);
                substrate.back().pos = algaeIt->pos;
                substrate.back().radius = algaeIt->radius * 0.5;
            }
            algaeIt = algaes.erase(algaeIt);
        }
        else
            ++algaeIt;
    }
}
void Aquarium::updateAlgaes(){
    size_t originalAlgaeCount = algaes.size();
    for(size_t algaeIdx = 0; algaeIdx < originalAlgaeCount; ++algaeIdx){
        algaes[algaeIdx].update(environment, algaes, algaeIdx);
        reproduceAlgae(algaeIdx);
    }

    removeDeadAlgae();
}
void Aquarium::updateAlgaesOutsideAquarium(){
    for(size_t algaeIdx = 0; algaeIdx < algaes.size();++algaeIdx)
        algaes[algaeIdx].update(environment, algaes, algaeIdx);
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

    //printf("Algaes: %ld, Substrate: %ld\n", algaes.size(), substrate.size());
}

void Aquarium::updateOutsideAquarium(){
    updateWaterDroplet();
    updateAlgaesOutsideAquarium();
    updateSubstrate();
    updateOstracods();
    updatePlants();

    //printf("Algaes: %ld, Substrate: %ld\n", algaes.size(), substrate.size());
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

        const float newWaterLevel = std::min(
            aquariumIt.environment.waterLevel - NORMAL_WATER_DROPLET_VALUE,
            aquariumIt.environment.waterLevel
                - (environment.maxWaterLevel * WATER_DROPLET_VALUE_FACTOR)
        );
        aquariumIt.environment.waterLevel = std::max(0.0f, newWaterLevel);
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
    DrawRectangleV({0, 0}, environment.size, { 200, 200, 200, 150 });

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
        DrawCircle(algaeIt.pos.x, algaeIt.pos.y, algaeIt.radius, algaeIt.color); 
    }

    for(const Substrate & substrateIt : substrate){
        DrawCircle(substrateIt.pos.x, substrateIt.pos.y, substrateIt.radius, substrateIt.color);
    }

    for(const Ostracod & ostracodIt : ostracods){
        //DrawCircleLines(ostracod.pos.x, ostracod.pos.y, ostracod.vision, BLACK); 
        DrawCircle(ostracodIt.pos.x, ostracodIt.pos.y, ostracodIt.radius, Ostracod::COLOR); 
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

void Aquarium::draw(bool drawSectors) const {
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
        environment.waterLevel / environment.size.x, Environment::WATER_COLOR
    ); 

    if(drawSectors){
        for(const Sector & sector : sectorController.sectors){
            DrawRectangleLines(sector.position.x, sector.position.y, sector.SIZE.x, sector.SIZE.y,
                RED
            );
            DrawText(std::to_string(sector.algaeCount).c_str(), sector.position.x,
                sector.position.y, 20, RED
            );
        }
    }

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
    sectorController.updateCoverage(environment.size);
}

bool Aquarium::isExpansionDisabled() const {
    return substrate.size() > 0 || plants.size() > 0;
}

void Aquarium::saveToFile(std::ofstream & file) const {
    //Environment
    writeToBinary(file, environment.position);
    writeToBinary(file, environment.size);
    writeToBinary(file, environment.waterLevel);
    writeToBinary(file, environment.maxWaterLevel);

    //Sectors and their controller
    writeToBinary(file, sectorController.size);
    writeToBinary(file, sectorController.rows);
    writeToBinary(file, sectorController.columns);
    writeToBinary(file, sectorController.sectors.size());
    for(const Sector & sectorIt : sectorController.sectors){
        writeToBinary(file, sectorIt.position);
        writeToBinary(file, sectorIt.column);
        writeToBinary(file, sectorIt.row);
        writeToBinary(file, sectorIt.algaeCount);
        writeToBinary(file, sectorIt.neighbourIndexes.size());
        for(const size_t neighbourIdx : sectorIt.neighbourIndexes){
            writeToBinary(file, neighbourIdx);
        }
    }

    //Water droplets
    writeToBinary(file, waterDroplets.size());
    for(const WaterDroplet & it : waterDroplets){
        it.saveToFile(file);
    }

    //Algaes
    writeToBinary(file, algaes.size());
    for(const Algae & it : algaes){
        it.saveToFile(file);
        writeToBinary(file, static_cast<int>(it.type));
        writeToBinary(file, it.timeToReproduce);
        writeToBinary(file, it.lifespan);
        writeToBinary(file, it.dormantTimer);
        writeToBinary(file, it.isPlayerMade);
        writeToBinary(file, it.sectorIdx);
    }

    //Substrate
    writeToBinary(file, substrate.size());
    for(const Substrate & it : substrate){
        it.saveToFile(file);
        writeToBinary(file, static_cast<int>(it.type));
    }

    //Ostracods
    writeToBinary(file, ostracods.size());
    for(const Ostracod & it : ostracods){
        it.saveToFile(file);
        writeToBinary(file, it.alive);
        writeToBinary(file, it.saturation);
        writeToBinary(file, it.reactionTime);
        writeToBinary(file, it.reactionCooldown);
        writeToBinary(file, it.eatingCooldown);
        writeToBinary(file, it.vision);
        writeToBinary(file, it.speed);
    }

    //Plants
    writeToBinary(file, plants.size());
    for(const Plant & it : plants){
        it.saveToFile(file);

        //Dna - TODO: PLEASE MOVE IT TO WHOLE PLANT STRUCT
        writeToBinary(file, it.dna.growthTime);
        writeToBinary(file, it.dna.growthSpeed);
        writeToBinary(file, it.dna.stemMaxLength);
        writeToBinary(file, it.dna.leafMaxLength);
        writeToBinary(file, it.dna.rootMaxLength);
        writeToBinary(file, it.dna.flowerMaxLength);
        writeToBinary(file, it.dna.numberOfStemBranches);
        writeToBinary(file, it.dna.numberOfRootBranches);
        writeToBinary(file, it.dna.stemMaxLevel);
        writeToBinary(file, it.dna.rootMaxLevel);
        writeToBinary(file, it.dna.leafMaxLevel);
        writeToBinary(file, it.dna.stemGrowthRate);
        writeToBinary(file, it.dna.rootGrowthRate);
        writeToBinary(file, it.dna.stemBranchingAngle);
        writeToBinary(file, it.dna.rootBranchingAngle);
        writeToBinary(file, it.dna.leafBranchingAngle);
        writeToBinary(file, it.dna.stemShrinkage);
        writeToBinary(file, it.dna.rootShrinkage);
        writeToBinary(file, it.dna.rootBranchingChance);
        writeToBinary(file, it.dna.distanceBetweenLeaves);
        writeToBinary(file, it.dna.maxWaterStored);
        writeToBinary(file, it.dna.waterConsumption);
        writeToBinary(file, it.dna.minWaterToGrow);
        writeToBinary(file, it.dna.minWaterToDivide);
        writeToBinary(file, it.dna.rootWaterIntake);
        writeToBinary(file, it.dna.distanceBetweenLeavesAtLevel.size());
        for(float distance : it.dna.distanceBetweenLeavesAtLevel)
            writeToBinary(file, distance);

        //Plant
        writeToBinary(file, static_cast<int>(it.type));
        writeToBinary(file, it.plantIdx);
        writeToBinary(file, it.plantPartIdx);
        writeToBinary(file, it.parentPartIdx);
        writeToBinary(file, static_cast<int>(it.growthDecision));
        writeToBinary(file, it.currentLevel);
        writeToBinary(file, it.leafNodesCounter);
        writeToBinary(file, it.waterStored);
        writeToBinary(file, it.dead);

        //Substrate info
        writeToBinary(file, it.substrateInfo.sandCount);
        writeToBinary(file, it.substrateInfo.gravelCount);
        writeToBinary(file, it.substrateInfo.soilCount);
        writeToBinary(file, it.substrateInfo.deadMatterCount);
    }

    //Whole plants
    writeToBinary(file, wholePlants.size());
    for(const WholePlantData & it : wholePlants){
        writeToBinary(file, it.plantPartsCount);
        writeToBinary(file, it.touchedSand);
        writeToBinary(file, it.touchedGravel);
        writeToBinary(file, it.touchedGravel);
    }
}

void Aquarium::loadFromFile(std::ifstream & file){
    //Environment
    readFromBinary(file, environment.position);
    readFromBinary(file, environment.size);
    readFromBinary(file, environment.waterLevel);
    readFromBinary(file, environment.maxWaterLevel);

    //Sectors and their controller
    readFromBinary(file, sectorController.size);
    readFromBinary(file, sectorController.rows);
    readFromBinary(file, sectorController.columns);
    size_t vectorSize = 0;
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        sectorController.sectors.emplace_back();
        readFromBinary(file, sectorController.sectors.back().position);
        readFromBinary(file, sectorController.sectors.back().column);
        readFromBinary(file, sectorController.sectors.back().row);
        readFromBinary(file, sectorController.sectors.back().algaeCount);
        size_t neighbourSize = 0;
        readFromBinary(file, neighbourSize);
        for(size_t neighbourIdx = 0; neighbourIdx < neighbourSize; ++neighbourIdx){
            sectorController.sectors.back().neighbourIndexes.push_back(0);
            readFromBinary(file, sectorController.sectors.back().neighbourIndexes.back());
        }
    }

    //Water droplets
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        waterDroplets.emplace_back(WaterDroplet());
        waterDroplets.back().loadFromFile(file);
    }

    //Algaes
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        algaes.emplace_back(AlgaeType::floating_algae);
        algaes.back().loadFromFile(file);
        readFromBinary(file, algaes.back().type);
        readFromBinary(file, algaes.back().timeToReproduce);
        readFromBinary(file, algaes.back().lifespan);
        readFromBinary(file, algaes.back().dormantTimer);
        readFromBinary(file, algaes.back().isPlayerMade);
        readFromBinary(file, algaes.back().sectorIdx);
    }

    //Substrate
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        substrate.emplace_back(Substrate());
        substrate.back().loadFromFile(file);
        readFromBinary(file, substrate.back().type);
    }

    //Ostracods
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        ostracods.emplace_back(Ostracod());
        ostracods.back().loadFromFile(file);
        readFromBinary(file, ostracods.back().alive);
        readFromBinary(file, ostracods.back().saturation);
        readFromBinary(file, ostracods.back().reactionTime);
        readFromBinary(file, ostracods.back().reactionCooldown);
        readFromBinary(file, ostracods.back().eatingCooldown);
        readFromBinary(file, ostracods.back().vision);
        readFromBinary(file, ostracods.back().speed);
    }

    //Plants
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        plants.emplace_back(Plant());
        plants.back().loadFromFile(file);

        //Dna - TODO: PLEASE MOVE IT TO WHOLE PLANT STRUCT
        readFromBinary(file, plants.back().dna.growthTime);
        readFromBinary(file, plants.back().dna.growthSpeed);
        readFromBinary(file, plants.back().dna.stemMaxLength);
        readFromBinary(file, plants.back().dna.leafMaxLength);
        readFromBinary(file, plants.back().dna.rootMaxLength);
        readFromBinary(file, plants.back().dna.flowerMaxLength);
        readFromBinary(file, plants.back().dna.numberOfStemBranches);
        readFromBinary(file, plants.back().dna.numberOfRootBranches);
        readFromBinary(file, plants.back().dna.stemMaxLevel);
        readFromBinary(file, plants.back().dna.rootMaxLevel);
        readFromBinary(file, plants.back().dna.leafMaxLevel);
        readFromBinary(file, plants.back().dna.stemGrowthRate);
        readFromBinary(file, plants.back().dna.rootGrowthRate);
        readFromBinary(file, plants.back().dna.stemBranchingAngle);
        readFromBinary(file, plants.back().dna.rootBranchingAngle);
        readFromBinary(file, plants.back().dna.leafBranchingAngle);
        readFromBinary(file, plants.back().dna.stemShrinkage);
        readFromBinary(file, plants.back().dna.rootShrinkage);
        readFromBinary(file, plants.back().dna.rootBranchingChance);
        readFromBinary(file, plants.back().dna.distanceBetweenLeaves);
        readFromBinary(file, plants.back().dna.maxWaterStored);
        readFromBinary(file, plants.back().dna.waterConsumption);
        readFromBinary(file, plants.back().dna.minWaterToGrow);
        readFromBinary(file, plants.back().dna.minWaterToDivide);
        readFromBinary(file, plants.back().dna.rootWaterIntake);
        size_t nestedVectorSize = 0;
        readFromBinary(file, nestedVectorSize);
        for(size_t idx = 0; idx < nestedVectorSize; ++idx){
            plants.back().dna.distanceBetweenLeavesAtLevel.emplace_back();
            readFromBinary(file, plants.back().dna.distanceBetweenLeavesAtLevel.back());
        }   

        //Plant
        readFromBinary(file, plants.back().type);
        readFromBinary(file, plants.back().plantIdx);
        readFromBinary(file, plants.back().plantPartIdx);
        readFromBinary(file, plants.back().parentPartIdx);
        readFromBinary(file, plants.back().growthDecision);
        readFromBinary(file, plants.back().currentLevel);
        readFromBinary(file, plants.back().leafNodesCounter);
        readFromBinary(file, plants.back().waterStored);
        readFromBinary(file, plants.back().dead);

        //Substrate info
        readFromBinary(file, plants.back().substrateInfo.sandCount);
        readFromBinary(file, plants.back().substrateInfo.gravelCount);
        readFromBinary(file, plants.back().substrateInfo.soilCount);
        readFromBinary(file, plants.back().substrateInfo.deadMatterCount);
    }

    //Whole plants
    readFromBinary(file, vectorSize);
    for(size_t idx = 0; idx < vectorSize; ++idx){
        wholePlants.emplace_back(WholePlantData());
        readFromBinary(file, wholePlants.back().plantPartsCount);
        readFromBinary(file, wholePlants.back().touchedSand);
        readFromBinary(file, wholePlants.back().touchedGravel);
        readFromBinary(file, wholePlants.back().touchedGravel);
    }
}