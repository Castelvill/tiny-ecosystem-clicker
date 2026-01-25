#include "utilities.hpp"

float getDistance(Vector2 pos1, Vector2 pos2){
    return std::sqrt(std::pow(pos1.x-pos2.x, 2) + std::pow(pos1.y-pos2.y, 2));
}

Vector2 getDirectionVector(Vector2 pos1, Vector2 pos2){
    return {pos1.x-pos2.x, pos1.y-pos2.y};
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

unsigned char randomBetween(unsigned char min, unsigned char max){
    return min + rand() % (1 + max - min);
}

int randomBetween(IntRange range){
    return range.min + rand() % (1 + range.max - range.min);
}

Color randomBetween(const ColorRange & range){
    return{
        randomBetween(range.first.r, range.second.r),
        randomBetween(range.first.g, range.second.g),
        randomBetween(range.first.b, range.second.b),
        randomBetween(range.first.a, range.second.a),
    };
}

Vector2 angleToVector(float theta){
    return VEC2(std::cos(theta), std::sin(theta));
}

float vectorToAngle(Vector2 vector){
    return std::atan2(vector.y, vector.x);
}

Vector2 operator+(Vector2 lhs, float rhs){
    return VEC2(
        lhs.x + rhs,
        lhs.y + rhs
    );
}

Vector2 operator-(Vector2 lhs, float rhs){
    return VEC2(
        lhs.x - rhs,
        lhs.y - rhs
    );
}

Vector2 operator*(Vector2 lhs, float rhs){
    return VEC2(
        lhs.x * rhs,
        lhs.y * rhs
    );
}

Vector2 operator/(Vector2 lhs, float rhs){
    return VEC2(
        lhs.x / rhs,
        lhs.y / rhs
    );
}

bool operator==(Vector2 lhs, Vector2 rhs){
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

Vector2 operator+(Vector2 lhs, Vector2 rhs){
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Vector2 operator-(Vector2 lhs, Vector2 rhs){
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

string intToStr(int integer){
    char buff[11];
    sprintf(buff, "%d", integer);
    return buff;
}

bool detectButtonCollision(Vector2 mousePos, Vector2 buttonPos, Vector2 buttonSize){
    return mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x
        && mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
}

Rectangle toRectangle(Vector2 position, Vector2 size){
    return {position.x, position.y, size.x, size.y};
}

bool collisionOfPointAndRectangle(Vector2 point, Rectangle rectangle){
    return point.x > rectangle.x
        && point.x < rectangle.x + rectangle.width
        && point.y > rectangle.y
        && point.y < rectangle.y + rectangle.height;
}