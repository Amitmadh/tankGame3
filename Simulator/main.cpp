#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include "Simulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "CommandLineParser.h"
#include "../common/GameResult.h"
#include "../UserCommon/GameUtilis.h"



int main(int argc, char** argv) {
    CommandLineParser parser(argc, argv);
    if (!parser.parse()) {
        parser.printUsageWithErrors();
        return 1;
    }

    const SimulationConfig config = parser.getConfig();
    
    std::cout << "gameMapFile: " << config.gameMapFile << std::endl;
    std::cout << "gameMapsFolder: " << config.gameMapsFolder << std::endl;
    std::cout << "gameManagerFile: " << config.gameManagerFile << std::endl;
    std::cout << "gameManagersFolder: " << config.gameManagersFolder << std::endl;
    std::cout << "algorithm1: " << config.algorithm1 << std::endl;
    std::cout << "algorithm2: " << config.algorithm2 << std::endl;
    std::cout << "algorithmsFolder: " << config.algorithmsFolder << std::endl;
    std::cout << "numThreads: " << config.numThreads << std::endl;
    std::cout << "verbose: " << config.verbose << std::endl;

    Simulator simulator(config);
    simulator.run();
    /*
    // FOR DEBUGGING: replace simulator.run() with the code below
    std::string so_path_game_manager = "../group_so/game_managers/GameManager_322213836_212054837_2.so";
    std::string so_path_algorithm1 = "../group_so/ours/Algorithm_322996059_211779582_1.so";
    std::string so_path_algorithm2 = "../group_so/ours/Algorithm_322996059_211779582_2.so";
    std::string map_path = "../input_b.txt";
    simulator.debugBattle(map_path, so_path_game_manager, so_path_algorithm1, so_path_algorithm2);
    */
}