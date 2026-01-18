#include "aquarium.hpp"
#include "shelf.hpp"

struct AquariumExpansion{
    bool active = false;
    Rectangle exansionRect;
    bool isObstructed = false;
    const Aquarium * aquarium = nullptr;
    Direction direction = Direction::none;
};

struct GameState{
    const Vector2 NEW_AQUARIUM_SIZE = {50, 50}; 
    const float expansionSizeForMouse = 20.0f;
    const float realExpansionSize = 20.0f;

    const Color DEFAULT_SHADOW_COLOR = {255, 255, 255, 150};
    const Color OBSTRUCTED_SHADOW_COLOR = {230, 41, 55, 150};
    const Color EXPAND_SHADOW_COLOR = {0, 228, 48, 150};

    Aquarium gameArea;
    vector<Aquarium> aquariums;
    vector<Shelf> shelves;
    PlayerState player;
    UserInterface gui;
    AquariumExpansion newAquariumExpansion;

    bool doesExpansionCollideWithAquariums(const Rectangle & expansionRect) const;
    AquariumExpansion getAquariumExpansion() const;
    bool isGuiBlockingBuilding() const;
    void buildAquarium();
    void drawAquariumShadow(const AquariumExpansion & expansion) const;
    void draw() const;
};

bool GameState::doesExpansionCollideWithAquariums(const Rectangle & expansionRect) const {
    for(const Aquarium & aquarium : aquariums){
        if(CheckCollisionRecs(expansionRect, aquarium.getRectangle()))
            return true;
    }
    return false;
}

AquariumExpansion GameState::getAquariumExpansion() const {
    AquariumExpansion expansionState;

    expansionState.active = true;

    const Vector2 mousePosition = GetMousePosition();
    const Vector2 shadowPosition = mousePosition - (NEW_AQUARIUM_SIZE / 2); 
    Rectangle shadowCollision = {
        shadowPosition.x, shadowPosition.y,
        NEW_AQUARIUM_SIZE.x, NEW_AQUARIUM_SIZE.y
    };

    expansionState.exansionRect = shadowCollision;

    for(const Aquarium & aquarium : aquariums){
        Rectangle aquariumRect = aquarium.getRectangle();

        if(CheckCollisionRecs(shadowCollision, aquariumRect)){
            expansionState.aquarium = &aquarium;

            //Expand upwards
            Rectangle aquariumRectUp = {
                aquariumRect.x, aquariumRect.y, aquariumRect.width, expansionSizeForMouse
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectUp)){
                aquariumRectUp.y -= realExpansionSize;
                aquariumRectUp.height = realExpansionSize;

                expansionState.direction = Direction::up;
                expansionState.exansionRect = aquariumRectUp;
                if(doesExpansionCollideWithAquariums(aquariumRectUp))
                    expansionState.isObstructed = true;

                return expansionState;
            }

            //Expand to the right
            Rectangle aquariumRectRight = {
                aquariumRect.x + aquariumRect.width - expansionSizeForMouse, aquariumRect.y,
                expansionSizeForMouse, aquariumRect.height
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectRight)){
                aquariumRectRight.x += expansionSizeForMouse;
                aquariumRectRight.width = realExpansionSize;

                expansionState.direction = Direction::right;
                expansionState.exansionRect = aquariumRectRight;
                if(doesExpansionCollideWithAquariums(aquariumRectRight))
                    expansionState.isObstructed = true;

                return expansionState;
            }
            
            //Expand to the left
            Rectangle aquariumRectLeft = {
                aquariumRect.x, aquariumRect.y, expansionSizeForMouse, aquariumRect.height
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectLeft)){
                aquariumRectLeft.x -= realExpansionSize;
                aquariumRectLeft.width = realExpansionSize;

                expansionState.direction = Direction::left;
                expansionState.exansionRect = aquariumRectLeft;
                if(doesExpansionCollideWithAquariums(aquariumRectLeft))
                    expansionState.isObstructed = true;

                return expansionState;
            }

            expansionState.isObstructed = true;
            
            return expansionState;
        }
    }

    return expansionState;
}

bool GameState::isGuiBlockingBuilding() const {
    return GetMousePosition().y - (NEW_AQUARIUM_SIZE.y / 2) <= gui.GUI_HEIGHT;
}

void GameState::buildAquarium(){
    newAquariumExpansion.active = false;

    if(gui.selectedInventorySlotIdx != InventorySlots::slotAquarium)
        return;

    newAquariumExpansion = getAquariumExpansion();

    if(IsMouseButtonPressed(0) && !isGuiBlockingBuilding() && !newAquariumExpansion.isObstructed){
        //Expand existing aquarium
        if(newAquariumExpansion.aquarium != nullptr
            && newAquariumExpansion.aquarium >= aquariums.data()
            && newAquariumExpansion.aquarium < aquariums.data() + aquariums.size()
        ){
            size_t aquariumIdx = newAquariumExpansion.aquarium - aquariums.data();
            aquariums[aquariumIdx].expand(newAquariumExpansion.direction, realExpansionSize);
        }
        else //Or create a new one
            aquariums.push_back(Aquarium(newAquariumExpansion.exansionRect));

        newAquariumExpansion.active = false;
    }
}

void GameState::drawAquariumShadow(const AquariumExpansion & expansion) const {
    if(gui.selectedInventorySlotIdx != InventorySlots::slotAquarium
        || !newAquariumExpansion.active 
        || isGuiBlockingBuilding()
    ){
        return;
    }

    DrawRectangleRec(expansion.exansionRect,
        expansion.isObstructed ? OBSTRUCTED_SHADOW_COLOR
            : expansion.aquarium != nullptr ? EXPAND_SHADOW_COLOR : DEFAULT_SHADOW_COLOR
    );
}

void GameState::draw() const {
    gui.drawBackground();

    for(const Aquarium & aquarium : aquariums)
        aquarium.draw();
    
    gameArea.drawEntities();

    for(const Shelf & shelf : shelves)
        shelf.draw();

    gui.draw(player);

    drawAquariumShadow(newAquariumExpansion);
}

void update(GameState & gameState){
    gameState.buildAquarium();
    gameState.gui.update(gameState.player);
    gameState.gameArea.updateGameArea(gameState.gui, gameState.aquariums, gameState.player);
    for(Aquarium & aquarium : gameState.aquariums)
        aquarium.update();
    gameState.gameArea.update();
}

void draw(const GameState & gameState){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    gameState.draw();

    EndDrawing();
}

void mainLoop(){
    GameState gameState;
    while(!WindowShouldClose()){ //Detect window close button or ESC key
        update(gameState);
        draw(gameState);
    }
}

int main(){
    //Setup
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tiny Ecosystem Clicker");
    SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    SetTargetFPS(60);

    mainLoop();

    CloseWindow();

    return 0;
}