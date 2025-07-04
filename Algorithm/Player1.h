#pragma once

#include "../common/PlayerRegistration.h"
#include "MyBattleInfo1.h"
#include "MyPlayer.h"
#include <cstddef> // for size_t
#include <vector>

namespace Algorithm_322996059_211779582 {

class Player1 : public MyPlayer {
    public:
        Player1(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        
        void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};

}