#include "position.hpp"

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


void printBoard(const position *pos) {
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

U64 generatePosKey(const position *pos) {
    
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

void resetBoard(position *pos) {

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


int parseFEN(const char *fen, position *pos) {

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
    return 0;
}
