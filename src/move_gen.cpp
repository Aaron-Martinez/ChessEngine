#include "move_gen.hpp"

// move directions for each piece
const int knightDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int bishopDir[8] = {-9, -11, 11, 9};
const int rookDir[8] = {-1, -10, 1, 10};
const int kingDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

// move flag bit masks
const int moveFlagEP = 0x40000;
const int moveFlagPawnDouble = 0x80000;
const int moveFlagCastles = 0x3000000;
const int moveFlagIsCapture = 0x7c000;
const int moveFlagIsPromote = 0xf00000;

int originSQ(int move) {
    return (move & 0x7f);
}

int targetSQ(int move) {
    return ((move >> 7) & 0x7f);
}

int captured(int move) {
    return ((move >> 14) & 0xf);
}

int promoted(int move) {
    return ((move >> 20) & 0xf);
}


// check if a particular square is attacked in a position
// possibly optimize this method later?
bool isSqAttacked(const int sq, const int side, const Position *pos) {

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
        if(isPieceN[piece] && pieceColor[piece] == side) {
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
        if(isPieceK[piece] && pieceColor[piece] == side) {
            return true;
        }
    }

    return false;

}