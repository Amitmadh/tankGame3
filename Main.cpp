#include <iostream>
#include <fstream>
#include <sstream>
#include "GameManager/GameManager.h"
#include "Simulator/Simulator.h"
#include "UserCommon/MySatelliteView.h"
#include "UserCommon/GameUtilis.h"
#include "Algorithm/Player1.h"
#include "Algorithm/Player2.h"
#include "Algorithm/TankAlgorithm1.h"
#include "Algorithm/TankAlgorithm2.h"
#include "common/TankAlgorithm.h"
#include "common/GameResult.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n" << std::endl;
        return 1;
    }
    size_t map_width, map_height, max_steps, num_shells;
    MySatelliteView map;
    Simulator sim;
    
    sim.readBoard(argv[1], map, map_width, map_height, max_steps, num_shells);
    for(size_t y = 0; y < map_height; y++){
        for(size_t x = 0; x < map_width; x++){
            if(map.getObjectAt(x, y) == ' '){
                std::cout << '_';
            }
            else{
                std::cout << map.getObjectAt(x, y);
            }
        }
        std::cout << std::endl;
    }
    GameManager game(true);
    Player1 player1(1,map_width, map_height, max_steps, num_shells);
    Player2 player2(2,map_width, map_height, max_steps, num_shells);
    TankAlgorithmFactory factory1 = [](int player_index, int tank_index) {
    return std::make_unique<TankAlgorithm1>(player_index, tank_index);};
    TankAlgorithmFactory factory2 = [](int player_index, int tank_index) {
    return std::make_unique<TankAlgorithm2>(player_index, tank_index);};
    GameResult result = game.run(map_width, map_height, map, max_steps, num_shells, player1, player2, factory1, factory2);
    std::cout << stringGameResult(result, map_width, map_height);
}