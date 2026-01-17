#include "utilities.hpp"

class Shelf{
public:
    Vector2 position;
    Vector2 size;
    Shelf(Vector2 newPosition, Vector2 newSize);
    void draw() const;
};