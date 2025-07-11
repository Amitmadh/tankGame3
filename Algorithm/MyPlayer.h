#pragma once
#include "../common/Player.h"
#include "../UserCommon/GameUtilis.h"
#include <utility>

namespace Algorithm_322996059_211779582 {
using namespace UserCommon_322996059_211779582;

class MyPlayer : public Player {
protected:
    int player_index;
    size_t width;
    size_t height;
    size_t max_steps;
    size_t num_shells;
    
protected:
    std::vector<std::vector<char>> getBoard(SatelliteView& satellite_view) const;

public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
};

}