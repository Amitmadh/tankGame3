#include "MyBattleInfo2.h"

namespace Algorithm_322996059_211779582 {

MyBattleInfo2::MyBattleInfo2(std::vector<std::vector<char>> game_board, int steps, std::vector<Shell> shells_object) : shells_object(shells_object) {
    board = game_board;
    max_steps = steps;
    for (size_t y = 0; y < board.size(); y++) {
        for (size_t x = 0; x < board[y].size(); x++) {
            if (board[y][x] == '#') {
                walls.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '1') {
                enemy_tanks.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '2') {
                friendly_tanks.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '@') {
                mines.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '*') {
                shells.push_back(std::pair<int, int>(x, y));
            } else if (board[y][x] == '%') {
                tank_position = std::pair<int, int>(x, y);
                friendly_tanks.push_back(std::pair<int, int>(x, y));
            }
        }
    }
}

MyBattleInfo2::MyBattleInfo2() {
    std::vector<std::vector<char>> empty_board(1, std::vector<char>(1, '2'));
    board = empty_board;
}

std::vector<Shell> MyBattleInfo2::getShellsObject() const{
    return shells_object;
}

void MyBattleInfo2::setShellObject(const std::vector<Shell> shells) {
    shells_object = shells;
}

}