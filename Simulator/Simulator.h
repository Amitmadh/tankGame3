#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <regex>
#include <cstddef> // for size_t
#include <vector>
#include <functional>
#include <thread>
#include <fstream>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#include <filesystem>

#include "../common/GameResult.h"
#include "../UserCommon/GameUtilis.h"
#include "../UserCommon/MySatelliteView.h"
#include "CommandLineParser.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"



struct BoardInfo {
    size_t map_width = 0;
    size_t map_height = 0;
    size_t max_steps = 0;
    size_t num_shells = 0;
    UserCommon_322996059_211779582::MySatelliteView map;
};

class Simulator {
    SimulationConfig config;
    std::vector<void*> handles; // For dlopenFiles and dlcloseFiles

public:
    Simulator(SimulationConfig config);
    void run();

    // Functions for loading and unloading all the .so files
    bool loadSharedLibraries(); // Return false if failed
    void unloadSharedLibraries();

    // =============== Debug Functions ===============
    bool debugBattle(std::string map_file, std::string so_path_game_manager, std::string so_path_algorithm1, std::string so_path_algorithm2);

private:
    // Comparative function
    void runComparative();
    // runComparative helper functions:
    void runComparativeThread(int thread_id, int num_threads, int game_managers_size, std::vector<GameResult>& results, BoardInfo& board_info);
    void groupByIdenticalOutcome(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height);
    void writeComparativeOutput(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height);
    void printComparativeOutput(std::vector<GameResult>& results, std::vector<std::vector<int>>& grouped_results, size_t map_width, size_t map_height);
    bool compareGameResult(const GameResult& result1, const GameResult& result2, size_t map_width, size_t map_height);
    std::string line5(std::vector<int>& group);
    std::string line6(const GameResult& result);


    // Competitive function
    void runCompetitive();
    // runCompetitive helper functions:


    // Reading board function
    bool readBoard(std::string& input_file, BoardInfo& board_info); // Return false if failed
    // readBoard helper functions:
    bool readFirstFiveLines(std::ifstream& file, BoardInfo& board_info); // Return false if failed
    void readBoardLines(std::ifstream& file, BoardInfo& board_info);
    int parseLine(const std::string& line, const std::string& key) const; // ChatGPT generated


    // loadSharedLibraries and unloadSharedLibraries helper functions:
    bool dlopenFolder(const std::string& folderPath, bool for_game_manager); // Return false if failed
    bool dlopenFile(const std::string& filePath, bool for_game_manager); // Return false if failed
    bool areSameFile(const std::string& path1, const std::string& path2);


    // General helper functions:
    std::string extractBaseName(const std::string& path);

};
