#pragma once
#include "MyTankAlgorithm.h"
#include "MyBattleInfo2.h"

class TankAlgorithm2 : public MyTankAlgorithm {
    MyBattleInfo2 battle_info;
    bool is_only_tank = true;
    int turn_number = -1;
    int turns_to_get_battle_info = -1;

public:
    TankAlgorithm2(int player_index, int tank_index);
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;

private:    
    bool isAttacked() const;
    ActionRequest escapeShell() override;

    void updateTurnsToGetBattleInfo();
    bool isFreePosition(std::pair<int, int> posotion) const;
    ActionRequest findDirectionTowardsEnemy(); // find the direction to the enemy tank using bfs
    ActionRequest changeDirection(Direction dest); // change the direction of the tank
};