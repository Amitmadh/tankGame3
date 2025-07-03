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
    BoardInfo board_info;
    Simulator sim;
    
    sim.readBoard(argv[1], board_info);
    for(size_t y = 0; y < board_info.map_height; y++){
        for(size_t x = 0; x < board_info.map_width; x++){
            if(board_info.map.getObjectAt(x, y) == ' '){
                std::cout << '_';
            }
            else{
                std::cout << board_info.map.getObjectAt(x, y);
            }
        }
        std::cout << std::endl;
    }
    GameManager_322996059_211779582::GameManager game(true);
    Algorithm_322996059_211779582::Player2 player1(1,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
    Algorithm_322996059_211779582::Player2 player2(2,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
    TankAlgorithmFactory factory1 = [](int player_index, int tank_index) {
    return std::make_unique<Algorithm_322996059_211779582::TankAlgorithm2>(player_index, tank_index);};
    TankAlgorithmFactory factory2 = [](int player_index, int tank_index) {
    return std::make_unique<Algorithm_322996059_211779582::TankAlgorithm2>(player_index, tank_index);};
    GameResult result = game.run(board_info.map_width, board_info.map_height, board_info.map, board_info.max_steps, board_info.num_shells, player1, player2, factory1, factory2);
    std::cout << stringGameResult(result, board_info.map_width, board_info.map_height);
}