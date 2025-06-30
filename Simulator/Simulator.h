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

class Simulator {
    int x;
    int y;
public:
    // reading board function
    bool readBoard(char* input_file, MySatelliteView& map, size_t& map_width, size_t& map_height, size_t& max_steps, size_t& num_shells); // return false if failed

private:
    // readBoard helper functions:
    bool readFirstFiveLines(std::ifstream& file, size_t& map_width, size_t& map_height, size_t& max_steps, size_t& num_shells); // return false if failed
    void readBoardLines(std::ifstream& file, MySatelliteView& map, size_t map_width, size_t map_height);
    int parseLine(const std::string& line, const std::string& key) const; // ChatGPT generated

};
