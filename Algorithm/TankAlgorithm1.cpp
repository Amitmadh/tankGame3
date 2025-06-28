#include "TankAlgorithm1.h"

TankAlgorithm1::TankAlgorithm1(int player_index, int tank_index) : MyTankAlgorithm(player_index, tank_index) {}

void TankAlgorithm1::updateBattleInfo(BattleInfo& info) {
    battle_info = (MyBattleInfo1&)info;
    MyTankAlgorithm::updateBattleInfo(info);
}

ActionRequest TankAlgorithm1::getAction() {
    //update shoot cooldown
    if (shoot_cooldown > 0) shoot_cooldown--;

    //every second turn request GetBattleInfo
    turn_number++;
    if (turn_number % 2 == 0) return getBattleInfo();
    
    //if attacked escape from shell
    ActionRequest action = escapeShell();
    if (action != ActionRequest::DoNothing) {
        return action;
    }

    //if theres an enemy tank infront shoot
    if (isTankInfront(battle_info)) {
        return shoot();
    }

    //rotate toward an enemy tank
    std::pair<int, int> closest_tank = getClosestTank();
    Direction direction = enemyTankDirection(closest_tank);
    return changeDirection(direction);
} 

ActionRequest TankAlgorithm1::escapeShell() {
    if (isAttackedFromDir(Direction::U) && last_action_was_shoot) return doNothing();
    if (isAttackedFromDir(Direction::U) && !last_action_was_shoot) return escapeFrontOrBack('F');
    else if (isAttackedFromDir(Direction::L)) return escapeSides('L');
    else if (isAttackedFromDir(Direction::R)) return escapeSides('R');
    else if (isAttackedFromDir(Direction::UL)) return escapeDiagonal1('F');
    else if (isAttackedFromDir(Direction::DR)) return escapeDiagonal1('B');
    else if (isAttackedFromDir(Direction::UR)) return escapeDiagonal2('F');
    else if (isAttackedFromDir(Direction::DL)) return escapeDiagonal2('B');
    else if (isAttackedFromDir(Direction::D)) return escapeFrontOrBack('B');
    else return doNothing();
}

std::pair<int, int> TankAlgorithm1::getClosestTank() const {
    int closest_distance = length_to_look;
    std::pair<int, int> closest_tank;

    for (std::pair<int, int> tank : battle_info.getEnemyTanks()) {
        int x_delta = std::min(std::abs(tank_position.first - tank.first), board_width - std::abs(tank_position.first - tank.first));
        int y_delta = std::min(std::abs(tank_position.second - tank.second), board_height - std::abs(tank_position.second - tank.second));
        int distance = x_delta + y_delta;
        if (distance < closest_distance) {
            closest_distance = distance;
            closest_tank = tank;
        }
    }
    return closest_tank;
}

Direction TankAlgorithm1::enemyTankDirection(std::pair<int, int> enemy_tank) const {
    int x_delta = tank_position.first - enemy_tank.first;
    int y_delta = tank_position.second - enemy_tank.second;
    if (abs(y_delta) > board_height / 2) {
        y_delta = (y_delta > 0) ? y_delta - board_height : y_delta + board_height;
    }
    
    if (std::abs(x_delta) > board_width / 2) {
        x_delta = (x_delta > 0) ? x_delta - board_width : x_delta + board_width;
    }

    if (y_delta < 0 && x_delta == 0) return Direction::U;
    else if (y_delta < 0 && x_delta < 0) return Direction::UR; 
    else if (y_delta == 0 && x_delta < 0) return Direction::R;
    else if (y_delta > 0 && x_delta < 0) return Direction::DR; 
    else if (y_delta > 0 && x_delta == 0) return Direction::D;
    else if (y_delta > 0 && x_delta > 0) return Direction::DL;
    else if (y_delta == 0 && x_delta > 0) return Direction::L; 
    else if (y_delta < 0 && x_delta > 0) return Direction::UL; 
    else return Direction::U; //default
}

ActionRequest TankAlgorithm1::changeDirection(Direction dest) {
    int dest_dir = (int)dest;
    int change_dir = (int)dest_dir - (int)tank_direction;

    if (change_dir == 0) {
        //default action if the tank is already in the right direction
        if (shoot_cooldown == 0 && num_of_shells > 0) {
            return shoot();
        } else {
            return doNothing();
        }
    } else if (change_dir == -1 || change_dir == 7) {
        return rotateTankEighthRight();
    } else if (change_dir == 1 || change_dir == -7) {
        return rotateTankEighthLeft();
    } else if (change_dir == -2 || change_dir == 6) {
        return rotateTankQuarterRight();
    } else if (change_dir == 2 || change_dir == -6) {
        return rotateTankQuarterLeft();
    } else if (change_dir == -3 || change_dir == 5) {
        return rotateTankQuarterRight();
    } else if (change_dir == 3 || change_dir == -5) {
        return rotateTankQuarterLeft();
    } else {
        return rotateTankQuarterRight();
    }
}

