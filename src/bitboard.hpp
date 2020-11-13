#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include "defs.h"
#include <iostream>


void initBitMasks();
void printBitboard(U64 board);
int popBit(U64 *bboard);
int countBits(U64 bboard);
void clearBit(U64& bboard, int index);
void setBit(U64& bboard, int index);

#endif