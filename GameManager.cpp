#include "GameManager.h"

int GameManager::parseLine(const std::string& line, const std::string& key) const{
    std::regex pattern(key + R"(\s*=\s*(\d+))");
    std::smatch match;
    if (std::regex_search(line, match, pattern)) {
        return std::stoi(match[1]);
    } else {
        throw std::runtime_error("Error: Could not parse " + key + " from line: " + line);
    }    
}

void GameManager::readFirstFiveLines(std::ifstream& file){
    std::string line;
    // Line 1: Description (ignored)
    if (!std::getline(file, line)) {
        std::cout << "Error: File is empty or missing description line." << std::endl;
        should_exit = true;
        file.close();
        return;
    }
    try {
        // Line 2: MaxSteps
        if (!std::getline(file, line)) throw std::runtime_error("Missing MaxSteps line");
        max_steps = parseLine(line, "MaxSteps");

        // Line 3: NumShells
        if (!std::getline(file, line)) throw std::runtime_error("Missing NumShells line");
        num_shells = parseLine(line, "NumShells");

        // Line 4: Rows
        if (!std::getline(file, line)) throw std::runtime_error("Missing Rows line");
        height = parseLine(line, "Rows");
        
        // Line 5: Cols
        if (!std::getline(file, line)) throw std::runtime_error("Missing Cols line");
        width = parseLine(line, "Cols");
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        should_exit = true;
        file.close();
        return;
    }
}

void GameManager::readBoardLines(std::ifstream& file){
    char ch;
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width + 1; x++){
            file.get(ch); // puts a char in ch
            if ((ch == '\n' && x == width) || (file.eof() && x == width && y == 0)){
                // Exact length
                break;
            }
            else if ((ch == '\n' || file.eof()) && x < width){
                // Underflow
                break;
            }
            else if (ch != '\n' && x == width){
                // Overflow
                while (file.get(ch)) { // Skipping the rest of the line
                    if (ch == '\n'){
                        break;
                    }
                }
            }
            else if (ch == '#'){
                addWall(x, y);
            }
            else if (ch == '1') {
                addTank(x, y, Direction::L, number_of_total_tanks, 1, living_tanks_player1, tank_factory->create(1, living_tanks_player1));
                living_tanks_player1++;
                number_of_total_tanks++;
            }
            else if (ch == '2') {
                addTank(x, y, Direction::R, number_of_total_tanks, 2, living_tanks_player2, tank_factory->create(2, living_tanks_player2));
                living_tanks_player2++;
                number_of_total_tanks++;
            }
            else if (ch == '@'){
                addMine(x, y);
            }
        }
    }
}

void GameManager::checkStartingWinningCondition(){
    // Checking winning conditions already
    if (living_tanks_player1 == 0 && living_tanks_player2 == 0){  // Tie
        std::ofstream file(output_file_name);
        if (file) {
            file << "Tie, both players have zero tanks" << std::endl;
        }
        else {
            std::cout << "Error: Output file couldn't be opened" << std::endl;
        }
        file.close();
        should_exit = true;
    }
    else if (living_tanks_player1 != 0 && living_tanks_player2 == 0){ // Player 1 wins
        std::ofstream file(output_file_name);
        if (file) {
            file << "Player 1 won with " << living_tanks_player1 << " tanks still alive" << std::endl;
        }
        else {
            std::cout << "Error: Output file couldn't be opened" << std::endl;
        }
        file.close();
        should_exit = true;
    }
    else if (living_tanks_player1 == 0 && living_tanks_player2 != 0){ // Player 2 wins
        std::ofstream file(output_file_name);
        if (file) {
            file << "Player 2 won with " << living_tanks_player2 << " tanks still alive" << std::endl;
        }
        else {
            std::cout << "Error: Output file couldn't be opened" << std::endl;
        }
        file.close();
        should_exit = true;
    }
}


void GameManager::readBoard(char* input_file){
    std::ifstream file(input_file);
    if (!file) {
        std::cout << "Error: Cannot open file " << input_file << std::endl;
        should_exit = true;
        return;
    }
    // Reading first 5 lines of the instructions
    readFirstFiveLines(file);
    if(should_exit){
        return;
    }
    // initializing board size
    board = std::vector<std::vector<std::vector<GameObject*>>>(
    width, std::vector<std::vector<GameObject*>>(height));

    // initializing players
    players.push_back(player_factory->create(1, width, height, max_steps, num_shells));
    players.push_back(player_factory->create(2, width, height, max_steps, num_shells));

    // initializing the output file name
    output_file_name = "output_" + std::string(input_file);

    // ---------- READING BOARD ----------
    living_tanks_player1 = 0;
    living_tanks_player2 = 0;
    number_of_total_tanks = 0;
    readBoardLines(file);
    file.close();

    // Checking the starting winning conditions
    checkStartingWinningCondition();
    if(should_exit){
        return;
    }
    
    // Initializing step_output_messages
    output_messages.resize(number_of_total_tanks);
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
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
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

void GameManager::debug_run(){
    if (should_exit){
        return;
    }
    int sudden_death = 40;
    bool sudden_death_flag = false;
    int step_counter = 1;
    std::cout << "Initial board:" << std::endl;
    printing_board();
    std::ofstream file(output_file_name); // Opens and overwrites if exists
    file.close(); // Done — file is now created or cleared
    while(sudden_death != 0 && step_counter <= max_steps){
        std::cout << "-------------- " << "Step " << step_counter << " --------------" << std::endl;
        step();
        std::cout << "After step " << step_counter << ":" << std::endl;
        for(const std::unique_ptr<Tank>& tank : tanks){
            tank->debug_print();
        }
        printing_board();

        if (sudden_death_flag){
            sudden_death--;
        }
        if(checkResult()){ // The game ended
            return;
        }
        if(!sudden_death_flag){
            sudden_death_flag = checkSuddenDeath();
        }
        step_counter++;
    }
    if (step_counter > max_steps){ // Reached max step
        writeToOutputFile("Tie, reached max steps = " + std::to_string(max_steps) + ", player 1 has " +
        std::to_string(living_tanks_player1) + " tanks, player 2 has " + std::to_string(living_tanks_player2) + " tanks");
    }
    else{ // All tanks are out of shells
        writeToOutputFile("Tie, both players have zero shells for " + std::to_string(sudden_death) + " steps");
    }
}

// -------------------------------------------- run and step functions ------------------------------------------------

void GameManager::run(){
    if (should_exit){
        return;
    }

    int sudden_death = 40;
    bool sudden_death_flag = false;
    int step_counter = 1;

    std::ofstream file(output_file_name); // Opens and overwrites if exists
    file.close(); // Done — file is now created or cleared

    while(sudden_death != 0 && step_counter <= max_steps){
        step();
        if (sudden_death_flag){
            sudden_death--;
        }
        if(checkResult()){
            // The game ended
            return;
        }
        if(!sudden_death_flag){
            sudden_death_flag = checkSuddenDeath();
        }
        step_counter++;
    }
    if (step_counter > max_steps){
        // Reached max step
        writeToOutputFile("Tie, reached max steps = " + std::to_string(max_steps) + ", player 1 has " +
        std::to_string(living_tanks_player1) + " tanks, player 2 has " + std::to_string(living_tanks_player2) + " tanks");
    }
    else{
        // All tanks are out of shells
        writeToOutputFile("Tie, both players have zero shells for " + std::to_string(sudden_death) + " steps");
    }
    return;
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
setSatelliteView(satellite);

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
writeStepActions();
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


void GameManager::setSatelliteView(MySatelliteView& satellite) const{
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            if (board[i][j].empty()){
                satellite.setObjectAt(i, j, ' ');
                continue;
            }
            GameObject* obj = board[i][j].back();
            if(dynamic_cast<Shell*>(obj)){
                satellite.setObjectAt(i, j, '*');
            }
            else if(dynamic_cast<Mine*>(obj)){
                satellite.setObjectAt(i, j, '@');
            }
            else if(dynamic_cast<Wall*>(obj)){
                satellite.setObjectAt(i, j, '#');
            }
            else if(Tank* t = dynamic_cast<Tank*>(obj)){
                if (t->getplayer() == 1){
                    satellite.setObjectAt(i, j, '1');
                }
                else if (t->getplayer() == 2){
                    satellite.setObjectAt(i, j, '2');
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


void GameManager::emptyCell(int x, int y){
    // Make a copy so we can modify the original during iteration
    std::vector<GameObject*> toDelete = board[x][y];
    for (GameObject* obj : toDelete){
        deleteObject(obj);
    }
    board[x][y].clear();
}

void GameManager::boardChecksHelper(int& i, int& j, int& num_of_tanks, int& num_of_shells, int& num_of_walls, int& num_of_mines){
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
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
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
        players[0]->updateTankWithBattleInfo(*tank->getTankAlgorithm(), satellite);
    }
    else{
        // The tank is of player 2
        players[1]->updateTankWithBattleInfo(*tank->getTankAlgorithm(), satellite);
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
        writeToOutputFile("Tie, both players have zero tanks");
        return true;
    }
    else if ((living_tanks_player1 == 0) && (living_tanks_player2 != 0)){
        writeToOutputFile("Player 2 won with " + std::to_string(living_tanks_player2) + " tanks still alive");
        return true;
    }
    else if ((living_tanks_player1 != 0) && (living_tanks_player2 == 0)){
        writeToOutputFile("Player 1 won with " + std::to_string(living_tanks_player1) + " tanks still alive");
        return true;
    }
    return false;
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
