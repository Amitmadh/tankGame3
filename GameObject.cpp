#include "GameObject.h"

GameObject::GameObject(int x, int y): x(x) , y(y){}
GameObject::~GameObject() {}

// Getters
int GameObject::getX() const{
    return x;
}

int GameObject::getY() const{
    return y;
}

// Setters
void GameObject::setX(int new_x){
    x = new_x;
}
void GameObject::setY(int new_y){
    y = new_y;
}