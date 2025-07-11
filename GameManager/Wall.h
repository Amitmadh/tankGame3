#pragma once

#include "../UserCommon/GameObject.h"

namespace GameManager_322996059_211779582 {
using namespace UserCommon_322996059_211779582;

class Wall: public GameObject {
    bool weak = false;
public:
    Wall(int row, int column);

    // sets weak to "true"
    void becomeWeak();
    bool isWeak() const;
};

}