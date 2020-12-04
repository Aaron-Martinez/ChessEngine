#include "defs.h"
// #include "bitboard.hpp"
#include "engine.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <iostream>
#include <string>


using std::cout;
using std::endl;

int main() {

    long startTime = utils::currentTimeMillis();
    printf("start\n");
    initAll();
    run();
    
    long endTime = utils::currentTimeMillis();
    utils::printExTime(startTime, endTime, "Total execution time:");

    printf("\ndone\n");
    return 0;
}