#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "defs.h"

void initAll();
void initBoardIndexConversions();
void run();
int getIndex(int file, int rank);

#endif