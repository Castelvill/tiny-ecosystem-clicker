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

    length = 0.0f;
    isMoving = false;
    time = 0;
    plantIdx = 0;
    parentIdx = 0;

    growthDecision = GrowthDecision::doNothing;
    currentLevel = 0;
    leafNodesCounter = 0;
}

void Plant::initSeed(Vector2 newPos, size_t newIdx){
    pos = newPos;
    radius = randomBetween(Plant::SEED_SIZE_RANGE.x, Plant::SEED_SIZE_RANGE.y); 

    dna.randomize();
    
    isMoving = true;
    time = dna.growthTime;

    type = PlantPartType::seed;
    plantIdx = newIdx;
    color = LIME;
}

Plant::Plant(PlantPartType plantPart, Plant parent, size_t newIdx, Vector2 newVelocity,
    bool justExtend
){
    Plant();
    pos = parent.pos;

    dna = parent.dna;
    isMoving = false;
    
    type = plantPart;
    plantIdx = newIdx;
    parentIdx = parent.plantIdx;

    currentLevel = parent.currentLevel;
    if(!justExtend)
        ++currentLevel;

    velocity = newVelocity;
    
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

void Plant::growSeed(){
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
    else
        growSeed();
}

void Plant::commonUpdate(Environment & environment){
    if(detectCollisionWithAquarium(pos + velocity, environment, false, false)){
        active = false;
        return;
    }
    pos.x += velocity.x;
    pos.y += velocity.y;
    length += getVectorMagnitude(velocity);
}

void Plant::updateStem(Environment & environment){
    commonUpdate(environment);
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

void Plant::updateLeaf(Environment & environment){
    commonUpdate(environment);
    if(length >= dna.leafMaxLength - currentLevel * dna.stemShrinkage){
        if(currentLevel < dna.leafMaxLevel)
            growthDecision = GrowthDecision::extendLeaf;
        active = false;
    }
}

void Plant::updateRoot(Environment & environment){
    commonUpdate(environment);
    if(length >= dna.rootMaxLength - currentLevel * dna.rootShrinkage){
        if(currentLevel == 1 || (rand() % 101 <= dna.rootBranchingChance * 100
            && currentLevel < dna.rootMaxLevel
        ))
            growthDecision = GrowthDecision::growRootBranches;
        active = false;
    }
}

void Plant::update(Environment & environment, vector<Substrate> & substrate){
    if(!active)
        return;
    switch (type){
        case PlantPartType::seed:
            updateSeed(environment, substrate);
            break;
        case PlantPartType::stem:
            updateStem(environment);
            break;
        case PlantPartType::leaf:
            updateLeaf(environment);
            break;
        case PlantPartType::root:
            updateRoot(environment);
            break;
        default:
            break;
    }

    //printf("idx: %ld, pos.x: %lf, pos.y: %lf, length: %lf\n", plantIdx, pos.x, pos.y, length);
}