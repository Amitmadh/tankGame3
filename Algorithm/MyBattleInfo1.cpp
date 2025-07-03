#include "MyBattleInfo1.h"

namespace Algorithm_322996059_211779582 {

MyBattleInfo1::MyBattleInfo1(const std::vector<std::vector<char>> game_board, int steps) {
    board = game_board;
    max_steps = steps;
    for (size_t y = 0; y < board.size(); y++) {
        for (size_t x = 0; x < board[y].size(); x++) {
            if (board[y][x] == '#') {
                walls.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '1') {
                friendly_tanks.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '2') {
                enemy_tanks.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '@') {
                mines.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '%') {
                tank_position = std::pair<int, int>(x, y);
            } else if (board[y][x] == '*') {
                shells.push_back(std::pair<int, int>(x, y));
                num_of_shells++;
            } 
        }
    }
}

MyBattleInfo1::MyBattleInfo1() {
    board = std::vector<std::vector<char>>(1, std::vector<char>(1, '1'));
}

}