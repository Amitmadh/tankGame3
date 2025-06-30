#include "Simulator.h"

// ============================ READING BOARD FUNCTIONS =================================

bool Simulator::readBoard(char* input_file, MySatelliteView& map, size_t& map_width, size_t& map_height, size_t& max_steps, size_t& num_shells){
    std::ifstream file(input_file);
    if (!file) {
        std::cout << "Error: Cannot open file " << input_file << std::endl;
        return false;
    }
    // Reading first 5 lines of the instructions
    if(!readFirstFiveLines(file, map_width, map_height, max_steps, num_shells)){
        return false;
    }
    map.initialize(map_width, map_height);
    // READING BOARD 
    readBoardLines(file, map, map_width, map_height);
    file.close();
    return true;
}

bool Simulator::readFirstFiveLines(std::ifstream& file, size_t& map_width, size_t& map_height, size_t& max_steps, size_t& num_shells){
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
        max_steps = parseLine(line, "MaxSteps");

        // Line 3: NumShells
        if (!std::getline(file, line)) throw std::runtime_error("Missing NumShells line");
        num_shells = parseLine(line, "NumShells");

        // Line 4: Rows
        if (!std::getline(file, line)) throw std::runtime_error("Missing Rows line");
        map_height = parseLine(line, "Rows");
        
        // Line 5: Cols
        if (!std::getline(file, line)) throw std::runtime_error("Missing Cols line");
        map_width = parseLine(line, "Cols");
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        file.close();
        return false;
    }
    return true;
}

void Simulator::readBoardLines(std::ifstream& file, MySatelliteView& map, size_t map_width, size_t map_height){
    char ch, dummy;
    bool underflow = false;
    bool overflow = false;
    for (size_t y = 0; y < map_height; y++){
        for (size_t x = 0; x < map_width + 1; x++){
            file.get(ch); // puts a char in ch
            if ((ch == '\n' && x == map_width) || (file.eof() && x == map_width && y == map_height - 1)){
                break;
            }
            else if ((ch == '\n' || file.eof()) && x < map_width){
                underflow = true;
                break;
            }
            else if (ch != '\n' && x == map_width){
                overflow = true;
                while (file.get(ch)) { // Skipping the rest of the line
                    if (ch == '\n'){
                        break;
                    }
                }
            }
            else if (ch == '#'){
                map.setObjectAt(x, y, '#');
            }
            else if (ch == '1') {
                map.setObjectAt(x, y, '1');
            }
            else if (ch == '2') {
                map.setObjectAt(x, y, '2');
            }
            else if (ch == '@'){
                map.setObjectAt(x, y, '@');
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