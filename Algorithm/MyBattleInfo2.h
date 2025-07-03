#pragma once

#include "../UserCommon/Shell.h"
#include "MyBattleInfo.h"
#include <vector>

namespace Algorithm_322996059_211779582 {

class MyBattleInfo2 : public MyBattleInfo {
    std::vector<Shell> shells_object;

public:
    MyBattleInfo2();
    MyBattleInfo2(std::vector<std::vector<char>> game_board, int steps, std::vector<Shell> shells_object);

    std::vector<Shell> getShellsObject() const;  
    void setShellObject(const std::vector<Shell> shells);
};

}