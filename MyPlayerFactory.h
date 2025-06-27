#pragma once

#include "common/PlayerFactory.h"
#include "Player1.h"
#include "Player2.h"

class MyPlayerFactory : public PlayerFactory {
public:

    ~MyPlayerFactory() override = default;
    
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                    size_t max_steps, size_t num_shells) const override;
};