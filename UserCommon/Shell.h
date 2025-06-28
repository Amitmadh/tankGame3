#pragma once

#include "GameObject.h"

class Shell: public GameObject {
    Direction direction;

public:
    Shell(int x, int y, Direction direction);
    // Getters and Setters
    Direction getDirection() const;
};