#include "plants.hpp"

class PlayerState{
    static constexpr float EXPERIENCE_CAP_GROWTH = 1.3f;
public:
    float loadingIndicatorAngle;
    int xp;
    float slowedDownXpBarLength;
    int level;
    int currentXpCap;

    void initPlayer();
    PlayerState();
    void addExperience(int experiencePoints);
    void updateExperienceBar(float currentScreenWidth);
};