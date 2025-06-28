#include "MyTankAlgorithm.h"


MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index) : tank_index(tank_index){
    if (player_index == 1) {
        tank_direction = Direction::L;
    } else {
        tank_direction = Direction::R;
    }
} 

void MyTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    MyBattleInfo battle_info = (MyBattleInfo&)info;
    tank_position = battle_info.getTankPosition();
    board = battle_info.getBoard();
    board_height = board.size();
    board_width = board[0].size();
    num_of_shells = battle_info.getNumOfShells();
    length_to_look = battle_info.getMaxSteps();
}

bool MyTankAlgorithm::isTankInfront(MyBattleInfo battle_Info) const{
    std::pair<int, int> position = tank_position;
    for(int i = 0; i < length_to_look; i++){
        position = move(position, board_width, board_height, tank_direction);

        if (board[position.second][position.first] == '#') return false;

        for (std::pair<int, int> tank : battle_Info.getFreindlyTanks()) {
            if (position.first == tank.first && position.second == tank.second) {
                return false;
            }
        }

        for (std::pair<int, int> tank : battle_Info.getEnemyTanks()) {
            if (position.first == tank.first && position.second == tank.second) {
                return true;
            }
        }
    }
    return false;
}

bool MyTankAlgorithm::isBlockingShell(int x, int y) const {
    char c = board[x][y];
    if (c == '1' || c == '2' || c == '#') {
        return true;
    } else {
        return false;
    }
}

bool MyTankAlgorithm::isAvailibleLocation(std::pair<int ,int> location) const {
    char c = board[location.second][location.first];
    if (c == '1' || c == '2' || c == '#' || c == '@') {
        return false;
    } else {
        return true;
    }
}

ActionRequest MyTankAlgorithm::moveForward() {
    tank_position = move(tank_position, board_width, board_height, tank_direction);
    last_action_was_shoot = false;
    return ActionRequest::MoveForward;
}

ActionRequest MyTankAlgorithm::rotateTankEighthLeft() {
    tank_direction = rotateEighthLeft(tank_direction);
    last_action_was_shoot = false;
    return ActionRequest::RotateLeft45;
}

ActionRequest MyTankAlgorithm::rotateTankEighthRight() {
    tank_direction = rotateEighthRight(tank_direction);
    last_action_was_shoot = false;
    return ActionRequest::RotateRight45;
}

ActionRequest MyTankAlgorithm::rotateTankQuarterLeft() {
    tank_direction = rotateQuarterLeft(tank_direction);
    last_action_was_shoot = false;
    return ActionRequest::RotateLeft90;
}

ActionRequest MyTankAlgorithm::rotateTankQuarterRight() {
    tank_direction = rotateQuarterRight(tank_direction);
    last_action_was_shoot = false;
    return ActionRequest::RotateRight90;
}

ActionRequest MyTankAlgorithm::shoot() {
    if (shoot_cooldown <= 0) {
        shoot_cooldown = 5;
        last_action_was_shoot = true;
        return ActionRequest::Shoot;
    } else {
        return ActionRequest::DoNothing;
    }
}

ActionRequest MyTankAlgorithm::getBattleInfo() {
    return ActionRequest::GetBattleInfo;
}

ActionRequest MyTankAlgorithm::doNothing() {
    last_action_was_shoot = false;
    return ActionRequest::DoNothing;
}

ActionRequest MyTankAlgorithm::escapeSides(char which_side) {
    if (isDirectionAvailible(Direction::U)) return moveForward();
    else if (isDirectionAvailible(Direction::UL)) return rotateTankEighthLeft();
    else if (isDirectionAvailible(Direction::UR)) return rotateTankEighthRight();
    else if (isDirectionAvailible(Direction::DL)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::DR)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::D)) return rotateTankQuarterRight();
    else if (which_side == 'L') return rotateTankQuarterLeft();
    else return rotateTankQuarterRight();
}

ActionRequest MyTankAlgorithm::escapeFrontOrBack(char front_or_back) {
    if (front_or_back == 'F' && shoot_cooldown == 0 && num_of_shells > 0) return shoot();
    else if (isDirectionAvailible(Direction::R)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::L)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::UL)) return rotateTankEighthLeft();
    else if (isDirectionAvailible(Direction::UR)) return rotateTankEighthRight();
    else if (isDirectionAvailible(Direction::DL)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::DR)) return rotateTankQuarterRight();
    else return rotateTankQuarterRight();
}

ActionRequest MyTankAlgorithm::escapeDiagonal1(char front_or_back) {
    if (isDirectionAvailible(Direction::U)) return moveForward();
    else if (isDirectionAvailible(Direction::R)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::L)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::UR)) return rotateTankEighthRight();
    else if (isDirectionAvailible(Direction::DL)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::D)) return rotateTankQuarterRight();
    else if (front_or_back == 'F') {
        return rotateTankEighthLeft();
    } else {
        return rotateTankQuarterRight();
    }
}

ActionRequest MyTankAlgorithm::escapeDiagonal2(char front_or_back) {
    if (isDirectionAvailible(Direction::U)) return moveForward();
    else if (isDirectionAvailible(Direction::R)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::L)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::UL)) return rotateTankEighthLeft();
    else if (isDirectionAvailible(Direction::DR)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::D)) return rotateTankQuarterRight();
    else if (front_or_back == 'F') {
        return rotateTankEighthRight();
    } else {
        return rotateTankQuarterLeft();
    }
}

bool MyTankAlgorithm::isAttackedFromDir(Direction rotaion_direction) const {
    Direction dir = rotate(tank_direction, rotaion_direction);
    std::pair<int, int> position = tank_position;

    for (int i = 0; i < length_to_look; i++) {
        position = move(position, board_width, board_height, dir);
        if (board[position.second][position.first] == '*') {
            return true;
        }
        if (isBlockingShell(position.second, position.first)) {
            return false;
        }
    }
    return false;
}

bool MyTankAlgorithm::isDirectionAvailible(Direction rotaion_direction) const {
    Direction dir = rotate(tank_direction, rotaion_direction);
    std::pair<int ,int> step = move(tank_position, board_width, board_height, dir);
    return isAvailibleLocation(step);
}


