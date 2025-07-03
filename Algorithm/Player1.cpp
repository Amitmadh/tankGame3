#include "Player1.h"

namespace Algorithm_322996059_211779582 {

Player1::Player1(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) :
    MyPlayer(player_index, x, y, max_steps, num_shells){}

void Player1::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    MyBattleInfo1 battle_info = MyBattleInfo1(getBoard(satellite_view), max_steps);
    battle_info.setNumOfShells(num_shells);
    tank.updateBattleInfo(battle_info);
}

}