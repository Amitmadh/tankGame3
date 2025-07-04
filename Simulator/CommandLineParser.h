#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <set>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>

enum class Mode { Comparative, Competitive };

struct SimulationConfig {
    Mode mode;
    std::string gameMapFile; // Comparative
    std::string gameMapsFolder; // Competitive
    std::string gameManagerFile; // Competitive
    std::string gameManagersFolder; // Comparative
    std::string algorithm1; // Comparative
    std::string algorithm2; // Comparative
    std::string algorithmsFolder; // Competitive
    int numThreads = 1;
    bool verbose = false;
};

class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    bool parse();
    void printUsageWithErrors() const;
    SimulationConfig getConfig() const;

private:
    int argc;
    char** argv;

    bool has_mode = false;
    SimulationConfig config;
    std::vector<std::string> arguments;
    std::vector<std::string> missing_argument_errors;
    std::vector<std::string> unsupported_argument_errors;
    std::vector<std::string> file_errors;
    std::vector<std::string> folder_errors;

    void parseMode();
    void parseCompetitive();
    void parseComparative();
    void validateFiles();
    void validateFolders();
    void validateFile(const std::string& path);
    void validateFolder(const std::string& path);
};
