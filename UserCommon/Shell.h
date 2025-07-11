#pragma once

#include "GameObject.h"

namespace UserCommon_322996059_211779582 {

class Shell: public GameObject {
    Direction direction;

public:
    Shell(int x, int y, Direction direction);
    // Getters and Setters
    Direction getDirection() const;
};

}