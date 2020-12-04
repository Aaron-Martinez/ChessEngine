#include "evaluate.hpp"

#include "bitboard.hpp"
#include "position.hpp"
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

// todo: replace these tables with more sophisticated king safety metrics later
// for now this encourages castling in the opening
const int kingTableOpening[64] = {
      0,   5,   5, -10, -10,   0,  10,   5, 
    -10, -10, -10, -10, -10, -10, -10, -10, 
    -30, -30, -30, -30, -30, -30, -30, -30, 
    -70, -70, -70, -70, -70, -70, -70, -70, 
    -70, -70, -70, -70, -70, -70, -70, -70, 
    -70, -70, -70, -70, -70, -70, -70, -70, 
    -70, -70, -70, -70, -70, -70, -70, -70, 
    -70, -70, -70, -70, -70, -70, -70, -70
};

// need the king active in engame
const int kingTableEndgame[64] = {
    -40, -20,  0,  0,  0,  0, -20, -40, 
    -20,   0, 10, 10, 10, 10,   0, -20, 
      0,  10, 15, 15, 15, 15,  10,   0, 
     10,  10, 15, 20, 20, 15,  10,  10, 
     10,  10, 15, 20, 20, 15,  10,  10, 
      0,  10, 15, 15, 15, 15,  10,   0, 
    -20,   0, 20, 20, 20, 20,   0, -20, 
    -40, -20,  0,  0,  0,  0, -20, -40
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

int isolatedPawn = -10;
int passedPawn[8] = {0, 5, 10, 20, 35, 60, 100, 200};
int rookOpenFile = 15;
int rookSemiOpenFile = 10;
int queenOpenFile = 8;
int queenSemiOpenFile = 5;
int bishopPair;
int endgameMaterialLimit = 1460;


int evaluate(const Position *pos) {
    if(isMaterialDraw(pos)) {
        return 0;
    }
    int score = evalPieces(pos) + evalPawnStructure(pos);
    if(pos->side == WHITE) {
        return score;
    }
    else {
        return -score;
    }
}

int evalPieces(const Position *pos) {
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

    if(pos->numPieces[wB] >= 2) {
        score += bishopPair;
    }
    if(pos->numPieces[bB] >= 2) {
        score -= bishopPair;
    }

	piece = wR;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score += rookTable[index120to64[sq]];
        if(isOpenFile(pos->pawns, sq)) {
            score += rookOpenFile;
        }
        else if(isSemiOpenFile(pos->pawns, sq, WHITE)) {
            score += rookSemiOpenFile;
        }
	}

	piece = bR;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		score -= rookTable[mirror64[index120to64[sq]]];
        if(isOpenFile(pos->pawns, sq)) {
            score -= rookOpenFile;
        }
        else if(isSemiOpenFile(pos->pawns, sq, BLACK)) {
            score -= rookSemiOpenFile;
        }
	}

    piece = wQ;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
        if(isOpenFile(pos->pawns, sq)) {
            score += queenOpenFile;
        }
        else if(isSemiOpenFile(pos->pawns, sq, WHITE)) {
            score += queenSemiOpenFile;
        }
	}

	piece = bQ;	
	for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
        if(isOpenFile(pos->pawns, sq)) {
            score -= queenOpenFile;
        }
        else if(isSemiOpenFile(pos->pawns, sq, BLACK)) {
            score -= queenSemiOpenFile;
        }
	}

    //printf("returning score of %d for side %d\n", score, pos->side);
    // if(pos->side == WHITE) {
        return score;
    // }
    // else {
    //     return -score;
    // }
}


int evalPawnStructure(const Position *pos) {

    int pieceNum;
    int sq;
    int score = 0;

    int piece = wP;
    for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		
        if(isPawnIsolated(pos->pawns, sq, WHITE)) {
            score += isolatedPawn;
        }
        if(isPawnPassed(pos->pawns, sq, WHITE)) {
            score += passedPawn[ranksArr[sq]];
        }
    }

    piece = bP;
    for(pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
		sq = pos->pList[piece][pieceNum];
		ASSERT(utils::sqOnBoard(sq));
		
        if(isPawnIsolated(pos->pawns, sq, BLACK)) {
            score -= isolatedPawn;
        }
        if(isPawnPassed(pos->pawns, sq, BLACK)) {
            score -= passedPawn[7 - ranksArr[sq]];
        }
    }


    // if(pos->side == WHITE) {
        return score;
    // }
    // else {
    //     return -score;
    // }
}

int evalKings(const Position *pos) {
    
    int score = 0;

    int sq = pos->kingSq[WHITE];
    if(pos->material[BLACK] < endgameMaterialLimit) {
        score += kingTableEndgame[index120to64[sq]];
    } else {
        score += kingTableOpening[index120to64[sq]];
    }

    sq = pos->kingSq[BLACK];
    if(pos->material[WHITE] < endgameMaterialLimit) {
        score -= kingTableEndgame[mirror64[index120to64[sq]]];
    } else {
        score -= kingTableOpening[mirror64[index120to64[sq]]];
    }

    return score;
    
}

bool isMaterialDraw(const Position *pos) {
    
    if(!pos->numPieces[wR] && !pos->numPieces[bR] && !pos->numPieces[wQ] && pos->numPieces[bQ] && !countBits(pos->pawns[BOTH])) {
        if(!pos->numPieces[wB] && !pos->numPieces[bB]) {
            if( (pos->numPieces[wN] + pos->numPieces[bN]) < 3 ) {
                return true;
            }
        }
        else if( (pos->numPieces[wN] + pos->numPieces[wB] < 2) && (pos->numPieces[bN] + pos->numPieces[bB] < 2) ) {
            return true;
        }
    }
    return false;
}