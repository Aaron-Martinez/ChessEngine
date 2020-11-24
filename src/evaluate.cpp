#include "evaluate.hpp"
#include "utils.hpp"


// temporary static value tables to help piece positioning for initial basic eval
const int pawnTable[64] = {
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
    10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
    5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
    0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
    5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
    10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
    20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int knightTable[64] = {
    0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
    0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
    0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
    0	,	5	,	10	,	20	,	20	,	10	,	5	,	0	,
    5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
    5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int bishopTable[64] = {
    0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int rookTable[64] = {
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

// mirror indexing for above tables to use from black perspective
const int mirror64[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
     8,  9, 10, 11, 12, 13, 14, 15,
     0,  1,  2,  3,  4,  5,  6,  7
};



int evaluate(const Position *pos) {

    int piece;
    int pieceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];
    
    piece = wP;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score += pawnTable[index120to64[sq]];
    }

	piece = bP;
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score -= pawnTable[mirror64[index120to64[sq]]];
    }
	
	piece = wN;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score += knightTable[index120to64[sq]];
	}

	piece = bN;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score -= knightTable[mirror64[index120to64[sq]]];
	}
	
	piece = wB;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score += bishopTable[index120to64[sq]];
	}

	piece = bB;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score -= bishopTable[mirror64[index120to64[sq]]];
	}

	piece = wR;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score += rookTable[index120to64[sq]];
	}

	piece = bR;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score -= rookTable[mirror64[index120to64[sq]]];
	}

    //printf("returning score of %d for side %d\n", score, pos->side);
    if(pos->side == WHITE) {
        return score;
    }
    else {
        return -score;
    }
}


