#pragma once

#include "../common/ActionRequest.h"
#include "../common/GameResult.h"
#include <iostream>
#include <string>
#include <utility> // for std::pair
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace UserCommon_322996059_211779582 {

enum class Direction {U = 0, UR = 1, R = 2, DR = 3, D = 4, DL = 5, L = 6, UL = 7};
static  const Direction directions[] = {Direction::U, Direction::UR, Direction::R, Direction::DR, Direction::D, Direction::DL, Direction::L, Direction::UL};

void printLineToFile(const std::string& line);

std::pair<int, int> move(std::pair<int, int> loc, int width, int height, Direction direction);
Direction rotateQuarterRight(Direction curr_dir);
Direction rotateQuarterLeft(Direction curr_dir);
Direction rotateEighthRight(Direction curr_dir);
Direction rotateEighthLeft(Direction curr_dir);
Direction rotate(Direction curr_dir, Direction rotation_dir);

Direction oppositeDirection(Direction direction);
std::string stringDirection(Direction direction);
std::string stringAction(ActionRequest action);
std::string directionArrowString(Direction direction);
std::string generateTimeBasedString(); // generating a string based on the time
std::string stringGameResult(GameResult& game_result, size_t map_width, size_t map_height);
}