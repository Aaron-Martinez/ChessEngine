#include "defs.h"
#include "bitboard.hpp"
#include "engine.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <iostream>
#include <string>


using std::cout;
using std::endl;

int main() {

    exTime_t startTime = std::chrono::steady_clock::now();
    printf("start\n");
    initAll();
    run();
    
    exTime_t endTime = std::chrono::steady_clock::now();
    utils::printExTime(startTime, endTime, "Total execution time:");

    printf("\ndone\n");
    return 0;
}