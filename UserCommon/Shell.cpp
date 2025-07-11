#include "Shell.h"

namespace UserCommon_322996059_211779582 {

Shell::Shell(int x, int y, Direction direction): GameObject(x, y), direction(direction){}

// Getters and setters
Direction Shell::getDirection() const{
    return direction;
}

}
