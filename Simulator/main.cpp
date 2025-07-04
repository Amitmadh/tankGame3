#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include "Simulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "../common/GameResult.h"
#include "../UserCommon/GameUtilis.h"



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n" << std::endl;
        return 1;
    }
    Simulator sim;
    std::string so_path_game_manager = "group_so/ours/GameManager_322996059_211779582.so";
    std::string so_path_algorithm1 = "group_so/group1/Algorithm_322213836_212054837.so";
    std::string so_path_algorithm2 = "group_so/ours/Algorithm_322996059_211779582_2.so";
    sim.debugBattle(argv[1], so_path_game_manager, so_path_algorithm1, so_path_algorithm2);
}
