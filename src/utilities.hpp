#pragma once

#include <stdlib.h>
#include <math.h>
#include <string>
#include "raylib.h"
#include <ostream>
#include <istream>
#include <type_traits>
#include <cstdint>

using std::string;

#define VEC2(X,Y) (Vector2){X,Y}

struct IntRange{int min; int max;};

struct MinMaxIntRange{IntRange min; IntRange max;};

struct ColorRange{Color first; Color second;};

float getDistance(Vector2 pos1, Vector2 pos2);

Vector2 getDirectionVector(Vector2 pos1, Vector2 pos2);

float getVectorMagnitude(Vector2 vector);

void limitVector(Vector2 & vector, float limit);

float randomBetween(float min, float max);

float randomBetween(Vector2 range);

unsigned char randomBetween(unsigned char min, unsigned char max);

int randomBetween(int min, int max);

int randomBetween(IntRange range);

Color randomBetween(const ColorRange & range);

//theta: <0.0f, 2.0f * PI>
Vector2 angleToVector(float theta);

float vectorToAngle(Vector2 vector);

Vector2 operator+(Vector2 lhs, float rhs);

Vector2 operator-(Vector2 lhs, float rhs);

Vector2 operator*(Vector2 lhs, float rhs);

Vector2 operator/(Vector2 lhs, float rhs);

bool operator==(Vector2 lhs, Vector2 rhs);

Vector2 operator+(Vector2 lhs, Vector2 rhs);
Vector2 operator-(Vector2 lhs, Vector2 rhs);

string intToStr(int integer);

bool detectButtonCollision(Vector2 mousePos, Vector2 buttonPos, Vector2 buttonSize);

Rectangle toRectangle(Vector2 position, Vector2 size);

bool collisionOfPointAndRectangle(Vector2 point, Rectangle rectangle);

// Write an object as raw bytes
template <typename T>
bool writeToBinary(std::ostream &out, const T &value) {
    static_assert(std::is_trivially_copyable_v<T>);
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return bool(out);
}

// Read an object from raw bytes
template <typename T>
bool readFromBinary(std::istream &in, T &value) {
    static_assert(std::is_trivially_copyable_v<T>);
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
    return bool(in);
}