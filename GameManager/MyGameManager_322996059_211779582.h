#pragma once

#include "../common/TankAlgorithm.h"
#include "../common/Player.h"
#include "../common/AbstractGameManager.h"
#include "../common/GameResult.h"
#include "../common/ActionRequest.h"
#include "../common/GameManagerRegistration.h"
#include "../UserCommon/GameObject.h"
#include "../UserCommon/Shell.h"
#include "../UserCommon/GameUtilis.h"
#include "../UserCommon/MySatelliteView.h"
#include "Tank.h"
#include "Wall.h"
#include "Mine.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For std::erase, std::remove
#include <memory>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <regex>

namespace GameManager_322996059_211779582 {
using namespace UserCommon_322996059_211779582;

class MyGameManager_322996059_211779582 : public AbstractGameManager {
private:
    size_t width = 0;
    size_t height = 0;
    size_t max_steps = 0;
    size_t num_shells = 0;
    bool verbose = false;
    size_t number_of_total_tanks = 0;
    size_t living_tanks_player1 = 0;
    size_t living_tanks_player2 = 0;

    std::vector<std::unique_ptr<Tank>> tanks; 
    std::vector<std::unique_ptr<Shell>> shells; 
    std::vector<std::unique_ptr<Wall>> walls; 
    std::vector<std::unique_ptr<Mine>> mines;

    Player* player1;
    Player* player2;
    
    std::vector<std::vector<std::vector<GameObject*>>> board;

    std::vector<std::string> output_messages;
    std::string output_file_name;
    bool should_exit = false;
    GameResult game_result;

public:
    MyGameManager_322996059_211779582(bool verbose);

    // Runs the game
   GameResult run(
        size_t map_width, size_t map_height,
	    const SatelliteView& map, // <= a snapshot, NOT updated
        string map_name,
        size_t max_steps, size_t num_shells,
        Player& player1, string name1, Player& player2, string name2, 
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) override;

private:
    // Runs the actual game
    void runGame();

    // Runs the actual game with prints
    void debug_runGame();

    // Adding a GameObject to the game
    void addTank(int x, int y, Direction direction, int serial, int player,  int tank_index, std::unique_ptr<TankAlgorithm> tank_algorithm);
    void addWall(int x, int y);
    void addMine(int x, int y);
    void addShell(int x, int y, Direction direction);

    // Deleting an object from the game
    void deleteObject(GameObject* object_to_delete); // Delete object from the board and from the corresponding Vector

    // For debug purposes
    void printing_board() const;

    // Performs a step
    void step();

    void setSatelliteView(MySatelliteView* satellite) const;

    // Moves all the shells
    void moveAllShells();

    // Deletes all the objects that will collide passed each other ("mid-way" collision)
    void deleteCollidingObjects(bool only_shells);

    // Perform all tanks actions
    void performTanksAction(MySatelliteView& satellite);

    // calculate the destination cell, and perform the movement of obj to that cell
    void moveWrapper(GameObject *obj);

    // Check if obj is about to colide "mid-way". if so, add him to the colided_objects vector, to be deleted later.
    // "mid-way" example: our object is on (5, 5) and wants to move to (5, 6), and there is another object on (5, 6) that wants to move to (5, 5).
    // only_shells is true if this is a "half-step" that only shells move at.
    void markMidwayCollisions(GameObject* obj, std::vector<GameObject*>& colided_objects, bool only_shells) const;
    bool setTankDirectionOfCollision(Tank* t, Direction& dir) const; // helper function of markMidwayCollisions
    void clearBoardCollision(std::vector<GameObject*>& colided_objects);

    // Emptying board[x][y] from objects
    void emptyCell(size_t x, size_t y);

    // After the movement of objects, this function will be called to empty necessary cells (with two or more incompatible objects), weaken walls...
    void boardChecks();
    void boardChecksHelper(size_t& i, size_t& j, int& num_of_tanks, int& num_of_shells, int& num_of_walls, int& num_of_mines); //herlper function of boardChecks (num of objects in cell (i, j))

    // This function checks if all the actions are legal, and if they aren't write "bad step"
    void checkActionLegality();
    void setBadStep(const std::unique_ptr<Tank>&, bool resetBackwardCount); // helper function of checkActionLegality

    // This perform the current action of a tank
    void performAction(Tank* tank, MySatelliteView& satellite);
    void performShoot(Tank* tank); // helper function of performAction
    void performGetBattleInfo(Tank* tank, MySatelliteView& satellite); // helper function of performAction

    // We need to update the backward_count at the end of the turn
    void updateBackwardCount();

    // We need to update the shoot_cooldown count at the end of the turn
    void updateShootCooldown();

    // We need to reset the bad_action attribute at the end of the turn
    void resetBadAction();

    // We need to check if all tanks are out of shells
    bool checkSuddenDeath() const;

    // Checks if there is a winner. return true if the game ends.
    bool checkResult() const;

    //  Writing to output file
    void writeToOutputFile(const std::string& content) const;

    // Initialize the step_output_messages
    void initializeOutputMessages();

    // Writes the actions of the current step
    void writeStepActions() const;

    void setGameResult(int reason, int round_num);

    // run helper functions:
    void checkStartingWinningCondition();
    void initializeGameBoard(const SatelliteView& map, TankAlgorithmFactory& player1_tank_algo_factory, TankAlgorithmFactory& player2_tank_algo_factory);
    void resetState();
};


}