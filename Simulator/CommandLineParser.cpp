#include "CommandLineParser.h"

CommandLineParser::CommandLineParser(int argc, char** argv)
    : argc(argc), argv(argv) {}


void CommandLineParser::parseMode(){
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-verbose" && !config.verbose) {
            config.verbose = true;
        } else if (arg == "-comparative" && !has_mode) {
            config.mode = Mode::Comparative;
            has_mode = true;
        } else if (arg == "-competition" && !has_mode) {
            config.mode = Mode::Competitive;
            has_mode = true;
        } else {
            arguments.push_back(arg);
        }
    }
}

void CommandLineParser::parseCompetitive(){
    bool has_game_maps_folder = false, has_game_manager = false,
         has_algorithms_folder = false, num_threads = false;
    for (std::string arg : arguments){
        auto eq = arg.find('=');
        if (eq != std::string::npos) {
            std::string key = arg.substr(0, eq);
            std::string value = arg.substr(eq + 1);
            if (key == "game_maps_folder" && !has_game_maps_folder){
                config.gameMapsFolder = value;
                has_game_maps_folder = true;
            } else if (key == "game_manager" && !has_game_manager){
                config.gameManagerFile = value;
                has_game_manager = true;
            } else if (key == "algorithms_folder" && !has_algorithms_folder){
                config.algorithmsFolder = value;
                has_algorithms_folder = true;
            } else if (key == "num_threads" && !num_threads){
                try {
                    config.numThreads = std::stoi(value);
                    num_threads = true;
                } catch (...) {
                    unsupported_argument_errors.push_back(arg);
                }
            } else{
                unsupported_argument_errors.push_back(arg);
            }
        } else {
            unsupported_argument_errors.push_back(arg);
        }
    }
    if (!has_game_maps_folder) missing_argument_errors.push_back("game_maps_folder");
    if (!has_game_manager) missing_argument_errors.push_back("game_manager");
    if (!has_algorithms_folder) missing_argument_errors.push_back("algorithms_folder");
}

void CommandLineParser::parseComparative(){
    bool has_game_map = false, has_game_managers_folder = false,
         has_algorithm1 = false, has_algorithm2 = false, num_threads = false;
    for (std::string arg : arguments){
        auto eq = arg.find('=');
        if (eq != std::string::npos) {
            std::string key = arg.substr(0, eq);
            std::string value = arg.substr(eq + 1);
            if (key == "game_map" && !has_game_map){
                config.gameMapFile = value;
                has_game_map = true;
            } else if (key == "game_managers_folder" && !has_game_managers_folder){
                config.gameManagersFolder = value;
                has_game_managers_folder = true;
            } else if (key == "algorithm1" && !has_algorithm1){
                config.algorithm1 = value;
                has_algorithm1 = true;
            } else if (key == "algorithm2" && !has_algorithm2){
                config.algorithm2 = value;
                has_algorithm2 = true;
            } else if (key == "num_threads" && !num_threads){
                try {
                    config.numThreads = std::stoi(value);
                    num_threads = true;
                } catch (...) {
                    unsupported_argument_errors.push_back(arg);
                }
            } else{
                unsupported_argument_errors.push_back(arg);
            }
        } else {
            unsupported_argument_errors.push_back(arg);
        }
    }
    if (!has_game_map) missing_argument_errors.push_back("game_map");
    if (!has_game_managers_folder) missing_argument_errors.push_back("game_managers_folder");
    if (!has_algorithm1) missing_argument_errors.push_back("algorithm1");
    if (!has_algorithm2) missing_argument_errors.push_back("algorithm2");
}

bool CommandLineParser::parse() {
    parseMode();
    if (!has_mode) {
        return false;
    }
    if (config.mode == Mode::Comparative)
        parseComparative();
    else
        parseCompetitive();

    validateFiles();
    validateFolders();

    return unsupported_argument_errors.empty() && missing_argument_errors.empty() && file_errors.empty() && folder_errors.empty();
}

void CommandLineParser::printUsageWithErrors() const {
    if (!has_mode){
        std::cout << "\nMissing Mode Argument." << std::endl;
        std::cout << "Usage: ./simulator_<submitter_ids> -comparative ... or -competition ... with required arguments." << std::endl;
        return;
    }
    if (!unsupported_argument_errors.empty()){
        std::cout << "\nUnsupported Arguments (Including Duplicates):" << std::endl;
        for (const auto& arg : unsupported_argument_errors) {
            std::cout << arg << std::endl;
        }
    }
    if (!missing_argument_errors.empty()){
        if (config.mode == Mode::Comparative) std::cout << "\nMissing Arguments for Comparative Mode:" << std::endl;
        else std::cout << "\nMissing Arguments for Compatitive Mode:" << std::endl;
        for (const auto& arg : missing_argument_errors) {
            std::cout << arg << std::endl;
        }
    }
    if (unsupported_argument_errors.empty() && missing_argument_errors.empty()){
        if (!file_errors.empty()){
            std::cout << "\nFile Arguments Problems:" << std::endl;
            for (const auto& err : file_errors) {
                std::cout << err << std::endl;
            }
        }
        if (!folder_errors.empty()){
            std::cout << "\nFolder Arguments Problems:" << std::endl;
            for (const auto& err : folder_errors) {
                std::cout << err << std::endl;
            }
        }
    }
    if (config.mode == Mode::Comparative){
        std::cout << "\nUsage:\n" << "  ./simulator_<ids> -comparative game_map=<file> game_managers_folder=<folder> algorithm1=<file> algorithm2=<file> [num_threads=<num>] [-verbose]\n";
    } else {
        std::cout << "\nUsage:\n" << "  ./simulator_<ids> -competition game_maps_folder=<folder> game_manager=<file> algorithms_folder=<folder> [num_threads=<num>] [-verbose]\n";
    }
}

SimulationConfig CommandLineParser::getConfig() const {
    return config;
}

void CommandLineParser::validateFiles() {
    if (config.mode == Mode::Comparative) {
        validateFile(config.gameMapFile);
        validateFile(config.algorithm1);
        validateFile(config.algorithm2);
    } else if (config.mode == Mode::Competitive) {
        validateFile(config.gameManagerFile);
    }
}

void CommandLineParser::validateFolders() {
    if (config.mode == Mode::Comparative) {
        validateFolder(config.gameManagersFolder);
    } else if (config.mode == Mode::Competitive) {
        validateFolder(config.algorithmsFolder);
        validateFolder(config.gameMapsFolder);
    }
}

void CommandLineParser::validateFile(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        file_errors.push_back(path + " doesn't exist");
    } else {
        std::ifstream f(path);
        if (!f.is_open()) {
            file_errors.push_back(path + " cannot be opened");
        }
    }
}

void CommandLineParser::validateFolder(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        folder_errors.push_back(path + " doesn't exist");
        return;
    }

    if (!std::filesystem::is_directory(path)) {
        folder_errors.push_back(path + " cannot be traversed");
        return;
    }
}
