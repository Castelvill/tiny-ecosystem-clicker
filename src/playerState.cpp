#include "playerState.hpp"

void PlayerState::initPlayer(){
    loadingIndicatorAngle = 0.0f;
    xp = 0;
    slowedDownXpBarLength = 0;
    level = 0;
    currentXpCap = 5;
}

PlayerState::PlayerState(){
    initPlayer();
}

void PlayerState::addExperience(int experiencePoints){
    xp += experiencePoints;
    if(xp >= currentXpCap){
        ++level;
        xp %= currentXpCap;
        currentXpCap *= EXPERIENCE_CAP_GROWTH;
        slowedDownXpBarLength = 0;
    }
}

void PlayerState::updateExperienceBar(){
    int realXpBarLength = (SCREEN_WIDTH * xp) / currentXpCap;
    if(slowedDownXpBarLength < realXpBarLength)
        slowedDownXpBarLength += std::max(0.2f, (realXpBarLength - slowedDownXpBarLength) * 0.1f);
}