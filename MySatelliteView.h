#pragma once

#include "common/SatelliteView.h"
#include <vector>


class MySatelliteView : public SatelliteView {
    size_t width;
    size_t height;
    std::vector<std::vector<char>> grid;
public:
    MySatelliteView(size_t width, size_t height);

    // Implement the pure virtual function
    char getObjectAt(size_t x, size_t y) const override;
    
    void setObjectAt(size_t x, size_t y, char object);
};