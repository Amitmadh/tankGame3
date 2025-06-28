#include <iostream>
#include <fstream>
#include <sstream>
#include "GameManager/GameManager.h"
#include "MyTankAlgorithmFactory.h"
#include "MyPlayerFactory.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n" << std::endl;
        return 1;
    }
    GameManager game(MyPlayerFactory{}, MyTankAlgorithmFactory{});
    game.readBoard(argv[1]);
	game.run();
}