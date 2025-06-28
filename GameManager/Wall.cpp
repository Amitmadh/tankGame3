#include "Wall.h"

Wall::Wall(int row, int column) : GameObject(row, column){}

void Wall::becomeWeak(){
    weak = true;
}

bool Wall::isWeak() const {
    return weak;
}