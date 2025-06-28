#pragma once

#include "common/TankAlgorithmFactory.h"
#include "Algorithm/TankAlgorithm1.h"
#include "Algorithm/TankAlgorithm2.h"

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    ~MyTankAlgorithmFactory() override = default;
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};