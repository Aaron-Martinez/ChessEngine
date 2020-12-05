#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include "defs.h"
#include <iostream>


void initBitMasks();
void initBitboards();

// bitboard creating functions
U64 forwardRanksBB(int color, int sq);
U64 adjacentFilesBB(int sq);

// evaluation related bitboard functions
bool isPawnIsolated(const U64 pawns[3], int sq, int color);
bool isPawnPassed(const U64 pawns[3], int sq, int color);
bool isOpenFile(const U64 pawns[3], int sq);
bool isSemiOpenFile(const U64 pawns[3], int sq, int color);

// bit operations
void clearBit(U64& bboard, int index);
void setBit(U64& bboard, int index);
int popBit(U64 *bboard);
int countBits(U64 bboard);

void printBitboard(U64 board);


#endif