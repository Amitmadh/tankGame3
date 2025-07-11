#include "MySatelliteView.h"

namespace UserCommon_322996059_211779582 {

MySatelliteView::MySatelliteView() = default;

MySatelliteView::MySatelliteView(size_t width, size_t height)
    : width(width), height(height), grid(width, std::vector<char>(height, ' ')) {}


void MySatelliteView::initialize(size_t width, size_t height) {
    this->width = width;
    this->height = height;
    grid = std::vector<std::vector<char>>(width, std::vector<char>(height, ' '));
}

char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= width || y >= height) {
        return '&';
    }
    return grid[x][y];
}

void MySatelliteView::setObjectAt(size_t x, size_t y, char object) {
    grid[x][y] = object;
}

}