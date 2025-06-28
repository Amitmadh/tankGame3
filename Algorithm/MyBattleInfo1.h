#pragma once

#include "MyBattleInfo.h"
#include <vector>

class MyBattleInfo1 : public MyBattleInfo {
public:
    MyBattleInfo1();
    MyBattleInfo1(const std::vector<std::vector<char>> game_board, int max_steps);
};