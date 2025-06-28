#include "GameUtilis.h"


std::pair<int, int> move(std::pair<int, int> loc, int width, int height, Direction direction){
    int x = loc.first;
    int y = loc.second;
    int new_x, new_y;

    if (direction == Direction::U) {
    new_x = x;
    new_y = y + 1;
    } else if (direction == Direction::UR) {
    new_x = x + 1;
    new_y = y + 1;
    } else if (direction == Direction::R) {
    new_x = x + 1;
    new_y = y;
    } else if (direction == Direction::DR) {
    new_x = x + 1;
    new_y = y - 1;
    } else if (direction == Direction::D) {
    new_x = x;
    new_y = y - 1;
    } else if (direction == Direction::DL) {
    new_x = x - 1;
    new_y = y - 1;
    } else if (direction == Direction::L) {
    new_x = x - 1;
    new_y = y;
    } else if (direction == Direction::UL) {
    new_x = x - 1;
    new_y = y + 1;
    }

    // Cyclic wrapping
    new_x = ((new_x % width) + width) % width;
    new_y = ((new_y % height) + height) % height;
    std::pair<int, int> ret_loc = std::make_pair(new_x, new_y);
    return ret_loc;
}

Direction oppositeDirection(Direction direction){
    switch (direction) {
        case Direction::U :
            return Direction::D;
        case Direction::UR :
            return Direction::DL; 
        case Direction::R :
            return Direction::L; 
        case Direction::DR :
            return Direction::UL;
        case Direction::D :
            return Direction::U;
        case Direction::DL :
            return Direction::UR;
        case Direction::L :
            return Direction::R;
        case Direction::UL :
            return  Direction::DR;
    }
    std::cout << "Problem in oppositeDirection function" << std::endl;
    return Direction::DL;
}

std::string stringDirection(Direction direction){
    switch (direction) {
        case Direction::U :
            return "U";
        case Direction::UR :
            return "UR";
        case Direction::R :
            return "R";
        case Direction::DR :
            return "DR";
        case Direction::D :
            return "D";
        case Direction::DL :
            return "DL";
        case Direction::L :
            return "L";
        case Direction::UL :
            return  "UL";
    }
    return "no_string";
}

std::string stringAction(ActionRequest action){
    switch (action) {
        case ActionRequest::MoveForward:
            return "MoveForward";
        case ActionRequest::MoveBackward:
            return "MoveBackward";
        case ActionRequest::RotateLeft45:
            return "RotateLeft45";
        case ActionRequest::RotateRight45:
            return "RotateRight45";
        case ActionRequest::RotateLeft90:
            return "RotateLeft90";
        case ActionRequest::RotateRight90:
            return "RotateRight90";
        case ActionRequest::Shoot:
            return "Shoot";
        case ActionRequest::GetBattleInfo:
            return "GetBattleInfo";
        case ActionRequest::DoNothing:
            return "DoNothing";
    }
    return "no_string";
}

std::string directionArrowString(Direction direction){
    switch (direction) {
        case Direction::U :
            return "↓";
        case Direction::UR :
            return "↘";
        case Direction::R :
            return "→";
        case Direction::DR :
            return "↗";
        case Direction::D :
            return "↑";
        case Direction::DL :
            return "↖";
        case Direction::L :
            return "←";
        case Direction::UL :
            return "↙";   
    }
    return "no_string";
}


Direction rotateQuarterLeft(Direction curr_dir){
    // Rotating a quarter to the right
    switch (curr_dir) {
        case Direction::U :
            return Direction::R;
            break;
        case Direction::UR :
            return Direction::DR;
            break;
        case Direction::R :
            return Direction::D;
            break;
        case Direction::DR :
            return Direction::DL;
            break;
        case Direction::D :
            return Direction::L;
            break;
        case Direction::DL :
            return Direction::UL;
            break;
        case Direction::L :
            return Direction::U;
            break;
        case Direction::UL :
            return Direction::UR;
            break;
        default:
            return Direction::U;
    }
}

Direction rotateQuarterRight(Direction curr_dir){
    // Rotating a quarter to the left
    switch (curr_dir) {
        case Direction::U :
            return Direction::L;
            break;
        case Direction::UR :
            return Direction::UL;
            break;
        case Direction::R :
            return Direction::U;
            break;
        case Direction::DR :
            return Direction::UR;
            break;
        case Direction::D :
            return Direction::R;
            break;
        case Direction::DL :
            return Direction::DR;
            break;
        case Direction::L :
            return Direction::D;
            break;
        case Direction::UL :
            return Direction::DL;
            break;
        default:
            return Direction::U;
    }
}


Direction rotateEighthLeft(Direction curr_dir){
    // Rotating an eight to the right
    switch (curr_dir) {
        case Direction::U :
            return Direction::UR;
            break;
        case Direction::UR :
            return Direction::R;
            break;
        case Direction::R :
            return Direction::DR;
            break;
        case Direction::DR :
            return Direction::D;
            break;
        case Direction::D :
            return Direction::DL;
            break;
        case Direction::DL :
            return Direction::L;
            break;
        case Direction::L :
            return Direction::UL;
            break;
        case Direction::UL :
            return Direction::U;
            break;
        default:
            return Direction::U;
    }
}

Direction rotateEighthRight(Direction curr_dir){
    // Rotating an eight to the left
    switch (curr_dir) {
        case Direction::U :
            return Direction::UL;
            break;
        case Direction::UR :
            return Direction::U;
            break;
        case Direction::R :
            return Direction::UR;
            break;
        case Direction::DR :
            return Direction::R;
            break;
        case Direction::D :
            return Direction::DR;
            break;
        case Direction::DL :
            return Direction::D;
            break;
        case Direction::L :
            return Direction::DL;
            break;
        case Direction::UL :
            return Direction::L;
            break;
        default:
            return Direction::U;
    }
}

Direction rotate(Direction curr_dir, Direction rotation_dir) {
    switch (rotation_dir) {
        case Direction::U :
            return curr_dir;
            break;
        case Direction::UR :
            return rotateEighthRight(curr_dir);
            break;
        case Direction::R :
            return rotateQuarterRight(curr_dir);
            break;
        case Direction::DR :
            return rotateEighthRight(rotateQuarterRight(curr_dir));
            break;
        case Direction::D :
            return rotateQuarterRight(rotateQuarterRight(curr_dir));
            break;
        case Direction::DL :
            return rotateEighthLeft(rotateQuarterLeft(curr_dir));
            break;
        case Direction::L :
            return rotateQuarterLeft(curr_dir);
            break;
        case Direction::UL :
            return rotateEighthLeft(curr_dir);
            break;
        default :
            return curr_dir;
    }  
}