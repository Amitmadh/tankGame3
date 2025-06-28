#pragma once

#include "../common/TankAlgorithm.h"
#include "../UserCommon/GameUtilis.h"
#include "MyBattleInfo.h"
#include <vector>
#include <iostream>


class MyTankAlgorithm : public TankAlgorithm{
    public:
        MyTankAlgorithm(int player_index, int tank_index);
        ~MyTankAlgorithm() = default;

        virtual void updateBattleInfo(BattleInfo& info);
        virtual ActionRequest getAction() = 0;

    protected:
        int tank_index;
        std::pair<int, int> tank_position;
        Direction tank_direction;
        int num_of_shells;
        int shoot_cooldown = 0;
        int board_width;
        int board_height;
        std::vector<std::vector<char>> board;
        int length_to_look;
        bool last_action_was_shoot = false;

        ActionRequest moveForward();
        ActionRequest getBattleInfo();
        ActionRequest doNothing();
        ActionRequest rotateTankEighthLeft();
        ActionRequest rotateTankEighthRight();
        ActionRequest rotateTankQuarterLeft();
        ActionRequest rotateTankQuarterRight();
        virtual ActionRequest shoot();

        bool isBlockingShell(int x, int y) const;
        bool isAttackedFromDir(Direction rotaion_direction) const;

        bool isAvailibleLocation(std::pair<int ,int> location) const; 
        bool isDirectionAvailible(Direction rotaion_direction) const;

        ActionRequest escapeSides(char which_side);
        ActionRequest escapeFrontOrBack(char front_or_back);
        ActionRequest escapeDiagonal1(char front_or_back);
        ActionRequest escapeDiagonal2(char front_or_back);

        virtual ActionRequest escapeShell() = 0;

        bool isTankInfront(MyBattleInfo battle_Info) const;
};