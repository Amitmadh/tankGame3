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

    // ============= Reading Board ================
    BoardInfo board_info;
    Simulator sim;
    sim.readBoard(argv[1], board_info);

    // ============= Getting both registrars ================
    auto& game_manager_registrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithm_registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    std::string so_path_game_manager = "GameManager/GameManager_322996059_211779582.so";
    std::string so_path_algorithm = "Algorithm/Algorithm_322996059_211779582.so";

    // ============= Creating entries in registrars ================
    game_manager_registrar.createGameManagerFactoryEntry(so_path_game_manager);
    algorithm_registrar.createAlgorithmFactoryEntry(so_path_algorithm);

    // ============= Looading .so files ================
    void* handle_game_manager = dlopen(so_path_game_manager.c_str(), RTLD_LAZY);
    if (!handle_game_manager) {
        std::cerr << "Error loading " << so_path_game_manager << ": " << dlerror() << std::endl;
        return 1;
    }

    void* handle_algorithm = dlopen(so_path_algorithm.c_str(), RTLD_LAZY);
    if (!handle_algorithm) {
        std::cerr << "Error loading " << so_path_algorithm << ": " << dlerror() << std::endl;
        return 1;
    }
    
    // ============= Validating and Running game ================
    try {
        game_manager_registrar.validateLastRegistration();
        algorithm_registrar.validateLastRegistration();
        std::cout << "Number of items in game_manager_registrar registrar: " << game_manager_registrar.count() << std::endl;
        std::cout << "Number of items in algorithm_registrar registrar: " << algorithm_registrar.count() << std::endl;

        auto& game_manager_registrar_entry = game_manager_registrar.getLast();
        auto& algorithm_registrar_entry = algorithm_registrar.getLast();

        std::unique_ptr<AbstractGameManager> game_manager = game_manager_registrar_entry.createGameManager(true);
        std::unique_ptr<Player> player1 = algorithm_registrar_entry.createPlayer(1,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
        std::unique_ptr<Player> player2 = algorithm_registrar_entry.createPlayer(2,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
        TankAlgorithmFactory player1_tank_algo_factory = algorithm_registrar_entry.getTankAlgorithmFactory();
        TankAlgorithmFactory player2_tank_algo_factory = algorithm_registrar_entry.getTankAlgorithmFactory();

        GameResult result = game_manager->run(board_info.map_width, board_info.map_height, board_info.map, board_info.max_steps, board_info.num_shells, *player1.get(), *player2.get(), player1_tank_algo_factory, player2_tank_algo_factory);
        std::cout << stringGameResult(result, board_info.map_width, board_info.map_height);
    }
    catch (GameManagerRegistrar::BadRegistrationException& e) {
        // TODO: report according to requirements
        std::cout << "---------------------------------" << std::endl;
        std::cout << "BadRegistrationException for: " << so_path_game_manager << std::endl;
        std::cout << "Name as registered: " << e.name << std::endl;
        std::cout << "Has Game Manager factory? " << std::boolalpha << e.hasGameManagerFactory << std::endl;
        std::cout << "---------------------------------" << std::endl;
        game_manager_registrar.removeLast();
    }
    catch(AlgorithmRegistrar::BadRegistrationException& e) {
        // TODO: report according to requirements
        std::cout << "---------------------------------" << std::endl;
        std::cout << "BadRegistrationException for: " << so_path_algorithm << std::endl;
        std::cout << "Name as registered: " << e.name << std::endl;
        std::cout << "Has Tank Algorithm factory? " << std::boolalpha << e.hasTankAlgorithmFactory << std::endl;
        std::cout << "Has Player factory? " << std::boolalpha << e.hasPlayerFactory << std::endl;
        std::cout << "---------------------------------" << std::endl;
        algorithm_registrar.removeLast();
    }

    // ============= Clearing registrars and unloading ================
    game_manager_registrar.clear();
    algorithm_registrar.clear();
    dlclose(handle_game_manager);
    dlclose(handle_algorithm);
}
