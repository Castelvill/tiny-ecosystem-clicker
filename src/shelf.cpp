#include "shelf.hpp"

Shelf::Shelf(Vector2 newPosition, Vector2 newSize){
    position = newPosition;
    size = newSize;
}

void Shelf::draw() const {
    DrawRectangleV(position, size, DARKBROWN);
}