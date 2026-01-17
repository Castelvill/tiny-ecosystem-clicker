#include <stdlib.h>
#include <math.h>
#include <string>
#include "raylib.h"

using std::string;

#define VEC2(X,Y) (Vector2){X,Y}

struct IntRange{int min; int max;};

struct MinMaxIntRange{IntRange min; IntRange max;};

float getDistance(Vector2 pos1, Vector2 pos2);

float getVectorMagnitude(Vector2 vector);

void limitVector(Vector2 & vector, float limit);

float randomBetween(float min, float max);

float randomBetween(Vector2 range);

int randomBetween(int min, int max);

int randomBetween(IntRange range);

//theta: <0.0f, 2.0f * PI>
Vector2 angleToVector(float theta);

Vector2 operator+(Vector2 lhs, float rhs);

Vector2 operator-(Vector2 lhs, float rhs);

Vector2 operator*(Vector2 lhs, float rhs);

Vector2 operator/(Vector2 lhs, float rhs);

bool operator==(Vector2 lhs, Vector2 rhs);

Vector2 operator+(Vector2 lhs, Vector2 rhs);
Vector2 operator-(Vector2 lhs, Vector2 rhs);

string intToStr(int integer);

bool detectButtonCollision(Vector2 mousePos, Vector2 buttonPos, Vector2 buttonSize);