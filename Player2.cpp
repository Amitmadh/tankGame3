#include "Player2.h"

Player2::Player2(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) :
    MyPlayer(player_index, x, y, max_steps, num_shells){}

void Player2::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    std::vector<std::vector<char>> new_board = getBoard(satellite_view);
    updateShells(board, new_board);
    board = new_board;
    MyBattleInfo2 battle_info = MyBattleInfo2(new_board, max_steps, shells);
    battle_info.setNumOfShells(num_shells);
    tank.updateBattleInfo(battle_info);
}

Direction Player2::getShellDirection(std::vector<std::vector<char>> old_board, std::pair<int, int> shell_location) {
    for (int dir = 0; dir < 8; dir++) {
        std::pair<int, int> previous_location = shell_location;
        for (int i = 0; i < 2; i++) {
            previous_location = move(previous_location, width, height, (Direction)dir);
        }

        if (old_board[previous_location.second][previous_location.first] == '*') {
            if (dir - 4 >= 0) {
                return Direction(dir - 4);
            } else {
                return Direction(dir + 4);
            }
        }
    }
    return Direction::U;
}

void Player2::updateShells(std::vector<std::vector<char>> old_board, std::vector<std::vector<char>> new_board) {
        shells.clear();
        std::vector<std::pair<int, int>> shell_locations;
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                if (new_board[y][x] == '*') {
                    shell_locations.push_back(std::pair<int, int>(x, y));
                }
            }
        }

        for (std::pair shell : shell_locations) {
            Direction dir = getShellDirection(old_board, shell);
            shells.push_back(Shell(shell.first, shell.second, dir));
        }
}
