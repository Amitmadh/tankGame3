#pragma once

#include "GameObject.h"
#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include <iostream>
#include <string>
#include <memory>


class Tank: public GameObject {
    Direction direction;
    int serial;
    int player; // AKA tank_index - 1 for player 1, 2 for player 2
    int tank_index;
    int shell_num;
    int backward_count = 0; // How many steps we already waited. We move backward when: (backward_count == 2) || (action_to_perform == MOVE_BACKWARD && backward_count == 3)
    int shoot_cooldown = 0; // How many steps we need to wait before we can shoot again.
    ActionRequest action_to_perform = ActionRequest::DoNothing;
    bool bad_move = false;
    std::unique_ptr<TankAlgorithm> tank_algorithm;
    std::string death_reason; // The reason why the tank got destroyed
    

public:
    Tank(int x, int y, Direction direction, int serial, int player, int tank_index, int shell_num, std::unique_ptr<TankAlgorithm> tank_algorithm);

    // Getters
    Direction getDirection() const;
    int getSerial() const;
    int getplayer() const;
    int getShellNum() const;
    int getBackwardCount() const;
    int getShootCooldown() const;
    ActionRequest getActionToPerform() const;
    TankAlgorithm* getTankAlgorithm() const;
    std::string getDeathReason() const;
    bool getBadMove() const;

    // Setters
    void setShellNum(int new_shell_num);
    void setBackwardCount(int new_backward_count);
    void setShootCooldown(int new_shoot_cooldown);
    void setActionToPerform(ActionRequest action);
    void setDeathReason(std::string death_reason);
    void setBadMove(bool is_bad_move);

    // Actions
    void rotateQuarter(bool right);
    void rotateEighth(bool right);
    void decrementShellNum();
    void incrementBackwardCount();

    // For debug purposes
    void debug_print();
};