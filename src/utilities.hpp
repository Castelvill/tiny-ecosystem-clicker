#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#define VEC2(X,Y) (Vector2){X,Y}

float getDistance(Vector2 pos1, Vector2 pos2);

float getVectorMagnitude(Vector2 vector);

void limitVector(Vector2 & vector, float limit);

float randomBetween(float min, float max);
