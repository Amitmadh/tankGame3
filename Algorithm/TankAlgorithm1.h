#pragma once
#include "MyTankAlgorithm.h"
#include "MyBattleInfo1.h"
#include "../common/TankAlgorithmRegistration.h"

namespace Algorithm_322996059_211779582 {

class TankAlgorithm1 : public MyTankAlgorithm {
    int turn_number = -1;
    MyBattleInfo1 battle_info;

public:
    TankAlgorithm1(int player_index, int tank_index); // inherits the constructor
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:
    ActionRequest escapeShell() override;
    std::pair<int, int> getClosestTank() const; // get the closest tank to the current tank
    Direction enemyTankDirection(std::pair<int, int> enemy_tank) const; // rotate the tank towards the enemy tank
    ActionRequest changeDirection(Direction dest);
};

}