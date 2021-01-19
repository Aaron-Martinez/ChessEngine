#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include "defs.h"


struct Position;


/*
    The move struct will contain an int encoding for the chess move, and a score given by evaluation function.
    The information required to represent a chess move are:
    
    1) Origin square - where the piece moved from
    2) Target square - where the piece moved to
    3) Capture flag - was a piece captured?
    4) En Passant flag - was this move an en passant capture?
    5) Pawn double push flag - was this move a 2 square pawn push?
    6) Promotion - did a pawn promote, and what did it promote to?
    6) Castles flag - castles kingside or queenside

    In this implementation I will use the 120 index board for origin/target squares. This means that (1) and (2)
    will require 7 bits each. The flags (4) and (5) will use 1 bit each. This already puts this at 16 bits with
    more info needed, therefore a 32-bit int will be used here to encode a chess move. This actually allows me
    to be a bit "space inefficient" with the remaining information since there are still 16 bits to play with.
    As such I will actually use 4 bits for the capture flag to not only determine if a piece is captured, but 
    also what piece was captured. Additionally I will simply use 4 bits for promotion which is enough to cover 
    all pieces in the piece type enum. Lastly I will use 1 bit each for kingside and queenside castles. In total 
    this uses 26 bits. Eventually I will try a 16 bit move encoding (see Move2)

    26 bit visual breakdown:
    0000 0000 0000 0000 0000 0000 0111 1111  -> target sq (7 bits)
    0000 0000 0000 0000 0011 1111 1000 0000  -> origin sq (7 bits)
    0000 0000 0000 0011 1100 0000 0000 0000  -> captured piece (4 bits)
    0000 0000 0000 0100 0000 0000 0000 0000  -> en passant flag (1 bit)
    0000 0000 0000 1000 0000 0000 0000 0000  -> pawn double push flag (1 bit)
    0000 0000 1111 0000 0000 0000 0000 0000  -> promoted piece (4 bits)
    0000 0011 0000 0000 0000 0000 0000 0000  -> castles (2 bits)
*/
struct Move {
    int move;
    int score;
};


/*
    This page was helpful for a 16-bit chess move encoding example: https://www.chessprogramming.org/Encoding_Moves
    The idea is to use only 6 bits each for origin/target squares using a 64 index board. Then with 4 remaining bits
    to represent the rest of the required information: 1 promotion bit, 1 captures bit, and 2 special bits which  
    will contain information like what piece a pawn promoted to, was a capture move an en passant, and castling. 
    Note that the reduced size of this encoding compared to 32 bit encoding could come at the cost of search 
    efficiency, since certain information is left out (the type of piece captured, for example), and since the 4 
    "special bits" will take a few extra steps to interpret. I would like to test this tradeoff myself.
*/
struct Move2 {
    short move;
    int score;
};


struct MoveList {
    Move moves[MAXPOSITIONMOVES];
    int count;
};


void initMvvLva();

void generateAllMoves(const Position *pos, MoveList *list);
void generateCaptureMoves(const Position *pos, MoveList *list);

int createMove(int originSQ, int targetSQ, int capturedPiece, int promotedPiece, int flags);
bool moveExists(Position *pos, const int move);
bool isSqAttacked(const int sq, const int side, const Position *pos);
bool isCheckmate(Position *pos);

// Add moves to move list
static void addQuietMove(const Position *pos, int move, MoveList *list);
static void addCaptureMove(const Position *pos, int move, MoveList *list);
static void addEnPasMove(const Position *pos, int move, MoveList *list);
static void addWhitePawnCapMove(const Position *pos, const int originSQ, const int targetSQ, const int cap, MoveList *list);
static void addWhitePawnMove(const Position *pos, const int originSQ, const int targetSQ, MoveList *list);
static void addBlackPawnCapMove(const Position *pos, const int originSQ, const int targetSQ, const int cap, MoveList *list);
static void addBlackPawnMove(const Position *pos, const int originSQ, const int targetSQ, MoveList *list);

// these methods extract specific information from a move
int getOriginSQ(int move);
int getTargetSQ(int move);
int getCaptured(int move);
int getPromoted(int move);


#endif