#pragma once

#include "MyBattleInfo2.h"
#include "MyPlayer.h"
#include "../UserCommon/Shell.h"
#include "../common/PlayerRegistration.h"

namespace Algorithm_322996059_211779582 {

class Player2 : public MyPlayer {
    std::vector<std::vector<char>> board;
    std::vector<Shell> shells;
    
    Direction getShellDirection(std::vector<std::vector<char>> old_board, std::pair<int, int> shell_location);
    void updateShells(std::vector<std::vector<char>> old_board, std::vector<std::vector<char>> new_board);

    public:
        Player2(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        
        void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};

}