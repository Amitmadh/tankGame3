#include "Wall.h"

namespace GameManager_322996059_211779582 {

Wall::Wall(int row, int column) : GameObject(row, column){}

void Wall::becomeWeak(){
    weak = true;
}

bool Wall::isWeak() const {
    return weak;
}

}