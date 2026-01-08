#include "plants.hpp"

void PlantDna::randomize(){
    growthTime = randomBetween(SEED_GROWTH_TIME);
    growthSpeed = randomBetween(SEED_GROWTH_SPEED);
    stemMaxLength = randomBetween(STEM_MAX_LENGTH);
    leafMaxLength = randomBetween(LEAF_MAX_LENGTH);
    rootMaxLength = randomBetween(ROOT_MAX_LENGTH);
    flowerMaxLength = randomBetween(FLOWER_MAX_LENGTH);
    numberOfStemBranches = {
        randomBetween(STEM_BRANCHES_RANGE.min),
        randomBetween(STEM_BRANCHES_RANGE.max)
    };
    numberOfRootBranches = {
        randomBetween(ROOT_BRANCHES_RANGE.min),
        randomBetween(ROOT_BRANCHES_RANGE.max)
    };
    numberOfLeafsPerStem = {
        randomBetween(LEAFS_PER_STEM_RANGE.min),
        randomBetween(LEAFS_PER_STEM_RANGE.max)
    };
    maxStemLevel = randomBetween(MAX_STEM_LEVEL);
    maxRootLevel = randomBetween(MAX_ROOT_LEVEL);
    stemGrowthRate = randomBetween(STEM_GROWTH_RATE);
    rootGrowthRate = randomBetween(ROOT_GROWTH_RATE);
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

    growNewParts = false;
    currentLevel = 0;
}

void Plant::initSeed(Vector2 newPos, size_t newIdx){
    pos = newPos;
    radius = randomBetween(Plant::SEED_SIZE_RANGE.x, Plant::SEED_SIZE_RANGE.y); 

    dna.randomize();
    
    isMoving = true;
    time = dna.growthTime;

    type = PlantPartType::seed;
    plantIdx = newIdx;
}

Plant::Plant(PlantPartType plantPart, Plant parent, size_t newIdx, int maxBranches, int branchIdx){
    Plant();
    pos = parent.pos;

    dna = parent.dna;
    isMoving = false;
    
    type = plantPart;
    plantIdx = newIdx;
    parentIdx = parent.plantIdx;
    currentLevel = parent.currentLevel + 1;
    
    switch (type){
        case PlantPartType::stem:
            radius = randomBetween(Plant::STEM_RADIUS_RANGE.x, Plant::STEM_RADIUS_RANGE.y);
            velocity = getBranchOutVelocity(maxBranches, branchIdx, true) * dna.stemGrowthRate;
            return;
        case PlantPartType::root:
            radius = randomBetween(Plant::STEM_RADIUS_RANGE.x, Plant::STEM_RADIUS_RANGE.y);
            velocity = getBranchOutVelocity(maxBranches, branchIdx, false) * dna.rootGrowthRate;
            return;
        default:
            return;
    }
}

void Plant::moveSeed(Environment & environment){
    bool isUnderwater = checkIfUnderwater(environment);
    applyGravityAndBuoyancy(environment, isUnderwater, radius / Plant::SEED_SIZE_RANGE.y);

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    detectCollisionWithAquarium(nextPosition);

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
    growNewParts = true;
}

void Plant::updateSeed(Environment & environment){
    if(isMoving)
        moveSeed(environment);
    else
        growSeed();
}

void Plant::updateStem(Environment & environment){
    pos.x += velocity.x;
    pos.y += velocity.y;
    length += getVectorMagnitude(velocity);
    if(length >= dna.stemMaxLength / currentLevel){
        if(currentLevel < dna.maxStemLevel)
            growNewParts = true;
        active = false;
    }
}

void Plant::updateRoot(Environment & environment){
    pos.x += velocity.x;
    pos.y += velocity.y;
    length += getVectorMagnitude(velocity);
    if(length >= dna.rootMaxLength / currentLevel){
        if(currentLevel < dna.maxRootLevel)
            growNewParts = true;
        active = false;
    }
}

void Plant::update(Environment & environment, vector<Substrate> & substrate){
    if(!active)
        return;
    switch (type){
        case PlantPartType::seed:
            updateSeed(environment);
            break;
        case PlantPartType::stem:
            updateStem(environment);
            break;
        case PlantPartType::root:
            updateRoot(environment);
            break;
        default:
            break;
    }

    //printf("idx: %ld, pos.x: %lf, pos.y: %lf, length: %lf\n", plantIdx, pos.x, pos.y, length);
}