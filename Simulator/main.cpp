#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include "Simulator.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "CommandLineParser.h"
#include "../common/GameResult.h"
#include "../UserCommon/GameUtilis.h"



int main(int argc, char** argv) {
    CommandLineParser parser(argc, argv);
    if (!parser.parse()) {
        parser.printUsageWithErrors();
        return 1;
    }

    const SimulationConfig config = parser.getConfig();
    Simulator simulator(config);
    simulator.run();
}