#include "position.hpp"
#include "bitboard.hpp"

#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    ((U64)rand() & 0xf) << 60 )

U64 pieceKeys[13][120];
U64 sideKey;
U64 casteKeys[16];


char pieceChars[] = ".PNBRQKpnbrqk";
char sideChars[] = "wb-";
char rankChars[] = "12345678";
char fileChars[] = "abcdefgh";


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

    return 0;
}


void initHashKeys() {
    for(int i = 0; i < 13; ++i) {
        for(int j = 0; j < 120; ++j) {
            pieceKeys[i][j] = RAND_64;
        }
    }
    sideKey = RAND_64;
    for(int i = 0; i < 16; ++i) {
        casteKeys[i] = RAND_64;
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
    finalKey ^= casteKeys[pos->castleRights];

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
