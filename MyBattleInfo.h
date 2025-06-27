#pragma once
#include "common/BattleInfo.h"
#include <vector>

class MyBattleInfo : public BattleInfo {
protected:
    std::vector<std::vector<char>> board;
    std::pair<int, int> tank_position;
    int max_steps = 0;
    int num_of_shells = 0;

    // Getting all objects locations
    std::vector<std::pair<int, int>> friendly_tanks;
    std::vector<std::pair<int, int>> enemy_tanks;
    std::vector<std::pair<int, int>> mines;
    std::vector<std::pair<int, int>> walls;
    std::vector<std::pair<int, int>> shells;

public:
    int getMaxSteps() const;
    std::vector<std::vector<char>> getBoard() const;
    std::pair<int, int> getTankPosition() const;
    std::vector<std::pair<int, int>> getFreindlyTanks() const;
    std::vector<std::pair<int, int>> getEnemyTanks() const;
    std::vector<std::pair<int, int>> getMines() const;
    std::vector<std::pair<int, int>> getWalls() const;
    std::vector<std::pair<int, int>> getShells() const;
    int getNumOfShells() const;

    void setNumOfShells(const int num);
    void setMaxSteps(const int max);
};