#include "utilities.hpp"

float getDistance(Vector2 pos1, Vector2 pos2){
    return std::sqrt(std::pow(pos1.x-pos2.x, 2) + std::pow(pos1.y-pos2.y, 2));
}

float randomBetween(float min, float max){
    return min + ((rand() % 10001) / 10000.0f) * (max - min);
}