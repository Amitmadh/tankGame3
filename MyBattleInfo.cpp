#include "MyBattleInfo.h"

//getters
std::vector<std::vector<char>> MyBattleInfo::getBoard() const{
    return board;
}

std::pair<int, int> MyBattleInfo::getTankPosition() const{
    return tank_position;
}

std::vector<std::pair<int, int>> MyBattleInfo::getFreindlyTanks() const{
    return friendly_tanks;
}

std::vector<std::pair<int, int>> MyBattleInfo::getEnemyTanks() const{
    return enemy_tanks;
}

std::vector<std::pair<int, int>> MyBattleInfo::getMines() const{
    return mines;
}

std::vector<std::pair<int, int>> MyBattleInfo::getWalls() const{
    return walls;
}

std::vector<std::pair<int, int>> MyBattleInfo::getShells() const{
    return shells;
}

int MyBattleInfo::getNumOfShells() const{
    return num_of_shells;
}

int MyBattleInfo::getMaxSteps() const{
    return max_steps;
}

//setters
void MyBattleInfo::setNumOfShells(const int num) {
    num_of_shells = num;
}

void MyBattleInfo::setMaxSteps(const int max) {
    max_steps = max;
}
