#include "plants.hpp"

inline float getMaxStemLengthForLevel(float stemMaxLength, float stemShrinkageRate,
    int currentlevel
){
    return stemMaxLength - currentlevel * stemShrinkageRate;
}

void PlantDna::randomize(){
    growthTime = randomBetween(SEED_GROWTH_TIME);
    growthSpeed = randomBetween(SEED_GROWTH_SPEED);
    stemMaxLength = randomBetween(MAX_STEM_LENGTH);
    leafMaxLength = randomBetween(MAX_LEAF_LENGTH);
    rootMaxLength = randomBetween(MAX_ROOT_LENGTH);
    flowerMaxLength = randomBetween(MAX_FLOWER_LENGTH);
    numberOfStemBranches = {
        randomBetween(STEM_BRANCHES_RANGE.min),
        randomBetween(STEM_BRANCHES_RANGE.max)
    };
    numberOfRootBranches = {
        randomBetween(ROOT_BRANCHES_RANGE.min),
        randomBetween(ROOT_BRANCHES_RANGE.max)
    };
    stemMaxLevel = randomBetween(MAX_STEM_LEVEL);
    rootMaxLevel = randomBetween(MAX_ROOT_LEVEL);
    leafMaxLevel = randomBetween(MAX_LEAF_LEVEL);
    stemGrowthRate = randomBetween(STEM_GROWTH_RATE);
    rootGrowthRate = randomBetween(ROOT_GROWTH_RATE);
    stemBranchingAngle = randomBetween(STEM_BRANCHING_ANGLE);
    rootBranchingAngle = randomBetween(ROOT_BRANCHING_ANGLE);
    leafBranchingAngle = randomBetween(LEAF_BRANCHING_ANGLE);
    stemShrinkage = randomBetween(STEM_SHRINKAGE);
    rootShrinkage = randomBetween(ROOT_SHRINKAGE);
    rootBranchingChance = randomBetween(ROOT_BRANCHING_CHANCE);
    maxWaterStored = randomBetween(MAX_WATER_STORED);
    waterConsumption = randomBetween(WATER_CONSUMPTION);
    minWaterToGrow = randomBetween(MIN_WATER_TO_GROW);
    minWaterToDivide = randomBetween(MIN_WATER_TO_DIVIDE);
    rootWaterIntake = randomBetween(ROOT_WATER_INTAKE);
    for(int level = 0; level < stemMaxLevel && level < LEAVES_PER_STEM_LEVEL_SIZE; ++level){
        int leavesNumber = randomBetween(LEAVES_PER_STEM_LEVEL[level]);
        if(leavesNumber > 0)
            distanceBetweenLeavesAtLevel.push_back(
                getMaxStemLengthForLevel(stemMaxLength, stemShrinkage, level) / leavesNumber
            );
        else
            distanceBetweenLeavesAtLevel.push_back(0.0f);
    }
}

Vector2 getBranchOutVelocity(int numberOfBranches, int branchIndex, bool up){
    float theta = (PI * branchIndex) / (numberOfBranches + 1);

    Vector2 velocity = angleToVector(theta);

    if(up)
        velocity.y *= -1;

    return velocity;
}

Plant::Plant(){
    active = true;
    pos = VEC2(0.0f, 0.0f);
    velocity = VEC2(0.0f, 0.0f);

    dead = false;

    length = 0.0f;
    isMoving = false;
    time = 0;
    plantPartIdx = 0;
    parentPartIdx = 0;

    growthDecision = GrowthDecision::doNothing;
    currentLevel = 0;
    leafNodesCounter = 0;

    waterStored = 0.0f;
}

void Plant::initSeed(Vector2 newPos, size_t newPartIdx, size_t newPlantIdx){
    pos = newPos;
    radius = randomBetween(Plant::SEED_SIZE_RANGE.x, Plant::SEED_SIZE_RANGE.y); 

    dna.randomize();
    
    isMoving = true;
    time = dna.growthTime;

    type = PlantPartType::seed;
    plantIdx = newPlantIdx;
    plantPartIdx = newPartIdx;
    color = LIME;

    waterStored = SEED_INITIAL_WATER;
}

Plant::Plant(PlantPartType plantPart, Plant parent, size_t newPartIdx, Vector2 newVelocity,
    bool justExtend
){
    Plant();
    pos = parent.pos;

    dna = parent.dna;
    isMoving = false;
    
    type = plantPart;
    plantIdx = parent.plantIdx;
    plantPartIdx = newPartIdx;
    parentPartIdx = parent.plantPartIdx;

    currentLevel = parent.currentLevel;
    if(!justExtend)
        ++currentLevel;

    velocity = newVelocity;

    waterStored = PLANT_PART_INITIAL_WATER;
    
    switch (type){
        case PlantPartType::stem:
            if(parent.type == PlantPartType::stem)
                color = parent.color;
            else
                color = randomBetween(PlantDna::STEM_COLOR_RANGE);
            radius = randomBetween(Plant::STEM_RADIUS_RANGE.x, Plant::STEM_RADIUS_RANGE.y);
            if(justExtend){
                length = parent.length;
                leafNodesCounter = parent.leafNodesCounter;
            } 
            return;
        case PlantPartType::root:
            if(parent.type == PlantPartType::leaf)
                color = parent.color;
            else
                color = randomBetween(PlantDna::ROOT_COLOR_RANGE);
            radius = randomBetween(Plant::STEM_RADIUS_RANGE.x, Plant::STEM_RADIUS_RANGE.y);
            return;
        case PlantPartType::leaf:
            if(parent.type == PlantPartType::leaf)
                color = parent.color;
            else
                color = randomBetween(PlantDna::LEAF_COLOR_RANGE);
            radius = randomBetween(Plant::STEM_RADIUS_RANGE.x, Plant::STEM_RADIUS_RANGE.y);
            if(parent.type == PlantPartType::stem)
                currentLevel = 0;
            return;
        default:
            return;
    }
}

void Plant::moveSeed(Environment & environment, vector<Substrate> & substrate){
    bool isUnderwater = checkIfUnderwater(environment);
    applyGravityAndBuoyancy(environment, isUnderwater, radius / Plant::SEED_SIZE_RANGE.y);

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    //Check simple collisions between falling (active) and settled substrate particles
    SimpleCollisionType collisionType = Substrate::checkFallingCollision(*this, substrate);
    switch (collisionType){
        case SimpleCollisionType::full:
            velocity.x = 0.0;
            velocity.y = 0.0;
            isMoving = false;
            return;
        case SimpleCollisionType::left:
            pos.x += 1.0f;
            break;
        case SimpleCollisionType::right:
            pos.x -= 1.0f;
            break;
        default:
            break;
    }

    detectCollisionWithAquarium(nextPosition, environment);

    pos.x += velocity.x;
    pos.y += velocity.y;

    if(velocity.x == 0.0f && velocity.y == 0.0f)
        isMoving = false;
}

void Plant::growSeed(Environment & environment){
    time -= dna.growthSpeed;
    if(time > 0){
        return;
    }
    active = false;
    growthDecision = GrowthDecision::growStemsAndRootsFromSeed;
}

void Plant::updateSeed(Environment & environment, vector<Substrate> & substrate){
    if(isMoving)
        moveSeed(environment, substrate); 
    else if(active && environment.waterLevel > 0)
        growSeed(environment);
}

bool Plant::commonUpdate(Environment & environment, WholePlantData * wholePlantData,
    float waterConsumption
){
    if(dead)
        return false;

    //Basic water consumption / living
    waterStored -= waterConsumption;
    if(wholePlantData != nullptr)
        wholePlantData->waterConsumedNow += waterConsumption;
    if(waterStored <= 0){
        dead = true;
        active = false;
        color = DARKBROWN;
        return false;
    }

    //Finished growing
    if(!active)
        return false;
    
    //Do nothing if there's too little stored water
    if(waterStored <= dna.minWaterToGrow)
        return false;

    //If a plant part collides with aquarium, deactivate growth
    if(detectCollisionWithAquarium(pos + velocity, environment, false, false)){
        active = false;
        return false;
    }

    //Grow
    waterStored -= waterConsumption;
    if(wholePlantData != nullptr)
        wholePlantData->waterConsumedNow += waterConsumption;
    pos.x += velocity.x;
    pos.y += velocity.y;
    length += getVectorMagnitude(velocity);
    return true;
}

void Plant::updateStem(Environment & environment, WholePlantData * wholePlantData){
    if(!commonUpdate(environment, wholePlantData, dna.waterConsumption))
        return;
    if(length >= getMaxStemLengthForLevel(dna.stemMaxLength, dna.stemShrinkage, currentLevel)){
        if(currentLevel < dna.stemMaxLevel)
            growthDecision = GrowthDecision::growBranches;
        else
            growthDecision = GrowthDecision::growLeavesFromStem;
        active = false;
    }
    else if((size_t)currentLevel < dna.distanceBetweenLeavesAtLevel.size()
        && dna.distanceBetweenLeavesAtLevel[currentLevel] > 0
        && length >= dna.distanceBetweenLeavesAtLevel[currentLevel] * (leafNodesCounter + 1)
    ){
        growthDecision = GrowthDecision::growLeavesAndExtendStem;
        ++leafNodesCounter;
        active = false;
    }
}

void Plant::updateLeaf(Environment & environment, WholePlantData * wholePlantData){
    if(!commonUpdate(environment, wholePlantData, dna.waterConsumption))
        return;
    if(length >= dna.leafMaxLength - currentLevel * dna.stemShrinkage){
        if(currentLevel < dna.leafMaxLevel)
            growthDecision = GrowthDecision::extendLeaf;
        active = false;
    }
}

void Plant::updateRoot(Environment & environment, vector<Substrate> & substrate,
    WholePlantData * wholePlantData
){
    //Absorb water from the substrate
    if(!dead && !active && environment.waterLevel > 0){
        float absorbedWater = dna.rootWaterIntake * (
            substrateInfo.sandCount * WATER_FROM_SAND
            + substrateInfo.gravelCount * WATER_FROM_GRAVEL
            + substrateInfo.soilCount * WATER_FROM_SOIL
        );
        absorbedWater = std::min(environment.waterLevel, absorbedWater);
        absorbedWater = std::min(dna.maxWaterStored - waterStored, absorbedWater);
        if(absorbedWater > 0){
            environment.waterLevel -= absorbedWater;
            waterStored += absorbedWater;
        }
        waterStored = std::min(waterStored, dna.maxWaterStored);
        if(wholePlantData != nullptr)
            wholePlantData->waterAbsorbedNow += absorbedWater;
    }

    if(!commonUpdate(environment, wholePlantData, dna.waterConsumption))
        return;

    //Prepare to grow more roots and measure touched soil
    if(length >= dna.rootMaxLength - currentLevel * dna.rootShrinkage){
        if(currentLevel == 1 || (rand() % 101 <= dna.rootBranchingChance * 100
            && currentLevel < dna.rootMaxLevel
        ))
            growthDecision = GrowthDecision::growRootBranches;
        active = false;

        //Get substrate info
        for(const Substrate & particle : substrate){
            if(getDistance(pos, particle.pos) <= ROOT_DRINKING_RANGE){
                switch (particle.type){
                    case SubstrateType::sand:
                        ++substrateInfo.sandCount;
                        break;
                    case SubstrateType::gravel:
                        ++substrateInfo.gravelCount;
                        break;
                    case SubstrateType::soil:
                        ++substrateInfo.soilCount;
                        break;
                    default:
                        break;
                }
            }
        }
        if(wholePlantData != nullptr){
            wholePlantData->touchedSand += substrateInfo.sandCount;
            wholePlantData->touchedGravel += substrateInfo.gravelCount;
            wholePlantData->touchedSoil += substrateInfo.soilCount;
        }
    }
}

void Plant::update(Environment & environment, vector<Substrate> & substrate, Plant * parentPart,
    WholePlantData * wholePlantData
){
    if(parentPart != nullptr){
        float waterDifference = (parentPart->waterStored - waterStored) / 2.0f;
        waterStored += waterDifference;
        parentPart->waterStored -= waterDifference;
    }
    switch (type){
        case PlantPartType::seed:
            updateSeed(environment, substrate);
            break;
        case PlantPartType::stem:
            updateStem(environment, wholePlantData);
            break;
        case PlantPartType::leaf:
            updateLeaf(environment, wholePlantData);
            break;
        case PlantPartType::root:
            updateRoot(environment, substrate, wholePlantData);
            break;
        default:
            break;
    }
    if(wholePlantData != nullptr)
        wholePlantData->waterStoredNow += waterStored;
}