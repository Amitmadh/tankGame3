#pragma once

#include "GameUtilis.h"

class GameObject {
    int x;
    int y;
public:
    GameObject(int x, int y);
    virtual ~GameObject();  // Virtual destructor to make the class polymorphic

    // Getters
    int getX() const;
    int getY() const;
    
    // Setters
    void setX(int new_x);
    void setY(int new_y);
};
