#ifndef POSITION_HPP
#define POSITION_HPP

#include "defs.h"
#include "engine.hpp"

struct position {

    int allPieces[BRD_NUM_SQ];
    U64 pawns[3];
    int kingSq[2];
    int side;
    int enPas;
    int fiftyMove;
    int ply;
    int hisPly;
    U64 posKey;
    int numPieces[13];
    int bigPieces[2];
    int majPieces[2];
    int minPieces[2];
    int material[2];
    int castleRights;

    moveHistory history[MAXMOVES];

    // piece list
    int pList[13][10];

};

void initHashKeys();
void resetBoard(position *pos);
U64 generatePosKey(const position *pos);
int parseFEN(const char *fen, position *pos);
void printBoard(const position *pos);

#endif