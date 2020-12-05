#ifndef POSITION_HPP
#define POSITION_HPP

#include "defs.h"
#include "table.hpp"


struct PVTable;

struct MoveHistory {
    int move;
    int castleRights;
    int enPas;
    int fiftyMove;
    U64 posKey;
};


struct Position {

    // piece lists and positions
    int pList[13][10];
    int allPieces[BRD_NUM_SQ];
    int numPieces[13];
    int bigPieces[2];
    int majPieces[2];
    int minPieces[2];
    U64 pawns[3];
    int kingSq[2];

    // other necessary position info
    int side;
    int enPas;
    int fiftyMove;
    U64 posKey;
    int castleRights;
    int material[2];                // material value
    int hisPly;                     // the number of moves made
    MoveHistory history[MAXMOVES];  // list of moves made

    // variables for search
    int searchHistory[13][BRD_NUM_SQ];
    int searchKillers[2][MAXDEPTH];
    PVTable pvTable;                // transposition table
    int pvArr[MAXDEPTH];            // principal variation
    int ply;                        // ply is the number of moves made in current search

};


void initHashKeys();

// functions to make moves
bool makeMove(int move, Position *pos);
void undoMove(Position *pos);
bool makeNullMove(Position *pos);
bool undoNullMove(Position *pos);
// bool moveExists(Position *pos, const int move);

static void removePiece(const int sq, Position *pos);
static void addPiece(const int sq, Position *pos, const int piece);
static void movePiece(const int originSQ, const int targetSQ, Position *pos);

// functions to set up position or update position info
void resetBoard(Position *pos);
void updateMaterialLists(Position *pos);
U64 generatePosKey(const Position *pos);
int parseFEN(const char *fen, Position *pos);

// debug functions
void printBoard(const Position *pos);
void mirrorBoard(Position *pos);
int checkBoard(const Position *pos);

#endif