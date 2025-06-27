#pragma once
#include "MyBattleInfo.h"
#include "Shell.h"
#include <vector>

class MyBattleInfo2 : public MyBattleInfo {
    std::vector<Shell> shells_object;

public:
    MyBattleInfo2();
    MyBattleInfo2(std::vector<std::vector<char>> game_board, int steps, std::vector<Shell> shells_object);

    std::vector<Shell> getShellsObject() const;  
    void setShellObject(const std::vector<Shell> shells);
};