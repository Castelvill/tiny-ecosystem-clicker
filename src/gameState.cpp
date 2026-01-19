#include "gameState.hpp"

bool GameState::doesExpansionCollideWithAquariums(const Rectangle & expansionRect) const {
    for(const Aquarium & aquarium : aquariums){
        if(CheckCollisionRecs(expansionRect, aquarium.getRectangle()))
            return true;
    }
    return false;
}

AquariumBuilder GameState::getAquariumBuilder() const {
    AquariumBuilder expansionState;

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
                aquariumRect.x, aquariumRect.y, aquariumRect.width, EXPANSION_SIZE_FOR_MOUSE
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectUp)){
                aquariumRectUp.y -= REAL_EXPANSION_SIZE;
                aquariumRectUp.height = REAL_EXPANSION_SIZE;

                expansionState.direction = Direction::up;
                expansionState.exansionRect = aquariumRectUp;
                if(doesExpansionCollideWithAquariums(aquariumRectUp))
                    expansionState.isObstructed = true;

                return expansionState;
            }

            //Expand to the right
            Rectangle aquariumRectRight = {
                aquariumRect.x + aquariumRect.width - EXPANSION_SIZE_FOR_MOUSE, aquariumRect.y,
                EXPANSION_SIZE_FOR_MOUSE, aquariumRect.height
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectRight)){
                aquariumRectRight.x += EXPANSION_SIZE_FOR_MOUSE;
                aquariumRectRight.width = REAL_EXPANSION_SIZE;

                expansionState.direction = Direction::right;
                expansionState.exansionRect = aquariumRectRight;
                if(doesExpansionCollideWithAquariums(aquariumRectRight))
                    expansionState.isObstructed = true;

                return expansionState;
            }
            
            //Expand to the left
            Rectangle aquariumRectLeft = {
                aquariumRect.x, aquariumRect.y, EXPANSION_SIZE_FOR_MOUSE, aquariumRect.height
            };
            if(collisionOfPointAndRectangle(mousePosition, aquariumRectLeft)){
                aquariumRectLeft.x -= REAL_EXPANSION_SIZE;
                aquariumRectLeft.width = REAL_EXPANSION_SIZE;

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
    aquariumBuilder.active = false;

    aquariumBuilder = getAquariumBuilder();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isGuiBlockingBuilding() && !aquariumBuilder.isObstructed){
        //Expand existing aquarium
        if(aquariumBuilder.aquarium != nullptr
            && aquariumBuilder.aquarium >= aquariums.data()
            && aquariumBuilder.aquarium < aquariums.data() + aquariums.size()
        ){
            size_t aquariumIdx = aquariumBuilder.aquarium - aquariums.data();
            aquariums[aquariumIdx].expand(aquariumBuilder.direction, REAL_EXPANSION_SIZE);
        }
        else //Or create a new one
            aquariums.push_back(Aquarium(aquariumBuilder.exansionRect));

        aquariumBuilder.active = false;
    }
}

Aquarium* GameState::findAquariumUnderTheMouse(){
    Vector2 mousePosition = GetMousePosition();
    for(Aquarium & aquarium : aquariums){
        if(collisionOfPointAndRectangle(mousePosition, aquarium.getRectangle()))
            return &aquarium;
    }
    return nullptr;
}

void AquariumMover::clear(){
    wasAquariumClicked = false;
    aquarium = nullptr;
    originalPosition = {0, 0};
    startingMouseOffset = {0, 0};
    isCollision = false;
}

void GameState::checkForAquariumMoverCollision(){
    Rectangle movedAquariumRect = aquariumMover.aquarium->getRectangle();
    for(Aquarium & aquarium : aquariums){
        if(&aquarium == aquariumMover.aquarium)
            continue;
        if(CheckCollisionRecs(aquarium.getRectangle(), movedAquariumRect)){
            aquariumMover.isCollision = true;
            return;
        }
    }
    aquariumMover.isCollision = false;
}

void GameState::moveAquarium(){
    if(!aquariumMover.wasAquariumClicked){
        aquariumMover.aquarium = nullptr;
        Aquarium * aquariumUnderTheMouse = findAquariumUnderTheMouse();
        if(aquariumUnderTheMouse == nullptr)
            return;

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            aquariumMover.wasAquariumClicked = true;
            aquariumMover.aquarium = aquariumUnderTheMouse;
            aquariumMover.originalPosition = aquariumUnderTheMouse->getPosition();
            aquariumMover.startingMouseOffset = GetMousePosition()
                - aquariumUnderTheMouse->getPosition();
            aquariumMover.isCollision = false;
        }
        else{
            aquariumMover.wasAquariumClicked = false;
            aquariumMover.aquarium = aquariumUnderTheMouse;
        }
    }
    else{
        if(aquariumMover.aquarium == nullptr){
            aquariumMover.clear();
        }
        else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            aquariumMover.aquarium->setPosition(
                GetMousePosition() - aquariumMover.startingMouseOffset
            );
            checkForAquariumMoverCollision();
        }
        else{
            if(aquariumMover.isCollision)
                aquariumMover.aquarium->setPosition(aquariumMover.originalPosition);
            aquariumMover.clear();
        }
    }
}

void GameState::performGlobalActions(){
    switch(gui.selectedInventorySlotIdx){
        case InventorySlots::slotBuildAquarium:
            buildAquarium();
            return;
        case InventorySlots::slotMoveAquarium:
            moveAquarium();
            return;
        default:
            return;
    }
}

void GameState::update(){
    performGlobalActions();
    gui.update(player);
    gameArea.updateGameArea(gui, aquariums, player);
    for(Aquarium & aquarium : aquariums)
        aquarium.update();
    gameArea.update();
}

void GameState::drawAquariumBuilderOverlay() const {
    if(!aquariumBuilder.active || isGuiBlockingBuilding())
        return;

    DrawRectangleRec(aquariumBuilder.exansionRect,
        aquariumBuilder.isObstructed ? OBSTRUCTED_OVERLAY_COLOR
            : aquariumBuilder.aquarium != nullptr ? EXPAND_OVERLAY_COLOR
                : ACTIVE_OVERLAY_COLOR
    );
}

void GameState::drawAquariumMoverOverlay() const {
    if(aquariumMover.aquarium == nullptr)
        return;

    DrawRectangleRec(aquariumMover.aquarium->getRectangle(),
        !aquariumMover.wasAquariumClicked ? DEFAULT_OVERLAY_COLOR
            : aquariumMover.isCollision ? OBSTRUCTED_OVERLAY_COLOR : ACTIVE_OVERLAY_COLOR
    );
}

void GameState::drawGlobalActionsOverlays() const {
    switch(gui.selectedInventorySlotIdx){
        case InventorySlots::slotBuildAquarium:
            drawAquariumBuilderOverlay();
            return;
        case InventorySlots::slotMoveAquarium:
            drawAquariumMoverOverlay();
            return;
        default:
            return;
    }
}

void GameState::draw() const {
    gui.drawBackground();

    for(const Aquarium & aquarium : aquariums)
        aquarium.draw();
    
    gameArea.drawEntities();

    gui.draw(player);

    drawGlobalActionsOverlays();
}