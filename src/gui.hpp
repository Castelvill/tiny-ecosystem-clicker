#include "playerState.hpp"

enum InventorySlots: int {
    slotAquarium,
    slotWater,
    slotRemoveWater,
    slotAlgae,
    slotSoil,
    slotGravel,
    slotSand,
    slotOstracod,
    slotSeed
};

class UserInterface{
    static constexpr Vector2 INVENTORY_BAR_SIZE = {40, 40};
    static constexpr Vector2 INVENTORY_POS = {0, 0};
    static constexpr Vector2 INVENTORY_SLOT_SIZE = {30, 30};
    static constexpr float BASIC_PADDING = 5.0f;
    static constexpr float SPACE_BETWEEN_SLOTS = 10.0f;
    static constexpr float LEVEL_BAR_HEIGHT = 30.0f;

    static constexpr Color INVENTORY_BAR_COLOR = {198, 99, 0, 255};
    static constexpr Color SELECTED_INVENTORY_SLOT_COLOR = {248, 149, 50, 255};
    static constexpr Color INVENTORY_SLOT_BACKGROUND = {255, 214, 141, 255};
    static constexpr Color LEVEL_BAR_BACKGROUND_COLOR = {153, 50, 30, 255};
public:
    static const int GUI_HEIGHT = 70;
    static const int MAX_SLOT_NUMBER = 10;

    Texture2D aquariumTexture;
    Texture2D waterTexture;
    Texture2D removeWaterTexture;
    Texture2D algaeTexture;
    Texture2D sandTexture;
    Texture2D soilTexture;
    Texture2D gravelTexture;
    Texture2D ostracodTexture;
    Texture2D seedTexture;
    Texture2D settingsTexture;

    Texture2D backgroundTexture;

    int selectedInventorySlotIdx = 0;
    Vector2 screenSize = {SCREEN_WIDTH, SCREEN_HEIGHT};

    UserInterface();
    ~UserInterface();
    Vector2 getSlotPos(int slotIdx) const;
    void update(PlayerState & player);
    void drawInventory(const PlayerState & player) const;
    void drawLevelBar(const PlayerState & player) const;
    void draw(const PlayerState & player) const;
    void drawBackground() const;
};