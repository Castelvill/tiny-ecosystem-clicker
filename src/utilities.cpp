#include "utilities.hpp"

float getDistance(Vector2 pos1, Vector2 pos2){
    return std::sqrt(std::pow(pos1.x-pos2.x, 2) + std::pow(pos1.y-pos2.y, 2));
}

float getVectorMagnitude(Vector2 vector){
    return std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2));
}

void limitVector(Vector2 & vector, float limit){
    float magnitude = getVectorMagnitude(vector);
    if(magnitude > limit){
        vector.x = (limit * vector.x) / magnitude;
        vector.y = (limit * vector.y) / magnitude;
    }
}

float randomBetween(float min, float max){
    return min + ((rand() % 10001) / 10000.0f) * (max - min);
}

float randomBetween(Vector2 range){
    return range.x + ((rand() % 10001) / 10000.0f) * (range.y - range.x);
}

int randomBetween(int min, int max){
    return min + rand() % (1 + max - min);
}

int randomBetween(IntRange range){
    return range.min + rand() % (1 + range.max - range.min);
}

Vector2 angleToVector(float theta){
    return VEC2(std::cos(theta), std::sin(theta));
}

Vector2 operator*(Vector2 lhs, float rhs){
    return VEC2(
        lhs.x * rhs,
        lhs.y * rhs
    );
}