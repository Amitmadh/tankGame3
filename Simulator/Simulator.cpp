#include "Simulator.h"

// ============================ READING BOARD FUNCTIONS =================================

bool Simulator::readBoard(char* input_file, BoardInfo& board_info){
    std::ifstream file(input_file);
    if (!file) {
        std::cout << "Error: Cannot open file " << input_file << std::endl;
        return false;
    }
    // Reading first 5 lines of the instructions
    if(!readFirstFiveLines(file, board_info)){
        return false;
    }
    board_info.map.initialize(board_info.map_width, board_info.map_height);
    // READING BOARD 
    readBoardLines(file, board_info);
    file.close();
    return true;
}

bool Simulator::readFirstFiveLines(std::ifstream& file, BoardInfo& board_info){
    std::string line;
    // Line 1: Description (ignored)
    if (!std::getline(file, line)) {
        std::cout << "Error: File is empty or missing description line." << std::endl;
        file.close();
        return false;
    }
    try {
        // Line 2: MaxSteps
        if (!std::getline(file, line)) throw std::runtime_error("Missing MaxSteps line");
        board_info.max_steps = parseLine(line, "MaxSteps");

        // Line 3: NumShells
        if (!std::getline(file, line)) throw std::runtime_error("Missing NumShells line");
        board_info.num_shells = parseLine(line, "NumShells");

        // Line 4: Rows
        if (!std::getline(file, line)) throw std::runtime_error("Missing Rows line");
        board_info.map_height = parseLine(line, "Rows");
        
        // Line 5: Cols
        if (!std::getline(file, line)) throw std::runtime_error("Missing Cols line");
        board_info.map_width = parseLine(line, "Cols");
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        file.close();
        return false;
    }
    return true;
}

void Simulator::readBoardLines(std::ifstream& file, BoardInfo& board_info){
    char ch, dummy;
    bool underflow = false;
    bool overflow = false;
    for (size_t y = 0; y < board_info.map_height; y++){
        for (size_t x = 0; x < board_info.map_width + 1; x++){
            file.get(ch); // puts a char in ch
            if ((ch == '\n' && x == board_info.map_width) || (file.eof() && x == board_info.map_width && y == board_info.map_height - 1)){
                break;
            }
            else if ((ch == '\n' || file.eof()) && x < board_info.map_width){
                underflow = true;
                break;
            }
            else if (ch != '\n' && x == board_info.map_width){
                overflow = true;
                while (file.get(ch)) { // Skipping the rest of the line
                    if (ch == '\n'){
                        break;
                    }
                }
            }
            else if (ch == '#'){
                board_info.map.setObjectAt(x, y, '#');
            }
            else if (ch == '1') {
                board_info.map.setObjectAt(x, y, '1');
            }
            else if (ch == '2') {
                board_info.map.setObjectAt(x, y, '2');
            }
            else if (ch == '@'){
                board_info.map.setObjectAt(x, y, '@');
            }
        }
    }
    if (file.get(dummy)) overflow = true;
    if (overflow || underflow) {
        std::ofstream errorFile("input_errors.txt");
        if (underflow) errorFile << "There are missing rows or columns!\n";
        if (overflow) errorFile << "There are additional rows or columns beyond the required!\n";
    }
}


int Simulator::parseLine(const std::string& line, const std::string& key) const{
    std::regex pattern(key + R"(\s*=\s*(\d+))");
    std::smatch match;
    if (std::regex_search(line, match, pattern)) {
        return std::stoi(match[1]);
    } else {
        throw std::runtime_error("Error: Could not parse " + key + " from line: " + line);
    } 
}

// =====================================================================================

// ============================ Debug 1v1 Testing =========================================

bool Simulator::debugBattle(char* map_file, std::string so_path_game_manager, std::string so_path_algorithm1, std::string so_path_algorithm2){

    // ============= Reading Board ================
    BoardInfo board_info;
    readBoard(map_file, board_info);

    // ============= Getting both registrars ================
    auto& game_manager_registrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithm_registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    // ============= Creating entries in registrars and Looading .so files ================
    // Game Manager
    game_manager_registrar.createGameManagerFactoryEntry(so_path_game_manager);
    void* handle_game_manager = dlopen(so_path_game_manager.c_str(), RTLD_LAZY);
    if (!handle_game_manager) {
        std::cerr << "Error loading " << so_path_game_manager << ": " << dlerror() << std::endl;
        return false;
    }
    game_manager_registrar.validateLastRegistration();

    // Algoeithm1
    algorithm_registrar.createAlgorithmFactoryEntry(so_path_algorithm1);
    void* handle_algorithm1 = dlopen(so_path_algorithm1.c_str(), RTLD_LAZY);
    if (!handle_algorithm1) {
        std::cerr << "Error loading " << so_path_algorithm1 << ": " << dlerror() << std::endl;
        return false;
    }
    algorithm_registrar.validateLastRegistration();


    // Algoeithm2
    algorithm_registrar.createAlgorithmFactoryEntry(so_path_algorithm2);
    void* handle_algorithm2 = dlopen(so_path_algorithm2.c_str(), RTLD_LAZY);
    if (!handle_algorithm2) {
        std::cerr << "Error loading " << so_path_algorithm2 << ": " << dlerror() << std::endl;
        return false;
    }
    algorithm_registrar.validateLastRegistration();

    {
        // ============= Running game ================
        auto game_manager_registrar_entry = game_manager_registrar.at(0);
        auto algorithm1_registrar_entry = algorithm_registrar.at(0);
        auto algorithm2_registrar_entry = algorithm_registrar.at(1);
        std::unique_ptr<AbstractGameManager> game_manager = game_manager_registrar_entry.createGameManager(true);
        std::unique_ptr<Player> player1 = algorithm1_registrar_entry.createPlayer(1,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
        std::unique_ptr<Player> player2 = algorithm2_registrar_entry.createPlayer(2,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
        TankAlgorithmFactory player1_tank_algo_factory = algorithm1_registrar_entry.getTankAlgorithmFactory();
        TankAlgorithmFactory player2_tank_algo_factory = algorithm2_registrar_entry.getTankAlgorithmFactory();
        std::string map_name = extractBaseName(std::string(map_file));
        std::string name1 = extractBaseName(so_path_algorithm1);
        std::string name2 = extractBaseName(so_path_algorithm2);
        GameResult result = game_manager->run(board_info.map_width, board_info.map_height, board_info.map, map_name ,board_info.max_steps, board_info.num_shells, *player1.get(), name1 ,*player2.get(), name2 , player1_tank_algo_factory, player2_tank_algo_factory);
        std::cout << stringGameResult(result, board_info.map_width, board_info.map_height);
    }
    // ============= Clearing registrars and unloading ================
    game_manager_registrar.clear();
    algorithm_registrar.clear();
    dlclose(handle_game_manager);
    dlclose(handle_algorithm1);
    dlclose(handle_algorithm2);
    return true;
}