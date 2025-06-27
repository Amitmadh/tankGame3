#include "TankAlgorithm2.h"
#include <string>

TankAlgorithm2::TankAlgorithm2(int player_index, int tank_index) : MyTankAlgorithm(player_index, tank_index) {}

void TankAlgorithm2::updateBattleInfo(BattleInfo& info) {
    battle_info = (MyBattleInfo2&)info;
    MyTankAlgorithm::updateBattleInfo(info);
}

void TankAlgorithm2::updateTurnsToGetBattleInfo() {
    if (battle_info.getFreindlyTanks().size() == 1) return;
    else if (turns_to_get_battle_info <= -1) {
        int counter = 0;
        for (size_t y = 0; y < board.size(); y++) {
            for (size_t x = 0; x < board[y].size(); x++) {
                if (board[y][x] == '2') {
                    counter++;
                } else if (board[y][x] == '%') {
                    turns_to_get_battle_info = counter;
                }
            }
        }
    } else if (turns_to_get_battle_info == 0) {
        turns_to_get_battle_info = battle_info.getFreindlyTanks().size() - 1;
    } else {
        turns_to_get_battle_info--;
    }
}

ActionRequest TankAlgorithm2::getAction() {
    updateTurnsToGetBattleInfo();
    is_only_tank = (battle_info.getFreindlyTanks().size() <= 1);
    turn_number++;

    //update shoot cooldown
    if (shoot_cooldown > 0) shoot_cooldown--;

    //check if its turn to update battle info
    if (is_only_tank && turn_number % 2 == 0) {
        return getBattleInfo();
    } else if (!is_only_tank && turns_to_get_battle_info <= 0) {
        return getBattleInfo();
    } 
    
    //if attacked escape from shell
    if (isAttacked()) {
        return escapeShell();
    }

    //if theres an enemy tank infront shoot
    if (isTankInfront(battle_info) && shoot_cooldown == 0 && num_of_shells > 0) {
        return shoot();
    }

    //use bfs to find the direction the tank should move towards the enemy tank
    //if the tank is facing the enemy tank, move forward
    return findDirectionTowardsEnemy();
}

//check if the location is free (not occupied by same player tank, wall or mine)
bool TankAlgorithm2::isFreePosition(std::pair<int, int> position) const {
    char c = board[position.second][position.first];
    if (c == '2' || c == '#' || c == '@' || c == '*') return false;
    else return true;
}

/// BFS algorithm to find the direction towards the enemy tank
ActionRequest TankAlgorithm2::findDirectionTowardsEnemy(){
    struct Step {
        std::pair<int, int> position;
        Direction first_direction;
    };

    bool is_first_dir = true;
    bool is_free_position = true;

    std::vector<std::vector<bool>> visited(board_height, std::vector<bool>(board_width, false));
    std::vector<Step> queue;

    //push the initial position of the tank into the queue
    Step curr_step;
    curr_step.position = tank_position;
    queue.push_back(curr_step);
    visited[curr_step.position.second][curr_step.position.first] = true;

    //will be used to keep current step neighbor cells
    std::pair<int, int> new_position;

    while(!queue.empty()) {
        curr_step = queue.front();
        queue.erase(queue.begin());

        for(Direction dir : directions) {
            new_position = move(curr_step.position, board_width, board_height, dir);
            if (visited[new_position.second][new_position.first] == true) {
                continue;
            }
            visited[new_position.second][new_position.first] = true;            
            is_free_position = isFreePosition(new_position);
            if (!is_free_position) {
                is_free_position = true;
                continue;
            }
            Step next_step;
            next_step.position = new_position;
            
            if (is_first_dir) curr_step.first_direction = dir; //update direction
            next_step.first_direction = curr_step.first_direction; 

            for (std::pair<int, int> enemy_tank_position : battle_info.getEnemyTanks()) {
                if (new_position == enemy_tank_position) {
                    Direction dest_dir = curr_step.first_direction;
                    return changeDirection(dest_dir);;
                }
            }
            queue.push_back(next_step);
        }
        is_first_dir = false;
    }
    // Default action if no enemy tank is found
    return shoot();
}

//return The action needed to change the direction of the tank to the destination direction
ActionRequest TankAlgorithm2::changeDirection(Direction dest) {
    int dest_dir = (int)dest;
    int change_dir = (int)dest_dir - (int)tank_direction;

    if (change_dir == 0) {
        //default action if the tank is already in the right direction
        if (isDirectionAvailible(Direction::U)) {
            return moveForward();
        } else {
            return doNothing();
        }
    } else if (change_dir == -1 || change_dir == 7) {
        return rotateTankEighthRight();
    } else if (change_dir == 1 || change_dir == -7) {
        return rotateTankEighthLeft();
    } else if (change_dir == -2 || change_dir == 6) {
        return rotateTankQuarterRight();
    } else if (change_dir == 2 || change_dir == -6) {
        return rotateTankQuarterLeft();
    } else if (change_dir == -3 || change_dir == 5) {
        return rotateTankQuarterRight();
    } else if (change_dir == 3 || change_dir == -5) {
        return rotateTankQuarterLeft();
    } else {
        return rotateTankQuarterRight();
    }
}

bool TankAlgorithm2::isAttacked() const {
    if (isAttackedFromDir(Direction::U) && last_action_was_shoot) return false;
    for (Shell shell : battle_info.getShellsObject()) {
        std::pair<int, int> position(shell.getX(), shell.getY());
        for (int i = 0; i < length_to_look; i++) {
            if (position == tank_position) {
                    return true;
            }
            position = move(position, board_width, board_height, shell.getDirection());
            if (board[position.second][position.first] == '#') continue;;
        }
    }
    return false;
}

ActionRequest TankAlgorithm2::escapeShell(){
    if (isDirectionAvailible(Direction::U)) return moveForward();
    else if (isDirectionAvailible(Direction::UR)) return rotateTankEighthRight();  
    else if (isDirectionAvailible(Direction::R)) return rotateTankQuarterRight();
    else if (isDirectionAvailible(Direction::UL)) return rotateTankEighthLeft();
    else if (isDirectionAvailible(Direction::L)) return rotateTankQuarterLeft();
    else if (isDirectionAvailible(Direction::D)) return rotateTankEighthLeft();
    else return rotateTankEighthLeft(); //default
}