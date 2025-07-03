#include "GameManager.h"

using namespace GameManager_322996059_211779582;
REGISTER_GAME_MANAGER(GameManager)

namespace GameManager_322996059_211779582 {

GameManager::GameManager(bool verbose) : verbose(verbose) {}


void GameManager::checkStartingWinningCondition(){
    bool tie = living_tanks_player1 == 0 && living_tanks_player2 == 0;
    bool player1_wins = living_tanks_player1 != 0 && living_tanks_player2 == 0;
    bool player2_wins = living_tanks_player1 == 0 && living_tanks_player2 != 0;
    if (tie || player1_wins || player2_wins){
        setGameResult(0, 0);
        should_exit = true;
        if(verbose){
            std::ofstream file(output_file_name);
            if (!file) {
            std::cout << "Error: Output file couldn't be opened" << std::endl;
            }
            else{
                if (tie){
                   file << "Tie, both players have zero tanks" << std::endl;
                }
                else if(player1_wins){
                   file << "Player 1 won with " << living_tanks_player1 << " tanks still alive" << std::endl;
                }
                else if(player2_wins){
                    file << "Player 2 won with " << living_tanks_player2 << " tanks still alive" << std::endl;
                }
                file.close();
            }
        }
    }
}


void GameManager::initializeGameBoard(const SatelliteView& map, TankAlgorithmFactory& player1_tank_algo_factory, TankAlgorithmFactory& player2_tank_algo_factory){
    char ch;
    for (size_t y = 0; y < height; y++){
        for (size_t x = 0; x < width; x++){
            ch = map.getObjectAt(x, y);
            if (ch == '#'){
                addWall(x, y);
            }
            else if (ch == '1') {
                addTank(x, y, Direction::L, number_of_total_tanks, 1, living_tanks_player1, player1_tank_algo_factory(1, living_tanks_player1));
                living_tanks_player1++;
                number_of_total_tanks++;
            }
            else if (ch == '2') {
                addTank(x, y, Direction::R, number_of_total_tanks, 2, living_tanks_player2, player2_tank_algo_factory(2, living_tanks_player2));
                living_tanks_player2++;
                number_of_total_tanks++;
            }
            else if (ch == '@'){
                addMine(x, y);
            }
        }
    }
}

void GameManager::resetState(){
    // Clear all objects
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            emptyCell(x, y);
        }
    }

    width = 0;
    height = 0;
    max_steps = 0;
    num_shells = 0;
    number_of_total_tanks = 0;
    living_tanks_player1 = 0;
    living_tanks_player2 = 0;
    should_exit = false;
}


// -------------------------------- Adding a game object --------------------------------
void GameManager::addTank(int x, int y, Direction direction, int serial, int player, int tank_index, std::unique_ptr<TankAlgorithm> tank_algorithm){
    std::unique_ptr<Tank> tank = std::make_unique<Tank>(x, y, direction, serial, player, tank_index, num_shells, std::move(tank_algorithm));
    Tank* raw = tank.get();
    tanks.push_back(std::move(tank));
    board[x][y].push_back(raw);
    return;
}
void GameManager::addWall(int x, int y){
    std::unique_ptr<Wall> wall = std::make_unique<Wall>(x, y);
    Wall* raw = wall.get();
    walls.push_back(std::move(wall));
    board[x][y].push_back(raw);
    return;
}
void GameManager::addMine(int x, int y){
    std::unique_ptr<Mine> mine = std::make_unique<Mine>(x, y);
    Mine* raw = mine.get();
    mines.push_back(std::move(mine));
    board[x][y].push_back(raw);
    return;
}
void GameManager::addShell(int x, int y, Direction direction){
    std::unique_ptr<Shell> shell = std::make_unique<Shell>(x, y, direction);
    Shell* raw = shell.get();
    shells.push_back(std::move(shell));
    board[x][y].push_back(raw);
    return;
}


// ---------------------- Deletions and deconstractor ------------------------------------
template<typename T>
bool eraseByRawPointer(std::vector<std::unique_ptr<T>>& vec, T* ptr) {
    // This function was generated using ChatGPT
    auto it = std::find_if(vec.begin(), vec.end(),
        [&](const std::unique_ptr<T>& uptr) {
            return uptr.get() == ptr;
        });
    if (it != vec.end()) {
        vec.erase(it); // Destroys the unique_ptr ⇒ deletes the object
        return true;
    }
    return false;
}


void GameManager::deleteObject(GameObject* object_to_delete) {
    // Step 1: Remove from the board
    auto& cell = board[object_to_delete->getX()][object_to_delete->getY()];
    cell.erase(std::remove(cell.begin(), cell.end(), object_to_delete), cell.end());

    // Step 2: Remove from the appropriate container
    if (Tank* t = dynamic_cast<Tank*>(object_to_delete)) {
        output_messages[t->getSerial()] += " (killed)";
        if(t->getplayer() == 1){
            living_tanks_player1--;
        }
        else if(t->getplayer() == 2){
            living_tanks_player2--;
        }
        eraseByRawPointer(tanks, t);
    } else if (Shell* s = dynamic_cast<Shell*>(object_to_delete)) {
        eraseByRawPointer(shells, s);
    } else if (Wall* w = dynamic_cast<Wall*>(object_to_delete)) {
        eraseByRawPointer(walls, w);
    } else if (Mine* m = dynamic_cast<Mine*>(object_to_delete)) {
        eraseByRawPointer(mines, m);
    }
}

// ------------------------------- For debug purposes -----------------------------------------
void GameManager::printing_board() const{
    for (size_t y = 0; y < height; y++){
        for (size_t x = 0; x < width; x++){
            if (board[x][y].empty()){
                std::cout << "_";
                continue;
            }
            GameObject* obj = board[x][y].back();
            if (Tank* t = dynamic_cast<Tank*>(obj)) {
                std::cout << t->getplayer();
            } else if (Shell* s = dynamic_cast<Shell*>(obj)) {
                std::cout << directionArrowString(s->getDirection());
            } else if (dynamic_cast<Wall*>(obj)) {
                std::cout << "#";
            } else if (dynamic_cast<Mine*>(obj)) {
                std::cout << "@";
            }
        }
    std::cout << std::endl;
    }
}

void GameManager::debug_runGame(){
    int sudden_death = 40;
    int sudden_death_counter = 0;
    bool sudden_death_flag = false;
    size_t step_counter = 1;
    if(verbose){
        std::ofstream file(output_file_name); // Opens and overwrites if exists
        file.close(); // Done — file is now created or cleared
    }
    std::cout << "map_width: " << width << ", map_height: " << height << ", max_steps: " << max_steps << ", num_shells: " << num_shells << ", Initial board:" << std::endl;
    printing_board();
    while(sudden_death_counter != sudden_death && step_counter <= max_steps){
        std::cout << "-------------- " << "Step " << step_counter << " --------------" << std::endl;
        step();
        std::cout << "After step " << step_counter << ":" << std::endl;
        for(const std::unique_ptr<Tank>& tank : tanks){
            tank->debug_print();
        }
        printing_board();
        if (sudden_death_flag){
            sudden_death_counter++;
        }
        if(checkResult()){
            // The game ended
            setGameResult(0, step_counter);
            return;
        }
        if(!sudden_death_flag){
            sudden_death_flag = checkSuddenDeath();
        }
        step_counter++;
    }
    if (step_counter > max_steps){
        // Reached max step
        setGameResult(1, max_steps);
        if(verbose){
            writeToOutputFile("Tie, reached max steps = " + std::to_string(max_steps) + ", player 1 has " +
            std::to_string(living_tanks_player1) + " tanks, player 2 has " + std::to_string(living_tanks_player2) + " tanks");
        }
    }
    else{
        // All tanks are out of shells
        step_counter--; // becasue we added an extra step at the end
        setGameResult(2, step_counter);
        if(verbose){
            writeToOutputFile("Tie, both players have zero shells for " + std::to_string(sudden_death) + " steps");
        }
    }
}

// -------------------------------------------- run and step functions ------------------------------------------------
GameResult GameManager::run(
        size_t map_width, size_t map_height,
	    const SatelliteView& map, // <= a snapshot, NOT updated
        string map_name,
        size_t max_steps, size_t num_shells,
        Player& player1, string name1, Player& player2, string name2, 
        TankAlgorithmFactory player1_tank_algo_factory,
        TankAlgorithmFactory player2_tank_algo_factory) {

    width = map_width;
    height = map_height;
    this->max_steps = max_steps;
    this->num_shells = num_shells;
    this->player1 = &player1;
    this->player2 = &player2;

    // initializing board size
    board = std::vector<std::vector<std::vector<GameObject*>>>(
    width, std::vector<std::vector<GameObject*>>(height));

    if(verbose){ // initializing the output file name
        output_file_name = "output_" + map_name + "_" + name1 + "_" + name2 + "_" + generateTimeBasedString() + ".txt"; // TODO
    }

    initializeGameBoard(map, player1_tank_algo_factory, player2_tank_algo_factory);

    // Checking the starting winning conditions
    checkStartingWinningCondition();
    if(should_exit){
        resetState();
        return std::move(game_result);
    }
    
    // Initializing step_output_messages
    output_messages.resize(number_of_total_tanks);

    debug_runGame();
    
    resetState();
    return std::move(game_result);
}


void GameManager::runGame(){
    int sudden_death = 40;
    int sudden_death_counter = 0;
    bool sudden_death_flag = false;
    size_t step_counter = 1;
    if(verbose){
        std::ofstream file(output_file_name); // Opens and overwrites if exists
        file.close(); // Done — file is now created or cleared
    }
    while(sudden_death_counter != sudden_death && step_counter <= max_steps){
        step();
        if (sudden_death_flag){
            sudden_death_counter++;
        }
        if(checkResult()){
            // The game ended
            setGameResult(0, step_counter);
            return;
        }
        if(!sudden_death_flag){
            sudden_death_flag = checkSuddenDeath();
        }
        step_counter++;
    }
    if (step_counter > max_steps){
        // Reached max step
        setGameResult(1, max_steps);
        if(verbose){
            writeToOutputFile("Tie, reached max steps = " + std::to_string(max_steps) + ", player 1 has " +
            std::to_string(living_tanks_player1) + " tanks, player 2 has " + std::to_string(living_tanks_player2) + " tanks");
        }
    }
    else{
        // All tanks are out of shells
        step_counter--; // becasue we added an extra step at the end
        setGameResult(2, step_counter);
        if(verbose){
            writeToOutputFile("Tie, both players have zero shells for " + std::to_string(sudden_death) + " steps");
        }
    }
}


void GameManager::step(){
    initializeOutputMessages();
    // Foreach tank, we ask the corresponsing TankAlgorithm for an action
    for(const std::unique_ptr<Tank>& tank : tanks){
        tank->setActionToPerform(tank->getTankAlgorithm()->getAction());
        // --------------------- REMEMBER TO DETELE HERE ----------------------------------
        output_messages[tank->getSerial()] = stringAction(tank->getActionToPerform());
    }

    // We now check each Tank action if it is a legal action
    checkActionLegality();

    // Preparing SatelliteView
    MySatelliteView satellite(width, height);
    setSatelliteView(&satellite);

    // ======= First half-step ========
    // First we account for shells "mid-way" collisions
    deleteCollidingObjects(true);

    // Move all shells, and then check for board collisions on the same square
    moveAllShells();
    boardChecks();

    // ======= Second half-step ========
    // We account for shells and tanks "mid-way" collisions
    deleteCollidingObjects(false);

    // Perform all tanks actions, and move all shells. Then check for board collisions on the same square
    moveAllShells();
    performTanksAction(satellite);
    boardChecks();

    // updates to do at the end of each step
    updateBackwardCount();
    updateShootCooldown();
    resetBadAction();
    if (verbose){
        writeStepActions();
    }
}

// ------------------------------------------------ Utilis ----------------------------------------------------

// Deletes all the objects that will collide passed each other ("mid-way" collision)
void GameManager::deleteCollidingObjects(bool only_shells){
    // We initialize a vector, that will contain all the objects that colided "mid-way".
    // Once we populate the vector, all objects in it will need to be deleted
    std::vector<GameObject*> colided_objects;
    for (const std::unique_ptr<Shell>& shell : shells) {
        markMidwayCollisions(shell.get(), colided_objects, only_shells);
    }
    if (!only_shells){
        for (const std::unique_ptr<Tank>& tank : tanks) {
            markMidwayCollisions(tank.get(), colided_objects, only_shells);
        }
    }
    clearBoardCollision(colided_objects);
}


void GameManager::setSatelliteView(MySatelliteView* satellite) const{
    for (size_t x = 0; x < width; x++){
        for (size_t y = 0; y < height; y++){
            if (board[x][y].empty()){
                satellite->setObjectAt(x, y, ' ');
                continue;
            }
            GameObject* obj = board[x][y].back();
            if(dynamic_cast<Shell*>(obj)){
                satellite->setObjectAt(x, y, '*');
            }
            else if(dynamic_cast<Mine*>(obj)){
                satellite->setObjectAt(x, y, '@');
            }
            else if(dynamic_cast<Wall*>(obj)){
                satellite->setObjectAt(x, y, '#');
            }
            else if(Tank* t = dynamic_cast<Tank*>(obj)){
                if (t->getplayer() == 1){
                    satellite->setObjectAt(x, y, '1');
                }
                else if (t->getplayer() == 2){
                    satellite->setObjectAt(x, y, '2');
                }
            }
        }
    }
}

// Moves all the shells
void GameManager::moveAllShells(){
    for (const std::unique_ptr<Shell>& shell : shells) {
        moveWrapper(shell.get());
    }
}


// Perform all tanks actions
void GameManager::performTanksAction(MySatelliteView& satellite){
    for (const std::unique_ptr<Tank>& tank : tanks) {
        performAction(tank.get(), satellite);
    }
}


void GameManager::moveWrapper(GameObject *obj){
    Direction direc;
    // getting the object (tank or shell) direction
    if (Tank* t = dynamic_cast<Tank*>(obj)) {
        if (t->getActionToPerform() == ActionRequest::MoveBackward){
            direc = oppositeDirection(t->getDirection());
        }
        else if(t->getActionToPerform() == ActionRequest::MoveForward){
            direc = t->getDirection();
        }
    }
    else if (Shell* s = dynamic_cast<Shell*>(obj)){
        direc = s->getDirection();
    }
    else{
        std::cout << "moveWrapper function misuse! not a tank or a shell!" << std::endl;
        return;
    }

    // we get the new location
    std::pair<int, int> new_loc_arr = move(std::make_pair(obj->getX(), obj->getY()), width, height, direc);

    // we move object to new location
    int new_x = new_loc_arr.first;
    int new_y = new_loc_arr.second;

    // we delete object from previous cell
    std::vector<GameObject*>& cell = board[obj->getX()][obj->getY()];
    cell.erase(std::remove(cell.begin(), cell.end(), obj), cell.end());

    // We add obj to (new_x, new_y)
    board[new_x][new_y].push_back(obj);

    // we update obj location
    obj->setY(new_y);
    obj->setX(new_x);
}

bool GameManager::setTankDirectionOfCollision(Tank* t, Direction& dir) const{
    ActionRequest action = t->getActionToPerform();
    int backward_count = t->getBackwardCount();
    // first we check if the tank plans to move or shoot this step
    if ((action == ActionRequest::MoveForward) && (backward_count == 0 || backward_count == 3)){
        // the tank will move forward this step
        dir = t->getDirection();
        return true;
    }
    else if ((action == ActionRequest::MoveBackward && backward_count == 3) || (backward_count == 2 && action != ActionRequest::MoveForward)){
        // the tank will move backward this step
        dir = oppositeDirection(t->getDirection());
        return true;
    }
    else if ((action == ActionRequest::Shoot) && ((backward_count == 0 || backward_count == 3))){
        // the tank will shoot a shell this step
        dir = t->getDirection();
        return true;
    }
    return false;
}

void GameManager::markMidwayCollisions(GameObject* obj, std::vector<GameObject*>& colided_objects, bool only_shells) const{
    Direction our_direction_of_movement; // The direction obj will move/shoot towards. #### First Step ####:  we want to initialize our_direction_of_movement
    if (Shell* our_shell = dynamic_cast<Shell*>(obj)){
        our_direction_of_movement = our_shell->getDirection();
    }
    else if (Tank* our_tank = dynamic_cast<Tank*>(obj)){
        if (only_shells){
            return; // it's not the tank half-step
        }
        if(!setTankDirectionOfCollision(our_tank, our_direction_of_movement)){
            return; // The tank won't move/shoot this step
        }
    }
    else {
        return; // obj isn't a tank or a shell (and therefore can't move/shoot)
    }
    std::pair<int, int> new_loc = move(std::make_pair(obj->getX(), obj->getY()), width, height, our_direction_of_movement);
    // #### Second Step ####: we need to check if there is an object on (new_loc.first, new_loc.second), that will move/shoot this step, in the opposite direction of our_direction_of_movement
    for (GameObject* obj : board[new_loc.first][new_loc.second]){
        if (Tank* their_tank = dynamic_cast<Tank*>(obj)){
            Direction their_direction_of_movement; // The direction their_tank will move/shoot towards
            if (only_shells){
                continue; // it's not the tank half-step
            }
            if(!setTankDirectionOfCollision(their_tank, their_direction_of_movement)){
                continue; // The tank won't move/shoot this step
            }
            if(oppositeDirection(their_direction_of_movement) == our_direction_of_movement){
                colided_objects.push_back(their_tank); // we add their tank to the colided objects
                return;
            }
        }
        else if (Shell* their_shell = dynamic_cast<Shell*>(obj)){
            if (oppositeDirection(their_shell->getDirection()) == our_direction_of_movement){
                colided_objects.push_back(their_shell); // we add their shell to the colided objects
                return;
            }
        }
    }
}

void GameManager::clearBoardCollision(std::vector<GameObject*>& colided_objects){
    for (GameObject* obj : colided_objects) {
        if (Tank* t = dynamic_cast<Tank*>(obj)){
            if (t->getActionToPerform() == ActionRequest::Shoot){
                // Set everything as if the tank already shot the shell
                t->setActionToPerform(ActionRequest::DoNothing);
                t->decrementShellNum();
                t->setShootCooldown(5); // 5 because we decrement it in this turn too
                continue;
            }
        }
        deleteObject(obj);
    }
}


void GameManager::emptyCell(size_t x, size_t y){
    // Make a copy so we can modify the original during iteration
    std::vector<GameObject*> toDelete = board[x][y];
    for (GameObject* obj : toDelete){
        deleteObject(obj);
    }
    board[x][y].clear();
}

void GameManager::boardChecksHelper(size_t& i, size_t& j, int& num_of_tanks, int& num_of_shells, int& num_of_walls, int& num_of_mines){
    if (num_of_walls == 1){
        // if there is a wall in the cell
        if (num_of_shells >= 2){
            // the wall should fall, and the cell should be empty
            emptyCell(i, j);
        }
        else if (num_of_shells == 1){
            // if the wall is weak we destroy it, and if it's not - we weaken it. The shell should be destroyed
            for (GameObject* object : board[i][j]){
                if(Wall* wall = dynamic_cast<Wall*>(object)){
                    if (wall->isWeak()){
                        deleteObject(wall);
                    }
                    else{
                        wall->becomeWeak();
                    }
                }
                else if (Shell* shell = dynamic_cast<Shell*>(object)){
                    deleteObject(shell);
                }
            }
        }
    }
    else if (num_of_tanks > 1){
        emptyCell(i, j);
    }
    else if (num_of_tanks + num_of_shells >= 2){
        emptyCell(i, j);
    }
    else if ((num_of_mines == 1) && (num_of_tanks != 0)){
        emptyCell(i, j);
    }
}

void GameManager::boardChecks(){
    // Foreach cell in the board
    for (size_t i = 0; i < width; i++){
        for (size_t j = 0; j < height; j++){
            int num_of_tanks = 0;
            int num_of_shells = 0;
            int num_of_walls = 0;
            int num_of_mines = 0;
            // We count the number of tanks/shells/walls/mines in the cell
            for (GameObject* obj : board[i][j]){
                if(dynamic_cast<Tank*>(obj)){
                    num_of_tanks += 1;
                }
                else if(dynamic_cast<Shell*>(obj)){
                    num_of_shells += 1;
                }
                else if(dynamic_cast<Wall*>(obj)){
                    num_of_walls += 1;
                }
                else if(dynamic_cast<Mine*>(obj)){
                    num_of_mines += 1;
                }
            }
            boardChecksHelper(i, j, num_of_tanks, num_of_shells, num_of_walls, num_of_mines);
        }
    }
}

void GameManager::setBadStep(const std::unique_ptr<Tank>& tank, bool resetBackwardCount){
    tank->setBadMove(true);
    output_messages[tank->getSerial()] += " (ignored)";
    tank->setActionToPerform(ActionRequest::DoNothing);
    if(resetBackwardCount){
        tank->setBackwardCount(0);
    }
}

void GameManager::checkActionLegality(){
    for (const std::unique_ptr<Tank>& tank : tanks){
        ActionRequest action = tank->getActionToPerform();
        int backward_count = tank->getBackwardCount();
        int x = tank->getX();
        int y = tank->getY();
        Direction direction = tank->getDirection();
        if ((action == ActionRequest::MoveForward) && (backward_count == 0 || backward_count == 3)){
            // if the tank needs to move forward, check for a wall
            std::pair<int, int> new_loc = move(std::make_pair(x, y), width, height, direction);
            for (GameObject* obj : board[new_loc.first][new_loc.second]){
                if(dynamic_cast<Wall*>(obj)){ // if this is a bad step
                    setBadStep(tank, true);
                    break;
                }
            }
        }
        else if ((action == ActionRequest::MoveBackward && backward_count == 3) || (backward_count == 2 && action != ActionRequest::MoveForward)){
            // if the tank actually needs to move backward, check for a wall
            std::pair<int, int> new_loc = move(std::make_pair(x, y), width, height, oppositeDirection(direction));
            for (GameObject* obj : board[new_loc.first][new_loc.second]){
                if(dynamic_cast<Wall*>(obj)){ // if this is a bad step
                    setBadStep(tank, true);
                    break;
                }
            }
        }
        else if (action == ActionRequest::Shoot && (backward_count == 0 || backward_count == 3)){
            // if the tank actually needs to shoot
            if ((tank->getShootCooldown() != 0) || (tank->getShellNum() == 0)){ // if this is a bad step
                setBadStep(tank, true);
            }
        }
        if((action != ActionRequest::MoveForward) && (tank->getBackwardCount() == 1 || tank->getBackwardCount() == 2)){
            // if a tank is on backword_count = 1 or backword_count = 2, and wants to perform an action (other then moving forward)
            setBadStep(tank, false);
        }
    }
}

void GameManager::performShoot(Tank* tank){
    // adding a new shell to the tank position
    std::pair<int, int> new_loc = move(std::make_pair(tank->getX(), tank->getY()), width, height, tank->getDirection());
    addShell(new_loc.first, new_loc.second, tank->getDirection());
    tank->decrementShellNum();
    tank->setShootCooldown(5); // 5 because we decrement it in this turn too
}
void GameManager::performGetBattleInfo(Tank* tank, MySatelliteView& satellite){
    int x = tank->getX();
    int y = tank->getY();
    char saved_char = satellite.getObjectAt(x, y);
    satellite.setObjectAt(x, y, '%');
    if (tank->getplayer() == 1){
        // The tank is of player 1
        player1->updateTankWithBattleInfo(*tank->getTankAlgorithm(), satellite);
    }
    else{
        // The tank is of player 2
        player2->updateTankWithBattleInfo(*tank->getTankAlgorithm(), satellite);
    }
    satellite.setObjectAt(x, y, saved_char);
}


void GameManager::performAction(Tank* tank, MySatelliteView& satellite){
    ActionRequest action = tank->getActionToPerform();
    int backward_count = tank->getBackwardCount();
    // reseting the backward_count if necessary
    if ((backward_count == 3) && (action != ActionRequest::MoveBackward)){
        tank->setBackwardCount(0);
    }
    // Now we take care of the actions
    if (action == ActionRequest::MoveForward){
        if ((backward_count == 0) || (backward_count == 3)) {
            moveWrapper(tank);
        }
        tank->setBackwardCount(0); // always happens
    }
    else if ((action == ActionRequest::MoveBackward && backward_count == 3) || (backward_count == 2 && action != ActionRequest::MoveForward)){
        tank->setActionToPerform(ActionRequest::MoveBackward);
        moveWrapper(tank);
    }
    else if (action == ActionRequest::RotateLeft45){
        tank->rotateEighth(false);
    }
    else if (action == ActionRequest::RotateRight45){
        tank->rotateEighth(true);
    }
    else if (action == ActionRequest::RotateLeft90){
        tank->rotateQuarter(false);
    }
    else if (action == ActionRequest::RotateRight90){
        tank->rotateQuarter(true);
    }
    else if (action == ActionRequest::Shoot){
        performShoot(tank);
    }
    else if (action == ActionRequest::GetBattleInfo){
        performGetBattleInfo(tank, satellite);
    }
}



void GameManager::updateBackwardCount(){
    for (const std::unique_ptr<Tank>& tank : tanks){
        int backward_count = tank->getBackwardCount();
        ActionRequest action = tank->getActionToPerform();
        if ((backward_count < 3) && ((action == ActionRequest::MoveBackward) || (backward_count > 0))){
            tank->incrementBackwardCount();
        }
    }
}

void GameManager::updateShootCooldown(){
    for (const std::unique_ptr<Tank>& tank : tanks){
        int shoot_cooldown = tank->getShootCooldown();
        if (shoot_cooldown > 0){
            tank->setShootCooldown(shoot_cooldown - 1);
        }
    }
}

void GameManager::resetBadAction(){
    for (const std::unique_ptr<Tank>& tank : tanks){
        tank->setBadMove(false);
    }
}

bool GameManager::checkSuddenDeath() const{
    for (const std::unique_ptr<Tank>& tank : tanks){
        if (tank->getShellNum() != 0){
            return false;
        }
    }
    return true;
}

bool GameManager::checkResult() const{
    if ((living_tanks_player1 == 0) && (living_tanks_player2 == 0)){
        if (verbose){
            writeToOutputFile("Tie, both players have zero tanks");
        }
        return true;
    }
    else if ((living_tanks_player1 == 0) && (living_tanks_player2 != 0)){
        if (verbose){
            writeToOutputFile("Player 2 won with " + std::to_string(living_tanks_player2) + " tanks still alive");
        }
        return true;
    }
    else if ((living_tanks_player1 != 0) && (living_tanks_player2 == 0)){
        if (verbose){
            writeToOutputFile("Player 1 won with " + std::to_string(living_tanks_player1) + " tanks still alive");
        }
        return true;
    }
    return false;
}

void GameManager::setGameResult(int reason, int round_num){
    // reason 0 - ALL_TANKS_DEAD
    // reason 1 - MAX_STEPS
    // reason 2 - ZERO_SHELLS
    game_result.remaining_tanks = {living_tanks_player1, living_tanks_player2};
    game_result.rounds = round_num;
    std::unique_ptr<MySatelliteView> game_state = std::make_unique<MySatelliteView>(width, height);
    setSatelliteView(game_state.get());
    game_result.gameState = std::move(game_state);
    if (reason == 0){
        game_result.reason = GameResult::ALL_TANKS_DEAD;
        if ((living_tanks_player1 == 0) && (living_tanks_player2 == 0)){
            game_result.winner = 0;
        }
        if ((living_tanks_player1 != 0) && (living_tanks_player2 == 0)){
            game_result.winner = 1;
        }
        if ((living_tanks_player1 == 0) && (living_tanks_player2 != 0)){
            game_result.winner = 2;
        }
    }
    else if(reason == 1){
        game_result.reason = GameResult::MAX_STEPS;
        game_result.winner = 0;
    }
    else if(reason == 2){
        game_result.reason = GameResult::ZERO_SHELLS;
        game_result.winner = 0;
    }
    else{
        std::cout << "Invalid 'reason' argument for the function 'setGameResult' - " << reason << std::endl;
    }
}

// --------------------- Function utilities that help write content to the output file ----------------------------
void GameManager::writeToOutputFile(const std::string& content) const{
    // The function was writting by ChatGPT
    std::ios_base::openmode mode;

    // If file exists, we append; otherwise, create and write from scratch
    if (std::filesystem::exists(output_file_name)) {
        mode = std::ios::app;
    } else {
        mode = std::ios::out; // this will create the file if it doesn't exist
    }

    std::ofstream file(output_file_name, mode);
    if (file.is_open()) {
        file << content << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open output file" << std::endl;
    }
}

void GameManager::initializeOutputMessages(){
    for (std::string& msg : output_messages) {
        msg = "killed";
    }
}

void GameManager::writeStepActions() const{
    std::string result;
    for (size_t i = 0; i < output_messages.size(); ++i) {
        result += output_messages[i];
        if (i != output_messages.size() - 1) {
            result += ", ";
        }
    }
    writeToOutputFile(result);
    return;
}


}