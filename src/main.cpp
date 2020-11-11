#include <cstdlib>
#include <iostream>
#include <string>
#include "defs.h"
#include "bitboard.hpp"
#include "engine.hpp"

using std::cout;
using std::endl;

int main() {

    printf("start\n");
    initAll();
    run();

    printf("\ndone\n");
    return 0;
}