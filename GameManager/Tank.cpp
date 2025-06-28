#include "Tank.h"

class GameObject; // Forward declaration of GameObject class

Tank::Tank(int x, int y, Direction direction, int serial, int player, int tank_index, int shell_num, std::unique_ptr<TankAlgorithm> tank_algorithm):
        GameObject(x, y), direction(direction), serial(serial), player(player), tank_index(tank_index), shell_num(shell_num), tank_algorithm(std::move(tank_algorithm)) {}

// For debug purposes
void Tank::debug_print(){
    std::cout << "Tank No. " << serial << ": " << "Player: " << player << ", Tank index: " << tank_index << ", Shell number: " << shell_num << ", backward counter: " << backward_count;
    std::cout <<  ", shoot cooldown: " << shoot_cooldown << ", direction: " << stringDirection(direction) << std::endl;
}

// Getters
Direction Tank::getDirection() const{
    return direction;
}
int Tank::getSerial() const{
    return serial;
}

int Tank::getplayer() const{
    return player;
}
int Tank::getShellNum() const{
    return shell_num;
}
int Tank::getBackwardCount() const{
    return backward_count;
}
int Tank::getShootCooldown() const{
    return shoot_cooldown;
}
ActionRequest Tank::getActionToPerform() const{
    return action_to_perform;
}
TankAlgorithm* Tank::getTankAlgorithm() const{
    return tank_algorithm.get();
}
std::string Tank::getDeathReason() const{
    return death_reason;
}
bool Tank::getBadMove() const{
    return bad_move;
}

// Setters
void Tank::setShellNum(int new_shell_num){
    shell_num = new_shell_num;
}
void Tank::setBackwardCount(int new_backward_count){
    backward_count = new_backward_count;
}
void Tank::setShootCooldown(int new_shoot_cooldown){
    shoot_cooldown = new_shoot_cooldown;
}
void Tank::setActionToPerform(ActionRequest action){
    action_to_perform = action;
}
void Tank::setDeathReason(std::string death_reason_message){
    // We are allowed to set the death reason only once
    if (death_reason == ""){
        death_reason = death_reason_message;
    }
}
void Tank::setBadMove(bool is_bad_move){
    bad_move = is_bad_move;
}


// Actions
void Tank::rotateQuarter(bool right){
    if (right){
        // Rotating a quarter to the right
        direction = rotateQuarterRight(direction);
    }
    else{
        // Rotating a quarter to the left
        direction = rotateQuarterLeft(direction);
    }
}
void Tank::rotateEighth(bool right){
    if (right){
        // Rotating an eight to the right
        direction = rotateEighthRight(direction);
    }
    else{
        // Rotating an eight to the left
        direction = rotateEighthLeft(direction);
    }
}
void Tank::decrementShellNum(){
    shell_num -= 1;
}
void Tank::incrementBackwardCount(){
    backward_count += 1;
}