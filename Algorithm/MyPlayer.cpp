#include "MyPlayer.h"

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) :
    player_index(player_index), width(x), height(y), max_steps(max_steps), num_shells(num_shells) {
}

std::vector<std::vector<char>> MyPlayer::getBoard(SatelliteView& satellite_view) const {
    std::vector<std::vector<char>> board(height, std::vector<char>(width, ' '));
    for (size_t y = 0; y < height ; y++) {
        for (size_t x = 0; x < width; x++) {
            board[y][x] = satellite_view.getObjectAt(x, y);
        }
    }
    return board;
}
