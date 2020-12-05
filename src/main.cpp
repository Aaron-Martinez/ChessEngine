#include "defs.h"
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

    run();
    
    long endTime = utils::currentTimeMillis();
    utils::printExTime(startTime, endTime, "Total execution time:");
    printf("\ndone\n");

    return 0;
}