#include "gui.hpp"

UserInterface::UserInterface(){
    waterTexture = LoadTexture("images/water.png");
    removeWaterTexture = LoadTexture("images/spill_water.png");
    algaeTexture = LoadTexture("images/algae.png");
    sandTexture = LoadTexture("images/sand.png");
    soilTexture = LoadTexture("images/soil.png");
    gravelTexture = LoadTexture("images/gravel.png");
    ostracodTexture = LoadTexture("images/ostracod.png");
    seedTexture = LoadTexture("images/seed.png");
    settingsTexture = LoadTexture("images/settings.png");
    backgroundTexture = LoadTexture("images/background.png");
    SetTextureWrap(backgroundTexture, TEXTURE_WRAP_REPEAT);
}

UserInterface::~UserInterface(){
    UnloadTexture(waterTexture);
    UnloadTexture(removeWaterTexture);
    UnloadTexture(algaeTexture);
    UnloadTexture(sandTexture);
    UnloadTexture(soilTexture);
    UnloadTexture(gravelTexture);
    UnloadTexture(ostracodTexture);
    UnloadTexture(seedTexture);
    UnloadTexture(settingsTexture);
    UnloadTexture(backgroundTexture);
}

Vector2 UserInterface::getSlotPos(int slotIdx) const {
    return {
        INVENTORY_POS.x + BASIC_PADDING + slotIdx * (INVENTORY_SLOT_SIZE.x + SPACE_BETWEEN_SLOTS),
        INVENTORY_POS.y + BASIC_PADDING
    };
}

void UserInterface::update(PlayerState & player){
    if(IsWindowResized())
        screenSize = {float(GetScreenWidth()), float(GetScreenHeight())};

    Vector2 mousePos = GetMousePosition();
    if(IsMouseButtonPressed(0)
        //Temporary guardrails until proper aquariums are added
        && mousePos.x < SCREEN_WIDTH 
        && mousePos.y < SCREEN_HEIGHT
    ){
        for(int slotIdx = 0; slotIdx < MAX_SLOT_NUMBER; ++slotIdx){
            Vector2 slotPos = getSlotPos(slotIdx);
            if(detectButtonCollision(mousePos, slotPos, INVENTORY_SLOT_SIZE)){
                selectedInventorySlotIdx = slotIdx;
                break;
            }
        }

        //TODO: Use or remove it
        bool isMouseInAquarium = mousePos.y > GUI_HEIGHT;
        if(isMouseInAquarium){
            player.loadingIndicatorAngle += 10.0f;
            if(player.loadingIndicatorAngle >= 360.0f)
                player.loadingIndicatorAngle = 0.0f;
        }
    }
    player.updateExperienceBar(screenSize.x);
}

void UserInterface::drawInventory(const PlayerState & player) const {
    //Inventory bar
    DrawRectangleV(INVENTORY_POS, {screenSize.x, INVENTORY_BAR_SIZE.y}, INVENTORY_BAR_COLOR);
    //Inventory slots
    for(int slotIdx = 0; slotIdx < MAX_SLOT_NUMBER; ++slotIdx){
        Vector2 slotPos = getSlotPos(slotIdx);
        if(selectedInventorySlotIdx == slotIdx){
            DrawRectangleV({slotPos.x-5, slotPos.y-5}, INVENTORY_BAR_SIZE,
                SELECTED_INVENTORY_SLOT_COLOR
            );
        }
        DrawRectangleGradientV(slotPos.x, slotPos.y, INVENTORY_SLOT_SIZE.x, INVENTORY_SLOT_SIZE.y,
            RAYWHITE, INVENTORY_SLOT_BACKGROUND
        );
        //Drawing icons
        switch (slotIdx){
            case InventorySlots::slotWater: 
                DrawTextureV(waterTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotRemoveWater: 
                DrawTextureV(removeWaterTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotAlgae: 
                DrawTextureV(algaeTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotSoil: 
                DrawTextureV(soilTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotGravel: 
                DrawTextureV(gravelTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotSand: 
                DrawTextureV(sandTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotOstracod: 
                DrawTextureV(ostracodTexture, slotPos, WHITE);
                break;
            case InventorySlots::slotSeed: 
                DrawTextureV(seedTexture, slotPos, WHITE);
                break;
            default:
                break;
        }
    }
    DrawTextureV(settingsTexture, {screenSize.x - 35.0f, 5.0f}, WHITE);
}

void UserInterface::drawLevelBar(const PlayerState & player) const {
    Vector2 levelBarPos = VEC2(0, INVENTORY_BAR_SIZE.y);
    DrawRectangleV(levelBarPos, VEC2(screenSize.x, LEVEL_BAR_HEIGHT), LEVEL_BAR_BACKGROUND_COLOR);
    DrawRectangleV(levelBarPos, VEC2(player.slowedDownXpBarLength, LEVEL_BAR_HEIGHT), YELLOW);
    string levelText = "Level " + intToStr(player.level);
    DrawText(levelText.c_str(), levelBarPos.x + BASIC_PADDING, levelBarPos.y + BASIC_PADDING, 20,
        BLACK
    );
}

void UserInterface::draw(const PlayerState & player) const {
    drawInventory(player);
    drawLevelBar(player);
    
    //TODO: Use or remove it
    //Loading indicator
    //DrawCircleGradient(mousePos.x, mousePos.y, 20, {255, 255, 255, 155}, { 255, 109, 194, 155 });
    // Vector2 mousePos = GetMousePosition();
    // if(mousePos.y > GUI_HEIGHT)
    //     DrawCircleSector(mousePos, 20, 0.0f, player.loadingIndicatorAngle,
    //         20.0f, {255, 109, 194, 155}
    //     );
}

void UserInterface::drawBackground() const {

    Rectangle source = {
        0, 0,
        screenSize.x,
        screenSize.y
    };

    Rectangle dest = {
        0, GUI_HEIGHT,
        screenSize.x,
        screenSize.y
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(backgroundTexture, source, dest, origin, 0.0f, WHITE);

    //DrawTextureV(backgroundTexture, {0, GUI_HEIGHT}, WHITE);
}