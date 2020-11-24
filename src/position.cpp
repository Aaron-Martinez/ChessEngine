#include "position.hpp"
#include "bitboard.hpp"
#include "utils.hpp"
#include "move_gen.hpp"

#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    ((U64)rand() & 0xf) << 60 )

U64 pieceKeys[13][120];
U64 sideKey;
U64 castleKeys[16];


char pieceChars[] = ".PNBRQKpnbrqk";
char sideChars[] = "wb-";
char rankChars[] = "12345678";
char fileChars[] = "abcdefgh";

// bitwise AND with these values will update castling rights
const int castleRightsUpdate[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};


// hash piece(piece, sq):  pos->posKey ^= pieceKeys[piece][sq]
// hash castle: pos->posKey ^= castleKeys[pos->castlePerm]
// hash side: pos->posKey ^= sideKey
// hash enPas: pos->posKey ^= pieceKeys[Empty][pos->enPas]


bool makeMove(int move, Position *pos) {

    ASSERT(checkBoard(pos));

    int originSQ = getOriginSQ(move);
    int targetSQ = getTargetSQ(move);
    int side = pos->side;

    pos->history[pos->hisPly].posKey = pos->posKey;

    if(move & moveFlagEP) {
        if(side == WHITE) {
            removePiece(targetSQ - 10, pos);
        }
        else {
            removePiece(targetSQ + 10, pos);
        }
    }
    if(move & moveFlagCastles) {
        switch (targetSQ) {
            case C1:
                movePiece(A1, D1, pos);
                break;
            case G1:
                movePiece(H1, F1, pos);
                break;
            case C8:
                movePiece(A8, D8, pos);
                break;
            case G8:
                movePiece(H8, F8, pos);
                break;
            default:
                // something went wrong
                ASSERT(false);
                break;
        }
    }

    if(pos->enPas != NO_SQ) {
        pos->posKey ^= pieceKeys[EMPTY][pos->enPas];
    }
    pos->posKey ^= castleKeys[pos->castleRights];

    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castleRights = pos->castleRights;

    pos->castleRights &= castleRightsUpdate[originSQ];
    pos->castleRights &= castleRightsUpdate[targetSQ];
    pos->enPas = NO_SQ;

    pos->posKey ^= castleKeys[pos->castleRights];

    int captured = getCaptured(move);
    pos->fiftyMove++;
    if(captured != EMPTY) {
        removePiece(targetSQ, pos);
        pos->fiftyMove = 0;
    }

    pos->hisPly++;
    pos->ply++;

    if(!isPieceBig[pos->allPieces[originSQ]]) {
        pos->fiftyMove = 0;
        if(move & moveFlagPawnDouble) {
            if(side == WHITE) {
                pos->enPas = originSQ + 10;
            }
            else {
                pos->enPas = originSQ - 10;
            }
            pos->posKey ^= pieceKeys[EMPTY][pos->enPas];
        }
    }

    movePiece(originSQ, targetSQ, pos);
    int promoted = getPromoted(move);
    if(promoted != EMPTY) {
        removePiece(targetSQ, pos);
        addPiece(targetSQ, pos, promoted);
    }

    if(isPieceK[pos->allPieces[targetSQ]]) {
        pos->kingSq[side] = targetSQ;
    }
    pos->side ^= 1;
    pos->posKey ^= sideKey;

    if(isSqAttacked(pos->kingSq[side], pos->side, pos)) {
        undoMove(pos);
        // update later - just dont generate illegal moves initially
        return false;
    }
    //ASSERT(checkBoard(pos));

    return true;
}

void undoMove(Position *pos) {
    
    //ASSERT(checkBoard(pos));

    pos->hisPly--;
    pos->ply--;

    int move = pos->history[pos->hisPly].move;
    int originSQ = getOriginSQ(move);
    int targetSQ = getTargetSQ(move);

    if(pos->enPas != NO_SQ) {
        pos->posKey ^= pieceKeys[EMPTY][pos->enPas];
    }
    pos->posKey ^= castleKeys[pos->castleRights];

    pos->castleRights = pos->history[pos->hisPly].castleRights;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPas = pos->history[pos->hisPly].enPas;

    if(pos->enPas != NO_SQ) {
        pos->posKey ^= pieceKeys[EMPTY][pos->enPas];
    }
    pos->posKey ^= castleKeys[pos->castleRights];

    pos->side ^= 1;
    pos->posKey ^= sideKey;

    if(move & moveFlagEP) {
        if(pos->side == WHITE) {
            addPiece(targetSQ - 10, pos, bP);
        }
        else {
            addPiece(targetSQ + 10, pos, wP);
        }
    }
    else if(move & moveFlagCastles) {
        switch (targetSQ) {
            case C1:
                movePiece(D1, A1, pos);
                break;
            case G1:
                movePiece(F1, H1, pos);
                break;
            case C8:
                movePiece(D8, A8, pos);
                break;
            case G8:
                movePiece(F8, H8, pos);
                break;
            default:
                // something went wrong
                ASSERT(false);
                break;
        }
    }

    movePiece(targetSQ, originSQ, pos);

    if(isPieceK[pos->allPieces[originSQ]]) {
        pos->kingSq[pos->side] = originSQ;
    }

    int captured = getCaptured(move);
    if(captured != EMPTY) {
        addPiece(targetSQ, pos, captured);
    }

    int promoted = getPromoted(move);
    if(promoted != EMPTY) {
        removePiece(originSQ, pos);
        int pawn = (pieceColor[promoted] == WHITE ? wP : bP);
        addPiece(originSQ, pos, pawn);
    }

    //ASSERT(checkBoard(pos));

}

static void movePiece(const int originSQ, const int targetSQ, Position *pos) {
    
    ASSERT(utils::sqOnBoard(originSQ));
    ASSERT(utils::sqOnBoard(targetSQ));

    int piece = pos->allPieces[originSQ];
    int color = pieceColor[piece];

    pos->posKey ^= pieceKeys[piece][originSQ];
    pos->allPieces[originSQ] = EMPTY;
    pos->posKey ^= pieceKeys[piece][targetSQ];
    pos->allPieces[targetSQ] = piece;

    // if it's a pawn update the bitboards
    if((!isPieceBig[piece]) && (piece != EMPTY)) {
        clearBit(pos->pawns[color], index120to64[originSQ]);
        clearBit(pos->pawns[BOTH], index120to64[originSQ]);
        setBit(pos->pawns[color], index120to64[targetSQ]);
        setBit(pos->pawns[BOTH], index120to64[targetSQ]);
    }

    for(int i = 0; i < pos->numPieces[piece]; ++i) {
        if(pos->pList[piece][i] == originSQ) {
            pos->pList[piece][i] = targetSQ;
            break;
        }
    }

}

static void addPiece(const int sq, Position *pos, const int piece) {

    ASSERT(utils::sqOnBoard(sq));
    ASSERT(utils::validatePiece(piece));

    int color = pieceColor[piece];
    pos->posKey ^= pieceKeys[piece][sq];
    pos->allPieces[sq] = piece;
    pos->material[color] += pieceValue[piece];

    if(isPieceBig[piece]) {
        pos->bigPieces[color]++;
        if(isPieceMaj[piece]) {
            pos->majPieces[color]++;
        }
        else {
            pos->minPieces[color]++;
        }
    }
    else {
        setBit(pos->pawns[color], index120to64[sq]);
        setBit(pos->pawns[BOTH], index120to64[sq]);
    }

    pos->pList[piece][pos->numPieces[piece]] = sq;
    pos->numPieces[piece]++;

}

static void removePiece(const int sq, Position *pos) {

    ASSERT(utils::sqOnBoard(sq));
    int piece = pos->allPieces[sq];
    ASSERT(utils::validatePiece(piece));

    int color = pieceColor[piece];
    pos->posKey ^= pieceKeys[piece][sq];

    pos->allPieces[sq] = EMPTY;
    pos->material[color] -= pieceValue[piece];

    if(isPieceBig[piece]) {
        pos->bigPieces[color]--;
        if(isPieceMaj[piece]) {
            pos->majPieces[color]--;
        }
        else {
            pos->minPieces[color]--;
        }
    }
    else {
        clearBit(pos->pawns[color], index120to64[sq]);
        clearBit(pos->pawns[BOTH], index120to64[sq]);
    }

    int pieceIndex = -1;
    for(int i = 0; i < pos->numPieces[piece]; ++i) {
        if(pos->pList[piece][i] == sq) {
            pieceIndex = i;
        }
    }
    ASSERT(pieceIndex != -1);

    // update piece list by moving last piece in list to the removed piece index
    pos->numPieces[piece]--;
    pos->pList[piece][pieceIndex] = pos->pList[piece][pos->numPieces[piece]];

}


void updateMaterialLists(Position *pos) {
    
    for(int sq = 0; sq < BRD_NUM_SQ; ++sq) {
        int piece = pos->allPieces[sq];
        if(piece != OFFBOARD && piece != EMPTY) {
            int color = pieceColor[piece];
            if(isPieceBig[piece])  pos->bigPieces[color]++;
            if(isPieceMin[piece])  pos->minPieces[color]++;
            if(isPieceMaj[piece])  pos->majPieces[color]++;

            pos->material[color] += pieceValue[piece];
            pos->pList[piece][pos->numPieces[piece]] = sq;
            pos->numPieces[piece]++;

            if(piece == wK)  pos->kingSq[WHITE] = sq;
            if(piece == bK)  pos->kingSq[BLACK] = sq;

            int sq64 = index120to64[sq];
            if(piece == wP) {
                setBit(pos->pawns[WHITE], sq64);
                setBit(pos->pawns[BOTH], sq64);
            }
            else if(piece == bP) {
                setBit(pos->pawns[BLACK], sq64);
                setBit(pos->pawns[BOTH], sq64);
            }
        }
    }

    

}

void printBoard(const Position *pos) {
    for(int rank = RANK_8; rank >= RANK_1; --rank) {
        printf("%d  ", rank+1);
        for(int file = FILE_A; file <= FILE_H; ++file) {
            int sq = getIndex(file, rank);
            int piece = pos->allPieces[sq];
            printf("%3c", pieceChars[piece]);
        }
        printf("\n");
    }
    printf("\n   ");
    for(int file = FILE_A; file <= FILE_H; ++file) {
        printf("%3c", fileChars[file]);
    }
    printf("\n\n");
    printf("side: %c\n", sideChars[pos->side]);
    printf("enPas: %d\n", pos->enPas);
    printf("CastleRights: %c%c%c%c\n",
            pos->castleRights & W_OO ? 'K' : '-',
            pos->castleRights & W_OOO ? 'Q' : '-',
            pos->castleRights & B_OO ? 'k' : '-',
            pos->castleRights & B_OOO ? 'q' : '-'
    );
    printf("Position key: %llX\n\n", pos->posKey);
}

// This method will be used for debugging to check if the position struct is updating correctly during games
int checkBoard(const Position *pos) {

    int pceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int bigPce[2] = {0, 0};
    int majPce[2] = {0, 0};
    int minPce[2] = {0, 0};
    int materialVal[2] = {0, 0};
    
    for(int piece = wP; piece <= bK; ++piece) {
        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(pos->allPieces[sq] == piece);
        }
    }

    for(int sq = 0; sq < 64; ++sq) {
        int sq120 = index64to120[sq];
        int piece = pos->allPieces[sq120];
        pceNum[piece]++;
        int color = pieceColor[piece];
        
        if(isPieceBig[piece])  bigPce[color]++;
        if(isPieceMin[piece])  minPce[color]++;
        if(isPieceMaj[piece])  majPce[color]++;

        materialVal[color] += pieceValue[piece];
    }

    for(int piece = wP; piece <= bK; ++piece) {
        ASSERT(pceNum[piece] == pos->numPieces[piece]);
    }

    // verify pawn bitboard counts
    int numPawns;
    U64 pawns_bb[3] = {0ULL, 0ULL, 0ULL};
    pawns_bb[WHITE] = pos->pawns[WHITE];
    pawns_bb[BLACK] = pos->pawns[BLACK];
    pawns_bb[BOTH] = pos->pawns[BOTH];

    numPawns = countBits(pawns_bb[WHITE]);
    ASSERT(numPawns == pos->numPieces[wP]);
    numPawns = countBits(pawns_bb[BLACK]);
    ASSERT(numPawns == pos->numPieces[bP]);
    numPawns = countBits(pawns_bb[BOTH]);
    ASSERT(numPawns == (pos->numPieces[wP] + pos->numPieces[bP]));

    // make sure bitboard pawn squares match board pawn squares
    while(pawns_bb[WHITE]) {
        int sq = popBit(&pawns_bb[WHITE]);
        ASSERT(pos->allPieces[index64to120[sq]] == wP);
    }
    while(pawns_bb[BLACK]) {
        int sq = popBit(&pawns_bb[BLACK]);
        ASSERT(pos->allPieces[index64to120[sq]] == bP);
    }
    while(pawns_bb[BOTH]) {
        int sq = popBit(&pawns_bb[BOTH]);
        ASSERT( (pos->allPieces[index64to120[sq]] == wP) || (pos->allPieces[index64to120[sq]] == bP) );
    }

    // verify piece and material counts
    ASSERT( (materialVal[WHITE] == pos->material[WHITE]) || (materialVal[BLACK] == pos->material[BLACK]) );
    ASSERT( (minPce[WHITE] == pos->minPieces[WHITE]) || (minPce[BLACK] == pos->minPieces[BLACK]) );
    ASSERT( (majPce[WHITE] == pos->majPieces[WHITE]) || (majPce[BLACK] == pos->majPieces[BLACK]) );
    ASSERT( (bigPce[WHITE] == pos->bigPieces[WHITE]) || (bigPce[BLACK] == pos->bigPieces[BLACK]) );

    ASSERT(pos->side == WHITE || pos->side == BLACK);
    ASSERT(generatePosKey(pos) == pos->posKey);

    // en passant square can only be on 3rd or 6th rank
    if(pos->side == WHITE) {
        ASSERT( (pos->enPas == NO_SQ) || (ranksArr[pos->enPas] == RANK_6) );
    }
    else if(pos->side == BLACK) {
        ASSERT( (pos->enPas == NO_SQ) || (ranksArr[pos->enPas] == RANK_3) );
    }

    ASSERT(pos->allPieces[pos->kingSq[WHITE]] == wK);
    ASSERT(pos->allPieces[pos->kingSq[BLACK]] == bK);

    return 1;
}


void initHashKeys() {
    for(int i = 0; i < 13; ++i) {
        for(int j = 0; j < 120; ++j) {
            pieceKeys[i][j] = RAND_64;
        }
    }
    sideKey = RAND_64;
    for(int i = 0; i < 16; ++i) {
        castleKeys[i] = RAND_64;
    }
}

U64 generatePosKey(const Position *pos) {
    
    U64 finalKey = 0;
    int piece = EMPTY;

    for(int sq = 0; sq < BRD_NUM_SQ; ++sq) {
        piece = pos->allPieces[sq];
        if(piece != NO_SQ && piece != EMPTY && piece != -1 && piece != OFFBOARD) {
            ASSERT(piece >= wP && piece <= bK);
            finalKey ^= pieceKeys[piece][sq];
        }
    }

    if(pos->side == WHITE) {
        finalKey ^= sideKey;
    }

    if(pos->enPas != NO_SQ) {
        ASSERT(pos->enPas >= 0 && pos->enPas < BRD_NUM_SQ);
        finalKey ^= pieceKeys[EMPTY][pos->enPas];
    }

    ASSERT(pos->castleRights >= 0 && pos->castleRights <= 15);
    finalKey ^= castleKeys[pos->castleRights];

    return finalKey;
}

void resetBoard(Position *pos) {

    for(int i = 0; i < BRD_NUM_SQ; ++i) {
        pos->allPieces[i] = OFFBOARD;
    }

    for(int i = 0; i < 64; ++i) {
        pos->allPieces[index64to120[i]] = EMPTY;
    }

    for(int i = 0; i < 2; ++i) {
        pos->bigPieces[i] = 0;
        pos->majPieces[i] = 0;
        pos->minPieces[i] = 0;
        pos->material[i] = 0;
        pos->pawns[i] = 0ULL;
    }
    pos->pawns[2] = 0ULL;

    for(int i = 0; i < 13; ++i) {
        pos->numPieces[i] = 0;
    }

    pos->kingSq[WHITE] = NO_SQ;
    pos->kingSq[BLACK] = NO_SQ;

    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;
    pos->ply = 0;
    pos->hisPly = 0;
    pos->castleRights = 0;
    pos->posKey = 0ULL;
}


int parseFEN(const char *fen, Position *pos) {

    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    resetBoard(pos);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int index64 = 0;
    int index120 = 0;

    while((rank >= RANK_1 ) && *fen) {
        count = 1;
        switch(*fen) {
            case 'p':
                piece = bP;
                break;
            case 'n':
                piece = bN;
                break;
            case 'b':
                piece = bB;
                break;
            case 'r':
                piece = bR;
                break;
            case 'q':
                piece = bQ;
                break;
            case 'k':
                piece = bK;
                break;
            case 'P':
                piece = wP;
                break;
            case 'N':
                piece = wN;
                break;
            case 'B':
                piece = wB;
                break;
            case 'R':
                piece = wR;
                break;
            case 'Q':
                piece = wQ;
                break;
            case 'K':
                piece = wK;
                break;
            
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;
            
            default:
                printf("Erroneous character in FEN: %c", *fen);
                return -1;
        }

        for(int i = 0; i < count; ++i) {
            index64 = rank * 8 + file;
            index120 = index64to120[index64];
            if(piece != EMPTY) {
                pos->allPieces[index120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');
    if(*fen == 'w') {
        pos->side = WHITE;
    }
    else {
        pos->side = BLACK;
    }

    fen += 2;
    for(int i = 0; i < 4; ++i) {
        if(*fen == ' ') {
            break;
        }
        switch(*fen) {
            case 'K':
                pos->castleRights |= W_OO;
                break;
            case 'Q':
                pos->castleRights |= W_OOO;
                break;
            case 'k':
                pos->castleRights |= B_OO;
                break;
            case 'q':
                pos->castleRights |= B_OOO;
                break;
            default:
                break;
        }
        fen++;
    }
    ASSERT(pos->castleRights >= 0 && pos->castleRights <= 15);
    
    fen++;
    if(*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        pos->enPas = index64to120[rank*8 + file];
        fen++;
    }

    fen += 2;
    pos->fiftyMove = *fen - '0';
    fen++;
    int fullMove = *fen - '0';
    pos->ply = 2 * (fullMove - 1) + pos->side;

    pos->posKey = generatePosKey(pos);
    updateMaterialLists(pos);
    return 0;
}
