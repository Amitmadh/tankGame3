#pragma once

#include "GameObject.h"

class Wall: public GameObject {
    bool weak = false;
public:
    Wall(int row, int column);

    // sets weak to "true"
    void becomeWeak();
    bool isWeak() const;
};