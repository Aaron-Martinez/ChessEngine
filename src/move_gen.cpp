#include "move_gen.hpp"
#include "utils.hpp"


bool isSlidingPiece[13] = {false, false, false, true, true, true, false, false, false, true, true, true, false};
int slidingPiecesIndex[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
int nonSlidingPiecesIndex[2][2] = {{wN, wK}, {bN, bK}};

// move directions for each piece
const int knightDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int bishopDir[8] = {-9, -11, 11, 9};
const int rookDir[8] = {-1, -10, 1, 10};
const int kingDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};
const int wpDir = 10;
const int wpCapLeft = 9;
const int wpCapRight = 11;
const int bpDir = -10;
const int bpCapLeft = -9;
const int bpCapRight = -11;
/// bigPieceDirs could replace the other arrays?
const int bigPieceDirs[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, // white knight
    {-9, -11, 11, 9, 0, 0, 0, 0},       // white bishop
    {-1, -10, 1, 10, 0, 0, 0, 0},       // white rook
    {-1, -10, 1, 10, -9, -11, 11, 9},   // white queen
    {-1, -10, 1, 10, -9, -11, 11, 9},   // white king
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, // black knight
    {-9, -11, 11, 9, 0, 0, 0, 0},       // black bishop
    {-1, -10, 1, 10, 0, 0, 0, 0},       // black rook
    {-1, -10, 1, 10, -9, -11, 11, 9},   // black queen
    {-1, -10, 1, 10, -9, -11, 11, 9}    // black king
};
const int bigPieceNumDirs[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

// move flag bit masks
const int moveFlagEP = 0x40000;
const int moveFlagPawnDouble = 0x80000;
const int moveFlagCastles = 0x3000000;
const int moveFlagIsCapture = 0x7c000;
const int moveFlagIsPromote = 0xf00000;

// for move ordering using MVV-LVA to reduce alpha beta search space
const int victimScores[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
static int mvvLvaScores[13][13];


void initMvvLva() {
    int attacker;
    int victim;
    for(attacker = wP; attacker <= bK; ++attacker) {
        for(victim = wP; victim <= bK; ++victim) {
            mvvLvaScores[victim][attacker] = victimScores[victim] + 6 - (victimScores[attacker] / 100);
        }
    }
}

int createMove(int originSQ, int targetSQ, int capturedPiece, int promotedPiece, int flags) {
    // flags include en passant, pawn 2sq push, and castles
    int move = originSQ | (targetSQ << 7) | (capturedPiece << 14) | (promotedPiece << 20) | flags;
    return move;
}

bool moveExists(Position *pos, const int move) {
    
    MoveList list[1];
    generateAllMoves(pos, list);
    int moveNum = 0;
    int tempMove = 0;
    
    for(moveNum = 0; moveNum < list->count; ++moveNum) {
        tempMove = list->moves[moveNum].move;
        if(tempMove != move) {
            continue;
        }
        if(makeMove(tempMove, pos)) {
            undoMove(pos);
            return true;
        }
    }
    return false;
}

/// optimize later
void generateAllMoves(const Position *pos, MoveList *list) {
    
    ASSERT(checkBoard(pos));
    int side = pos->side;
    list->count = 0;

    // generate pawn moves
    if(side == WHITE) {
        for(int pieceIndex = 0; pieceIndex < pos->numPieces[wP]; ++pieceIndex) {
            int sq = pos->pList[wP][pieceIndex];
            ASSERT(utils::sqOnBoard(sq));

            if(pos->allPieces[sq + wpDir] == EMPTY) {
                addWhitePawnMove(pos, sq, sq + wpDir, list);
                if(ranksArr[sq] == RANK_2 && pos->allPieces[sq + 2*wpDir] == EMPTY) {
                    // pawn can push two squares
                    addQuietMove(pos, createMove(sq, sq + 2*wpDir, EMPTY, EMPTY, moveFlagPawnDouble), list);
                }
            }

            // white pawn capture moves
            if(utils::sqOnBoard(sq + wpCapLeft) && pieceColor[pos->allPieces[sq + wpCapLeft]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq + wpCapLeft, pos->allPieces[sq + wpCapLeft], list);
            }
            if(utils::sqOnBoard(sq + wpCapRight) && pieceColor[pos->allPieces[sq + wpCapRight]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq + wpCapRight, pos->allPieces[sq + wpCapRight], list);
            }
            if(pos->enPas != NO_SQ && pos->enPas != OFFBOARD) {
                if(sq + wpCapRight == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + wpCapRight, EMPTY, EMPTY, moveFlagEP), list);
                }
                if(sq + wpCapLeft == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + wpCapLeft, EMPTY, EMPTY, moveFlagEP), list);
                }
            }

        }
    }
    else {
        for(int pieceIndex = 0; pieceIndex < pos->numPieces[bP]; ++pieceIndex) {
            int sq = pos->pList[bP][pieceIndex];
            ASSERT(utils::sqOnBoard(sq));

            if(pos->allPieces[sq + bpDir] == EMPTY) {
                addBlackPawnMove(pos, sq, sq + bpDir, list);
                if(ranksArr[sq] == RANK_7 && pos->allPieces[sq + 2*bpDir] == EMPTY) {
                    // pawn can push two squares
                    addQuietMove(pos, createMove(sq, sq + 2*bpDir, EMPTY, EMPTY, moveFlagPawnDouble), list);
                }
            }

            // black pawn capture moves
            if(utils::sqOnBoard(sq + bpCapLeft) && pieceColor[pos->allPieces[sq + bpCapLeft]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq + bpCapLeft, pos->allPieces[sq + bpCapLeft], list);
            }
            if(utils::sqOnBoard(sq + bpCapRight) && pieceColor[pos->allPieces[sq + bpCapRight]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq + bpCapRight, pos->allPieces[sq + bpCapRight], list);
            }
            if(pos->enPas != NO_SQ && pos->enPas != OFFBOARD) {
                if(sq + bpCapRight == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + bpCapRight, EMPTY, EMPTY, moveFlagEP), list);
                }
                if(sq + bpCapLeft == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + bpCapLeft, EMPTY, EMPTY, moveFlagEP), list);
                }
            }

        }
    }

    // generate non sliding big piece moves
    for(int i = 0; i < 2; ++i) {
        int piece = nonSlidingPiecesIndex[side][i];
        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(utils::sqOnBoard(sq));
            int numDirs = bigPieceNumDirs[piece];
            for(int j = 0; j < numDirs; ++j) {
                int dir = bigPieceDirs[piece][j];
                int targetSQ = sq + dir;
                if(utils::sqOnBoard(targetSQ)) {
                    if(pos->allPieces[targetSQ] != EMPTY) {
                        if(pieceColor[pos->allPieces[targetSQ]] == side ^ 1) {
                            // opponents piece on this square
                            addCaptureMove(pos, createMove(sq, targetSQ, pos->allPieces[targetSQ], EMPTY, 0), list);
                        }
                        continue;
                    }
                    else {
                        addQuietMove(pos, createMove(sq, targetSQ, EMPTY, EMPTY, 0), list);
                    }
                }

            }
        }
    }

    // generate sliding big piece moves
    for(int i = 0; i < 3; ++i) {
        int piece = slidingPiecesIndex[side][i];

        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(utils::sqOnBoard(sq));
            int numDirs = bigPieceNumDirs[piece];

            for(int j = 0; j < numDirs; ++j) {
                int dir = bigPieceDirs[piece][j];
                int targetSQ = sq + dir;

                while(utils::sqOnBoard(targetSQ)) {
                    if(pos->allPieces[targetSQ] != EMPTY) {
                        if(pieceColor[pos->allPieces[targetSQ]] == side ^ 1) {
                            // opponents piece on this square
                            addCaptureMove(pos, createMove(sq, targetSQ, pos->allPieces[targetSQ], EMPTY, 0), list);
                        }
                        break;
                    }
                    else {
                        addQuietMove(pos, createMove(sq, targetSQ, EMPTY, EMPTY, 0), list);
                    }
                    targetSQ += dir;
                }
            }
        }
    }

    // castling
    if(side == WHITE) {
        if(pos->castleRights & W_OO  && !isSqAttacked(E1, BLACK, pos)) {
            if(pos->allPieces[F1] == EMPTY && pos->allPieces[G1] == EMPTY) {
                if(!isSqAttacked(F1, BLACK, pos) && !isSqAttacked(G1, BLACK, pos)) {
                    // White O-O allowed
                    addQuietMove(pos, createMove(E1, G1, EMPTY, EMPTY, moveFlagCastles), list);
                }
            }
        }
        if(pos->castleRights & W_OOO  && !isSqAttacked(E1, BLACK, pos)) {
            if(pos->allPieces[D1] == EMPTY && pos->allPieces[C1] == EMPTY && pos->allPieces[B1] == EMPTY) {
                if(!isSqAttacked(D1, BLACK, pos) && !isSqAttacked(C1, BLACK, pos)) {
                    // White O-O-O allowed
                    addQuietMove(pos, createMove(E1, C1, EMPTY, EMPTY, moveFlagCastles), list);
                }
            }
        }
    }
    else if(side == BLACK) {
        if(pos->castleRights & B_OO && !isSqAttacked(E8, WHITE, pos)) {
            if(pos->allPieces[F8] == EMPTY && pos->allPieces[G8] == EMPTY) {
                if(!isSqAttacked(F8, WHITE, pos) && !isSqAttacked(G8, WHITE, pos)) {
                    // Black O-O allowed
                    addQuietMove(pos, createMove(E8, G8, EMPTY, EMPTY, moveFlagCastles), list);
                }
            }
        }
        if(pos->castleRights & B_OOO && !isSqAttacked(E8, WHITE, pos)) {
            if(pos->allPieces[D8] == EMPTY && pos->allPieces[C8] == EMPTY && pos->allPieces[B8] == EMPTY) {
                if(!isSqAttacked(D8, WHITE, pos) && !isSqAttacked(C8, WHITE, pos)) {
                    // Black O-O-O allowed
                    addQuietMove(pos, createMove(E8, C8, EMPTY, EMPTY, moveFlagCastles), list);
                }
            }
        }
    }

}


void generateCaptureMoves(const Position *pos, MoveList *list) {
    
    ASSERT(checkBoard(pos));
    int side = pos->side;
    list->count = 0;

    // generate pawn moves
    if(side == WHITE) {
        for(int pieceIndex = 0; pieceIndex < pos->numPieces[wP]; ++pieceIndex) {
            int sq = pos->pList[wP][pieceIndex];
            ASSERT(utils::sqOnBoard(sq));

            // white pawn capture moves
            if(utils::sqOnBoard(sq + wpCapLeft) && pieceColor[pos->allPieces[sq + wpCapLeft]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq + wpCapLeft, pos->allPieces[sq + wpCapLeft], list);
            }
            if(utils::sqOnBoard(sq + wpCapRight) && pieceColor[pos->allPieces[sq + wpCapRight]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq + wpCapRight, pos->allPieces[sq + wpCapRight], list);
            }
            if(pos->enPas != NO_SQ && pos->enPas != OFFBOARD) {
                if(sq + wpCapRight == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + wpCapRight, EMPTY, EMPTY, moveFlagEP), list);
                }
                if(sq + wpCapLeft == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + wpCapLeft, EMPTY, EMPTY, moveFlagEP), list);
                }
            }
        }
    }
    else {
        for(int pieceIndex = 0; pieceIndex < pos->numPieces[bP]; ++pieceIndex) {
            int sq = pos->pList[bP][pieceIndex];
            ASSERT(utils::sqOnBoard(sq));

            // black pawn capture moves
            if(utils::sqOnBoard(sq + bpCapLeft) && pieceColor[pos->allPieces[sq + bpCapLeft]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq + bpCapLeft, pos->allPieces[sq + bpCapLeft], list);
            }
            if(utils::sqOnBoard(sq + bpCapRight) && pieceColor[pos->allPieces[sq + bpCapRight]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq + bpCapRight, pos->allPieces[sq + bpCapRight], list);
            }
            if(pos->enPas != NO_SQ && pos->enPas != OFFBOARD) {
                if(sq + bpCapRight == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + bpCapRight, EMPTY, EMPTY, moveFlagEP), list);
                }
                if(sq + bpCapLeft == pos->enPas) {
                    addEnPasMove(pos, createMove(sq, sq + bpCapLeft, EMPTY, EMPTY, moveFlagEP), list);
                }
            }
        }
    }

    // generate non sliding big piece moves
    for(int i = 0; i < 2; ++i) {
        int piece = nonSlidingPiecesIndex[side][i];
        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(utils::sqOnBoard(sq));
            int numDirs = bigPieceNumDirs[piece];
            for(int j = 0; j < numDirs; ++j) {
                int dir = bigPieceDirs[piece][j];
                int targetSQ = sq + dir;
                if(utils::sqOnBoard(targetSQ)) {
                    if(pos->allPieces[targetSQ] != EMPTY) {
                        if(pieceColor[pos->allPieces[targetSQ]] == side ^ 1) {
                            // opponents piece on this square
                            addCaptureMove(pos, createMove(sq, targetSQ, pos->allPieces[targetSQ], EMPTY, 0), list);
                        }
                        continue;
                    }
                }
            }
        }
    }

    // generate sliding big piece moves
    for(int i = 0; i < 3; ++i) {
        int piece = slidingPiecesIndex[side][i];

        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(utils::sqOnBoard(sq));
            int numDirs = bigPieceNumDirs[piece];

            for(int j = 0; j < numDirs; ++j) {
                int dir = bigPieceDirs[piece][j];
                int targetSQ = sq + dir;

                while(utils::sqOnBoard(targetSQ)) {
                    if(pos->allPieces[targetSQ] != EMPTY) {
                        if(pieceColor[pos->allPieces[targetSQ]] == side ^ 1) {
                            // opponents piece on this square
                            addCaptureMove(pos, createMove(sq, targetSQ, pos->allPieces[targetSQ], EMPTY, 0), list);
                        }
                        break;
                    }
                    targetSQ += dir;
                }
            }
        }
    }
}

static void addQuietMove(const Position *pos, int move, MoveList *list) {

    ASSERT(utils::sqOnBoard(getOriginSQ(move)));
    ASSERT(utils::sqOnBoard(getTargetSQ(move)));

    list->moves[list->count].move = move;
    if(pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    }
    else if(pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    }
    else {
        list->moves[list->count].score = pos->searchHistory[pos->allPieces[getOriginSQ(move)]][getTargetSQ(move)];
    }
    list->count++;
}

static void addCaptureMove(const Position *pos, int move, MoveList *list) {

    ASSERT(utils::sqOnBoard(getOriginSQ(move)));
    ASSERT(utils::sqOnBoard(getTargetSQ(move)));
    ASSERT(utils::validatePiece(getCaptured(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = mvvLvaScores[getCaptured(move)][pos->allPieces[getOriginSQ(move)]] + 1000000;
    list->count++;
}

// dont need this method?
static void addEnPasMove(const Position *pos, int move, MoveList *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}

static void addWhitePawnCapMove(const Position *pos, const int originSQ, const int targetSQ, const int cap, MoveList *list) {

    if(ranksArr[originSQ] == RANK_7) {
        // this pawn is promoting, add move for each promotion piece choice
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, wQ, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, wR, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, wB, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, wN, 0), list);
    }
    else {
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, EMPTY, 0), list);
    }

}

static void addWhitePawnMove(const Position *pos, const int originSQ, const int targetSQ, MoveList *list) {
    if(ranksArr[originSQ] == RANK_7) {
        // this pawn is promoting, add move for each promotion piece choice
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, wQ, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, wR, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, wB, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, wN, 0), list);
    }
    else {
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, EMPTY, 0), list);
    }
}

static void addBlackPawnCapMove(const Position *pos, const int originSQ, const int targetSQ, const int cap, MoveList *list) {

    if(ranksArr[originSQ] == RANK_2) {
        // this pawn is promoting, add move for each promotion piece choice
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, bQ, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, bR, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, bB, 0), list);
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, bN, 0), list);
    }
    else {
        addCaptureMove(pos, createMove(originSQ, targetSQ, cap, EMPTY, 0), list);
    }

}

static void addBlackPawnMove(const Position *pos, const int originSQ, const int targetSQ, MoveList *list) {
    if(ranksArr[originSQ] == RANK_2) {
        // this pawn is promoting, add move for each promotion piece choice
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, bQ, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, bR, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, bB, 0), list);
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, bN, 0), list);
    }
    else {
        addQuietMove(pos, createMove(originSQ, targetSQ, EMPTY, EMPTY, 0), list);
    }
}

int getOriginSQ(int move) {
    return (move & 0x7f);
}

int getTargetSQ(int move) {
    return ((move >> 7) & 0x7f);
}

int getCaptured(int move) {
    return ((move >> 14) & 0xf);
}

int getPromoted(int move) {
    return ((move >> 20) & 0xf);
}


// check if a particular square is attacked in a position
// possibly optimize this method later?
bool isSqAttacked(const int sq, const int side, const Position *pos) {

    ASSERT(utils::sqOnBoard(sq));
    ASSERT(utils::validateSide(side));
    //ASSERT(checkBoard(pos));

    // pawn attacks
    if(side == WHITE) {
        if(pos->allPieces[sq - 11] == wP || pos->allPieces[sq - 9] == wP) {
            return true;
        }
    }
    else {
        if(pos->allPieces[sq + 11] == bP || pos->allPieces[sq + 9] == bP) {
            return true;
        }
    }

    // knight attacks
    for(int i = 0; i < 8; ++i) {
        int piece = pos->allPieces[sq + knightDir[i]];
        if(piece != OFFBOARD && isPieceN[piece] && pieceColor[piece] == side) {
            return true;
        }
    }

    // rook and queen sliding attacks
    for(int i = 0; i < 4; ++i) {
        int moveDir = rookDir[i];
        int checkSq = sq + moveDir;
        int piece = pos->allPieces[checkSq];
        while(piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(isPieceRQ[piece] && pieceColor[piece] == side) {
                    return true;
                }
                // piece other than R/Q blocks this direction, break out
                break;
            }
            checkSq += moveDir;
            piece = pos->allPieces[checkSq];
        }
    }

    // bishops and queen diagonal attacks
    for(int i = 0; i < 4; ++i) {
        int moveDir = bishopDir[i];
        int checkSq = sq + moveDir;
        int piece = pos->allPieces[checkSq];
        while(piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(isPieceBQ[piece] && pieceColor[piece] == side) {
                    return true;
                }
                // else, piece other than B/Q blocks this direction
                break;
            }
            checkSq += moveDir;
            piece = pos->allPieces[checkSq];
        }
    }

    // king attacks
    for(int i = 0; i < 8; ++i) {
        int piece = pos->allPieces[sq + kingDir[i]];
        if(piece != OFFBOARD && isPieceK[piece] && pieceColor[piece] == side) {
            return true;
        }
    }

    return false;
}