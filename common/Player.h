#pragma once
#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <cstddef> // for size_t
#include <vector>

class Player {
public:
  Player(int , size_t , size_t , size_t , size_t) {}
  virtual ~Player() {}
  virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};
    