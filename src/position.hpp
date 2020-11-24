#ifndef POSITION_HPP
#define POSITION_HPP

#include "defs.h"
#include "engine.hpp"
#include "table.hpp"


struct Position {

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

    MoveHistory history[MAXMOVES];
    PVTable pvTable;
    int pvArr[MAXDEPTH];
    int searchHistory[13][BRD_NUM_SQ];
    int searchKillers[2][MAXDEPTH];

    // piece list
    int pList[13][10];

};

static void removePiece(const int sq, Position *pos);
static void addPiece(const int sq, Position *pos, const int piece);
static void movePiece(const int originSQ, const int targetSQ, Position *pos);
bool makeMove(int move, Position *pos);
void undoMove(Position *pos);
bool moveExists(Position *pos, const int move);

void initHashKeys();
void resetBoard(Position *pos);
U64 generatePosKey(const Position *pos);
int parseFEN(const char *fen, Position *pos);
void printBoard(const Position *pos);
void updateMaterialLists(Position *pos);
int checkBoard(const Position *pos);

#endif