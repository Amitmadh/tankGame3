#include "MyTankAlgorithmFactory.h"

std::unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    if (player_index == 1) {
        return std::make_unique<TankAlgorithm1>(player_index, tank_index);
    } else if (player_index == 2) {
        return std::make_unique<TankAlgorithm2>(player_index, tank_index);
    } else {
        //error
        return std::make_unique<TankAlgorithm1>(1, tank_index);
    }
}
