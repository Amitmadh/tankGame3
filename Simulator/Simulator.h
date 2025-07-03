#pragma once

#include "../UserCommon/MySatelliteView.h"
#include <memory>
#include <iostream>
#include <string>
#include <regex>
#include <cstddef> // for size_t
#include <vector>
#include <functional>
#include <fstream>


struct BoardInfo {
    size_t map_width = 0;
    size_t map_height = 0;
    size_t max_steps = 0;
    size_t num_shells = 0;
    MySatelliteView map;
};

class Simulator {
    int x;
    int y;
public:
    // reading board function
    bool readBoard(char* input_file, BoardInfo& board_info); // return false if failed

private:
    // readBoard helper functions:
    bool readFirstFiveLines(std::ifstream& file, BoardInfo& board_info); // return false if failed
    void readBoardLines(std::ifstream& file, BoardInfo& board_info);
    int parseLine(const std::string& line, const std::string& key) const; // ChatGPT generated

};
