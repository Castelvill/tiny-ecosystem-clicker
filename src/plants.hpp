#include "entities.hpp"

enum class PlantPartType{
    //Falls to the bottom of the aquarium or floats to the surface. After some time seeds can grow into plants.
    seed,
    //Grows upwards. Can split into branches and grow leafs.
    stem,
    //Grows to its max size and stays that way.
    leaf,
    //Grows downwards. Can branch into many other roots. Stops growing longer when touches the soil.
    root,
    //Grows to its max size and looks pretty.
    flower
};

class PlantDna{
    constexpr static IntRange SEED_GROWTH_TIME = {60, 80};
    constexpr static IntRange SEED_GROWTH_SPEED = {2, 2};
    constexpr static Vector2 MAX_STEM_LENGTH = VEC2(40.0f, 60.0f);
    constexpr static Vector2 MAX_LEAF_LENGTH = VEC2(20.0f, 30.0f);
    constexpr static Vector2 MAX_ROOT_LENGTH = VEC2(15.0f, 35.0f);
    constexpr static Vector2 MAX_FLOWER_LENGTH = VEC2(10.0f, 20.0f);
    constexpr static MinMaxIntRange STEM_BRANCHES_RANGE = {{1, 1}, {1, 2}};
    constexpr static MinMaxIntRange ROOT_BRANCHES_RANGE = {{2, 2}, {3, 3}};
    constexpr static IntRange MAX_STEM_LEVEL = {3, 4};
    constexpr static IntRange MAX_ROOT_LEVEL = {3, 4}; //4-6
    constexpr static IntRange MAX_LEAF_LEVEL = {1, 4};
    constexpr static Vector2 STEM_GROWTH_RATE = VEC2(0.1, 0.1);
    constexpr static Vector2 ROOT_GROWTH_RATE = VEC2(0.1, 0.1);
    constexpr static Vector2 STEM_BRANCHING_ANGLE = {0.17f+0.10f, 0.33f+0.10f};
    constexpr static Vector2 ROOT_BRANCHING_ANGLE = {0.17f+0.20f, 0.33f+0.30f};
    constexpr static Vector2 LEAF_BRANCHING_ANGLE = {0.17f+0.20f, 0.33f+0.20f};
    constexpr static Vector2 STEM_SHRINKAGE = {0.01f, 0.1f};
    constexpr static Vector2 ROOT_SHRINKAGE = {0.02f, 0.2f};
    constexpr static Vector2 ROOT_BRANCHING_CHANCE = {0.6f, 0.8f}; //Range: 0-1
    constexpr static IntRange LEAVES_PER_STEM_LEVEL[] = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}
    };
    constexpr static int LEAVES_PER_STEM_LEVEL_SIZE = sizeof(LEAVES_PER_STEM_LEVEL)
        / sizeof(IntRange);
public:
    constexpr static ColorRange STEM_COLOR_RANGE = {{0, 128, 0, 255}, {70, 230, 48, 255}};
    constexpr static ColorRange ROOT_COLOR_RANGE = {{253, 249, 0, 255}, {253, 249, 0, 255}};
    constexpr static ColorRange LEAF_COLOR_RANGE = {{0, 140, 47, 255}, {20, 255, 77, 255}};
    int growthTime;
    int growthSpeed;
    float stemMaxLength;
    float leafMaxLength;
    float rootMaxLength;
    float flowerMaxLength;
    IntRange numberOfStemBranches;
    IntRange numberOfRootBranches;
    int stemMaxLevel;
    int rootMaxLevel;
    int leafMaxLevel;
    float stemGrowthRate;
    float rootGrowthRate;
    float stemBranchingAngle;
    float rootBranchingAngle;
    float leafBranchingAngle;
    float stemShrinkage;
    float rootShrinkage;
    float rootBranchingChance;
    float distanceBetweenLeaves;
    vector<float> distanceBetweenLeavesAtLevel;

    void randomize();
};

enum class GrowthDecision{
    doNothing,
    growStemsAndRootsFromSeed,
    growBranches,
    growLeavesFromStem,
    extendLeaf,
    growRootBranches,
    growLeavesAndExtendStem
};

class Plant: public Entity{
    constexpr static Vector2 SEED_SIZE_RANGE = VEC2(2.0f, 8.0f);
    constexpr static Vector2 STEM_RADIUS_RANGE = VEC2(3.0f, 4.0f);

    //Distance from the parent body part
    float length = 0.0f;
    bool isMoving = true;
    int time = 0;
public:
    PlantDna dna;
    PlantPartType type;
    size_t plantIdx = 0;
    size_t parentIdx = 0;
    GrowthDecision growthDecision = GrowthDecision::doNothing;
    int currentLevel = 0;
    int leafNodesCounter = 0;
    Color color;

    Plant();
    Plant(PlantPartType plantPart, Plant parent, size_t newIdx, Vector2 newVelocity,
        bool copyLength = false
    );
    void initSeed(Vector2 newPos, size_t newIdx);
    void moveSeed(Environment & environment, vector<Substrate> & substrate);
    void growSeed();
    void updateSeed(Environment & environment, vector<Substrate> & substrate);
    void updateStem(Environment & environment);
    void updateLeaf(Environment & environment);
    void updateRoot(Environment & environment);
    void update(Environment & environment, vector<Substrate> & substrate);
};