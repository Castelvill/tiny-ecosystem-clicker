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
    constexpr static Vector2 STEM_MAX_LENGTH = VEC2(40.0f, 80.0f);
    constexpr static Vector2 LEAF_MAX_LENGTH = VEC2(40.0f, 60.0f);
    constexpr static Vector2 ROOT_MAX_LENGTH = VEC2(30.0f, 60.0f);
    constexpr static Vector2 FLOWER_MAX_LENGTH = VEC2(10.0f, 20.0f);
    constexpr static MinMaxIntRange STEM_BRANCHES_RANGE = {{1, 1}, {3, 3}};
    constexpr static MinMaxIntRange ROOT_BRANCHES_RANGE = {{1, 1}, {3, 3}};
    constexpr static MinMaxIntRange LEAFS_PER_STEM_RANGE = {{1, 1}, {2, 2}};
    constexpr static IntRange MAX_STEM_LEVEL = {3, 4};
    constexpr static IntRange MAX_ROOT_LEVEL = {3, 4};
    constexpr static Vector2 STEM_GROWTH_RATE = VEC2(0.1, 0.1);
    constexpr static Vector2 ROOT_GROWTH_RATE = VEC2(0.1, 0.1);
public:
    int growthTime;
    int growthSpeed;
    float stemMaxLength;
    float leafMaxLength;
    float rootMaxLength;
    float flowerMaxLength;
    IntRange numberOfStemBranches;
    IntRange numberOfRootBranches;
    IntRange numberOfLeafsPerStem;
    int maxStemLevel;
    int maxRootLevel;
    float stemGrowthRate;
    float rootGrowthRate;

    void randomize();
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
    size_t plantIdx;
    size_t parentIdx;
    bool growNewParts = false;
    int currentLevel = 0;

    Plant();
    Plant(PlantPartType plantPart, Plant parent, size_t newIdx, int maxBranches, int branchIdx);
    void initSeed(Vector2 newPos, size_t newIdx);
    void moveSeed(Environment & environment, vector<Substrate> & substrate);
    void growSeed();
    void updateSeed(Environment & environment, vector<Substrate> & substrate);
    void updateStem(Environment & environment);
    void updateRoot(Environment & environment);
    void update(Environment & environment, vector<Substrate> & substrate);
};