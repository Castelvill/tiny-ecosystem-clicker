#include "aquarium.hpp"

const std::uint32_t SAVE_FILE_SIGNATURE = ('T' ) | ('I' << 8) | ('N' << 16) | ('Y' << 24);
const std::uint32_t SAVE_FILE_VERSION = 1;


struct AquariumBuilder{
    bool active = false;
    Rectangle exansionRect;
    bool isObstructed = false;
    const Aquarium * aquarium = nullptr;
    Direction direction = Direction::none;
};

struct AquariumMover{
    bool wasAquariumClicked = false;
    Aquarium * aquarium = nullptr;
    Vector2 originalPosition = {0, 0};
    Vector2 startingMouseOffset = {0, 0};
    bool isCollision = false;
    void clear();
};

class GameState{
public:
    const Vector2 NEW_AQUARIUM_SIZE = {50, 50}; 
    const float EXPANSION_SIZE_FOR_MOUSE = 20.0f;
    const float REAL_EXPANSION_SIZE = 20.0f;

    const Color DEFAULT_OVERLAY_COLOR = {255, 255, 255, 150};
    const Color OBSTRUCTED_OVERLAY_COLOR = {230, 41, 55, 150};
    const Color EXPAND_OVERLAY_COLOR = {40, 230, 80, 150};
    const Color ACTIVE_OVERLAY_COLOR = {40, 230, 40, 150};

    //Stores texture data and draws it
    UserInterface gui;
    //Keeps player level info
    PlayerState player;
    //Not persistent. Used only for spawning things and making them fall into real aquariums
    Aquarium gameArea;
    //Data about every created ecosystem
    vector<Aquarium> aquariums;

    //Used by action: build aquarium
    AquariumBuilder aquariumBuilder;
    //Used by action: move aquarium
    AquariumMover aquariumMover;

    bool doesExpansionCollideWithAquariums(const Rectangle & expansionRect) const;
    AquariumBuilder getAquariumBuilder() const;
    bool isGuiBlockingBuilding() const;
    void buildAquarium();

    Aquarium* findAquariumUnderTheMouse();
    void checkForAquariumMoverCollision();
    void moveAquarium();

    void performGlobalActions();
    void update();

    void drawAquariumBuilderOverlay() const;
    void drawAquariumMoverOverlay() const;
    void drawGlobalActionsOverlays() const;
    void draw() const;

    void saveToFile(const string & fileName);
    void loadFromFile(const string & fileName);
};