#pragma once
#include "TankAlgorithm.h"
#include <memory>  // for std::unique_ptr

class TankAlgorithmFactory {
public:
    virtual ~TankAlgorithmFactory() {}
    virtual std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const = 0;
};
    