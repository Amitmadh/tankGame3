#include "Simulator.h"

Simulator::Simulator(SimulationConfig config) : config(config) {}

void Simulator::run(){
    if(!loadSharedLibraries()){
        std::cout << "Failed loading necessary parts - Exiting!" << std::endl;
        unloadSharedLibraries();
        return;
    }
    std::cout << "Number of game_managers registered: " << GameManagerRegistrar::getGameManagerRegistrar().count() << std::endl; // TODO - delete
    std::cout << "Number algorithms registered: " << AlgorithmRegistrar::getAlgorithmRegistrar().count() << std::endl; // TODO - delete

    if (config.mode == Mode::Comparative){
        runComparative();
    }
    else {
        runCompetitive();
    }
    unloadSharedLibraries();
}

// ============================ READING BOARD FUNCTIONS =================================

bool Simulator::readBoard(std::string& input_file, BoardInfo& board_info) {
    std::ifstream file(input_file);
    if (!file) {
        appendToFile("input_errors.txt", "Error: Cannot open file " + input_file);
        return false;
    }
    // Reading first 5 lines of the instructions
    if(!readFirstFiveLines(file, board_info, input_file)){
        return false;
    }
    board_info.map.initialize(board_info.map_width, board_info.map_height);
    // READING BOARD 
    readBoardLines(file, board_info, input_file);
    file.close();
    return true;
}

bool Simulator::readFirstFiveLines(std::ifstream& file, BoardInfo& board_info, std::string& input_file){
    std::string line;
    // Line 1: Description (ignored)
    if (!std::getline(file, line)) {
        appendToFile("input_errors.txt", "Error: Map file is empty - " + input_file);
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
        appendToFile("input_errors.txt", "Error: Unrecoverable errors in map file " + input_file + " : " + e.what());
        file.close();
        return false;
    }
    return true;
}

void Simulator::readBoardLines(std::ifstream& file, BoardInfo& board_info, std::string& input_file){
    char ch, dummy;
    bool underflow = false;
    bool overflow = false;
    for (size_t y = 0; y < board_info.map_height; y++){
        for (size_t x = 0; x < board_info.map_width + 1; x++){
            file.get(ch); // puts a char in ch
            if ((ch == '\n' && x == board_info.map_width) || (file.eof() && x == board_info.map_width && y == board_info.map_height - 1)){
                break;
            } else if ((ch == '\n' || file.eof()) && x < board_info.map_width){
                underflow = true;
                break;
            } else if (ch != '\n' && x == board_info.map_width){
                overflow = true;
                while (file.get(ch)) { // Skipping the rest of the line
                    if (ch == '\n'){
                        break;
                    }
                }
            } else if (ch == '#'){
                board_info.map.setObjectAt(x, y, '#');
            } else if (ch == '1') {
                board_info.map.setObjectAt(x, y, '1');
            } else if (ch == '2') {
                board_info.map.setObjectAt(x, y, '2');
            } else if (ch == '@'){
                board_info.map.setObjectAt(x, y, '@');
            }
        }
    }
    if (file.get(dummy)) overflow = true;
    if (overflow || underflow) {
        appendToFile("input_errors.txt", "Recovered the following errors in map file " + input_file + " :");
        if (underflow) appendToFile("input_errors.txt", "There are missing rows or columns!");
        if (overflow) appendToFile("input_errors.txt", "There are additional rows or columns beyond the required!");
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

// ========================================================================================

// ==================== Loading and Unloading .so files functions =========================

bool Simulator::loadSharedLibraries(){
    if (config.mode == Mode::Comparative){
        bool loaded_algorithm1, loaded_algorithm2;
        int num_loaded_game_managers_files;
        num_loaded_game_managers_files = dlopenFolder(config.gameManagersFolder, true);
        loaded_algorithm1 = dlopenFile(config.algorithm1, false);
        if (areSameFile(config.algorithm1, config.algorithm2)){
            loaded_algorithm2 = true;
        } else {
            loaded_algorithm2 = dlopenFile(config.algorithm2, false);
        }
        if (num_loaded_game_managers_files == 0 || !loaded_algorithm1 || !loaded_algorithm2){
            if (!loaded_algorithm1) {
                std::cout << "Usage: Algorithm1 couldn't load successfully" << std::endl;
            } else if (!loaded_algorithm2) {
                std::cout << "Usage: Algorithm2 couldn't load successfully" << std::endl;
            } else {
                std::cout << "Usage: No game_manager could load successfully" << std::endl;
            }
            return false;
        }
        return true;
    } else { // Competitive
        bool loaded_game_manager;
        int num_loaded_algorithms_file;
        loaded_game_manager = dlopenFile(config.gameManagerFile, true);
        num_loaded_algorithms_file = dlopenFolder(config.algorithmsFolder, false);
        if (!loaded_game_manager || num_loaded_algorithms_file == 0){
            if (!loaded_game_manager) {
                std::cout << "Usage: game_manager couldn't load successfully" << std::endl;
            } else if (num_loaded_algorithms_file == 0){
                std::cout << "Usage: No Algorithm could load successfully" << std::endl;
            }
            return false;
        }
        return true;
    }
}

void Simulator::unloadSharedLibraries(){
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    for (auto handle : handles) {
        if (handle) {
            dlclose(handle);
        }
    }
    handles.clear();
}


int Simulator::dlopenFolder(const std::string& folderPath, bool for_game_manager) {
    int num_of_loaded_files = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string soPath = entry.path().string();
            if (dlopenFile(soPath, for_game_manager)) {
                num_of_loaded_files++;
            }
        }
    }
    return num_of_loaded_files;
}

 bool Simulator::dlopenFile(const std::string& filePath, bool for_game_manager) {
    if (for_game_manager){
        auto& game_manager_registrar = GameManagerRegistrar::getGameManagerRegistrar();
        game_manager_registrar.createGameManagerFactoryEntry(filePath);
        void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
        if (!handle) {
            appendToFile("input_errors.txt", "Failed to load the following .so file - " + filePath + ": " + dlerror());
            game_manager_registrar.removeLast();
            return false;
        } else {
            try {
                game_manager_registrar.validateLastRegistration();
            } catch(GameManagerRegistrar::BadRegistrationException& e) {
                appendToFile("input_errors.txt", "Failed to load the following .so file - " + filePath + ": BadRegistrationException");
                game_manager_registrar.removeLast();
                return false;
            }
            std::cout << "Loaded Game Manager successfully: " << filePath << std::endl; // TODO - delete this line
            handles.push_back(handle);
        }
    } else {
        auto& algorithm_registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        algorithm_registrar.createAlgorithmFactoryEntry(filePath);
        void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
        if (!handle) {
            appendToFile("input_errors.txt", "Failed to load the following .so file - " + filePath + ": " + dlerror());
            algorithm_registrar.removeLast();
            return false;
        } else {
            try {
                algorithm_registrar.validateLastRegistration();
            } catch(AlgorithmRegistrar::BadRegistrationException& e) {
                appendToFile("input_errors.txt", "Failed to load the following .so file - " + filePath + ": BadRegistrationException");
                algorithm_registrar.removeLast();
                return false;
            }
            std::cout << "Loaded Algorithm successfully: " << filePath << std::endl; // TODO - delete this line
            handles.push_back(handle);
        }
    }
    return true;
}

bool Simulator::areSameFile(const std::string& path1, const std::string& path2) {
    try {
        return std::filesystem::canonical(std::filesystem::path(path1)) == std::filesystem::canonical(std::filesystem::path(path2));
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
}
// ========================================================================================

// ================================== Comparative =========================================

void Simulator::runComparative() {
    int num_threads = config.numThreads;
    BoardInfo board_info;
    if (!readBoard(config.gameMapFile, board_info)) {
        std::cout << "Usage: Map file couldn't be read successfully" << std::endl;
        return;
    }
    int game_managers_size = GameManagerRegistrar::getGameManagerRegistrar().count();
    std::vector<GameResult> results(game_managers_size);
    if (num_threads == 1) {
        runComparativeThread(0, num_threads, game_managers_size, std::ref(results), std::ref(board_info));
    }
    else{
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([this, i, num_threads, game_managers_size, &results, &board_info]() {
                this->runComparativeThread(i, num_threads, game_managers_size, results, board_info);
                    });
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
    std::vector<std::vector<int>> grouped_results(game_managers_size);
    groupByIdenticalOutcome(results, grouped_results, board_info.map_width, board_info.map_height);
    std::sort(grouped_results.begin(), grouped_results.end(),
          [](const std::vector<int>& a, const std::vector<int>& b) {
              return a.size() > b.size(); // descending order
          });
    std::cout << "first: " << grouped_results[0][0] << std::endl; // TODO - delete
    std::cout << "second: " << grouped_results[1][0] << std::endl; // TODO - delete
    writeComparativeOutput(results, grouped_results, board_info.map_width, board_info.map_height);
}

void Simulator::runComparativeThread(int thread_id, int num_threads, int game_managers_size, std::vector<GameResult>& results, BoardInfo& board_info){
    auto& game_manager_registrar = GameManagerRegistrar::getGameManagerRegistrar();
    auto& algorithm_registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    // Default to first algorithm; if two are registered, use the second for the second player
    auto algorithm1_registrar_entry = algorithm_registrar.at(0);
    auto algorithm2_registrar_entry = algorithm_registrar.at(0);
    if (algorithm_registrar.count() == 2) {
        algorithm2_registrar_entry = algorithm_registrar.at(1);
    }
    std::unique_ptr<Player> player1 = algorithm1_registrar_entry.createPlayer(1,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
    std::unique_ptr<Player> player2 = algorithm2_registrar_entry.createPlayer(2,board_info.map_width, board_info.map_height, board_info.max_steps, board_info.num_shells);
    TankAlgorithmFactory player1_tank_algo_factory = algorithm1_registrar_entry.getTankAlgorithmFactory();
    TankAlgorithmFactory player2_tank_algo_factory = algorithm2_registrar_entry.getTankAlgorithmFactory();

    for (int i = thread_id; i < game_managers_size; i += num_threads) {
        std::cout << "Thread ID: " << std::this_thread::get_id() << ", GameManager index: " << i << std::endl; // TODO - delete
        auto game_manager_registrar_entry = game_manager_registrar.at(i);
        std::unique_ptr<AbstractGameManager> game_manager = game_manager_registrar_entry.createGameManager(config.verbose);
        std::string map_name = extractBaseName(config.gameMapFile);
        std::string name1 = extractBaseName(config.algorithm1);
        std::string name2 = extractBaseName(config.algorithm2);
        GameResult result = game_manager->run(board_info.map_width, board_info.map_height, board_info.map, map_name ,board_info.max_steps, board_info.num_shells, *player1.get(), name1 ,*player2.get(), name2 , player1_tank_algo_factory, player2_tank_algo_factory);
        results[i] = std::move(result);
    }
}

void Simulator::groupByIdenticalOutcome(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height){
    for (size_t i = 0; i < results.size(); i++){
        for (std::vector<int>& group : grouped_results){
            if (group.empty()){
                group.push_back(i);
                break;
            }
            else if (compareGameResult(results[i], results[group.back()], map_width, map_height)){
                group.push_back(i);
                break;
            }
        }
    }
}

void Simulator::writeComparativeOutput(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height){
    std::filesystem::path folder = config.gameManagersFolder;
    std::string filename = "comparative_results_" + UserCommon_322996059_211779582::generateTimeBasedString() + ".txt";
    std::filesystem::path full_path = folder / filename;
    std::ofstream file(full_path);

    if (!file) {
        std::cout << "The file of comparative_results cannot be created in the folder: " << config.gameManagersFolder << std::endl;
        printComparativeOutput(results, grouped_results, map_width, map_height);
    }
    else{
        file << "game_map=" << extractBaseName(config.gameMapFile) << std::endl;
        file << "algorithm1=" << extractBaseName(config.algorithm1) << std::endl;
        file << "algorithm2=" << extractBaseName(config.algorithm2) << std::endl;
        file << std::endl;
        for (std::vector<int>& group : grouped_results){
            if (group.empty()){
                break;
            }
            else {
                file << line5(group) << std::endl;
                int result_index = group.back();
                file << line6(results[result_index]) << std::endl;
                file << results[result_index].rounds << std::endl;
                for (size_t y = 0; y < map_height; y++){
                    for (size_t x = 0; x < map_width; x++){
                        file << results[result_index].gameState->getObjectAt(x, y);
                    }
                    file << std::endl;
                }
            }
        }
    }
}

void Simulator::printComparativeOutput(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height){
    std::cout << "game_map=" << extractBaseName(config.gameMapFile) << std::endl;
    std::cout << "algorithm1=" << extractBaseName(config.algorithm1) << std::endl;
    std::cout << "algorithm2=" << extractBaseName(config.algorithm2) << std::endl;
    std::cout << std::endl;
    for (std::vector<int>& group : grouped_results){
        if (group.empty()){
            break;
        }
        else {
            std::cout << line5(group) << std::endl;
            int result_index = group.back();
            std::cout << line6(results[result_index]) << std::endl;
            std::cout << results[result_index].rounds << std::endl;
            for (size_t y = 0; y < map_height; y++){
                for (size_t x = 0; x < map_width; x++){
                    std::cout << results[result_index].gameState->getObjectAt(x, y);
                }
                std::cout << std::endl;
            }
        }
    }
}

bool Simulator::compareGameResult(const GameResult& result1, const GameResult& result2, size_t map_width, size_t map_height){
    bool same_winner = result1.winner == result2.winner;
    bool same_reason = result1.reason == result2.reason;
    bool same_rounds = result1.rounds == result2.rounds;
    bool same_game_state = true;
    for (size_t x = 0; x < map_width; x++){
        for (size_t y = 0; y < map_height; y++){
            if (result1.gameState->getObjectAt(x, y) != result2.gameState->getObjectAt(x, y)){
                same_game_state = false;
            }
        }
    }
    return same_winner && same_reason && same_rounds && same_game_state;
}

std::string Simulator::line5(std::vector<int>& group){
    auto& game_manager_registrar = GameManagerRegistrar::getGameManagerRegistrar();
    std::string line_5 = "";
    for(size_t i = 0; i < group.size(); i++){
        line_5 += extractBaseName(game_manager_registrar.at(group[i]).name());
        if (i + 1 < group.size()){
            line_5 += ", ";
        }
    }
    return line_5;
}

std::string Simulator::line6(const GameResult& result){
    if (result.reason == GameResult::Reason::ALL_TANKS_DEAD){
        if (result.winner == 1){
            return "Player 1 won with " + std::to_string(result.remaining_tanks[0]) + " tanks still alive";
        }
        else if (result.winner == 2){
            return "Player 2 won with " + std::to_string(result.remaining_tanks[1]) + " tanks still alive"; 
        }
        else{
            return "Tie, both players have zero tanks";
        }
    }
    else if (result.reason == GameResult::Reason::MAX_STEPS){
        return "Tie, reached max steps = " + std::to_string(result.rounds) + ", player 1 has " + std::to_string(result.remaining_tanks[0]) + " tanks, player 2 has "+ std::to_string(result.remaining_tanks[1]) + " tanks";
    }
    else {
        return "Tie, both players have zero shells for 40 steps";
    }
}

// ========================================================================================

// ================================== Competitive =========================================
 void Simulator::runCompetitive(){

 }



// ========================================================================================

// ================================ Debug 1v1 Testing =====================================

bool Simulator::debugBattle(std::string map_file, std::string so_path_game_manager, std::string so_path_algorithm1, std::string so_path_algorithm2){

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
        std::cout << UserCommon_322996059_211779582::stringGameResult(result, board_info.map_width, board_info.map_height);
    }
    // ============= Clearing registrars and unloading ================
    game_manager_registrar.clear();
    algorithm_registrar.clear();
    dlclose(handle_game_manager);
    dlclose(handle_algorithm1);
    dlclose(handle_algorithm2);
    return true;
}

// ========================================================================================

// ================================ General helper functions ==============================

std::string Simulator::extractBaseName(const std::string& path) {
    // Find last slash
    size_t slash_pos = path.find_last_of("/\\");
    size_t start = (slash_pos == std::string::npos) ? 0 : slash_pos + 1;

    // Find last dot
    size_t dot_pos = path.find_last_of('.');
    size_t end = (dot_pos == std::string::npos || dot_pos < start) ? path.size() : dot_pos;

    // Return substring between them
    return path.substr(start, end - start);
}


void Simulator::appendToFile(const std::string& filepath, const std::string& content) {
    // Check if the file exists (creates it if it doesn't)
    if (!std::filesystem::exists(filepath)) {
        std::ofstream createFile(filepath); // creates the file
        if (!createFile) {
            std::cerr << "Error: Could not create file: " << filepath << std::endl;
            return;
        }
    }

    // Open file in append mode
    std::ofstream outFile(filepath, std::ios::app);
    if (!outFile) {
        std::cerr << "Error: Could not open file for appending: " << filepath << std::endl;
        return;
    }

    outFile << content << std::endl;
    outFile.close();
}

// ========================================================================================
