#include "Shell.h"

Shell::Shell(int x, int y, Direction direction): GameObject(x, y), direction(direction){}

// Getters and setters
Direction Shell::getDirection() const{
    return direction;
}
