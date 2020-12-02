#include "bitboard.hpp"
#include <iostream>
#include <cstdlib>


const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

U64 setMask[64];
U64 clearMask[64];

U64 fileBBMasks[8];
U64 rankBBMasks[8];
U64 passedPawnMasks[2][64];
U64 isoPawnMasks[64];


void initBitMasks() {
    for(int i = 0; i < 64; ++i) {
        setMask[i] = 0ULL;
        clearMask[i] = 0ULL;
    }

    for(int i = 0; i < 64; ++i) {
        setMask[i] |= (1ULL << i);
        clearMask[i] = ~setMask[i];
    }
}

void initBitboards() {
    
    for(int i = 0; i < 8; ++i) {
        fileBBMasks[i] = 0ULL;
        rankBBMasks[i] = 0ULL;
    }

    for(int r = RANK_1; r <= RANK_8; ++r) {
        for(int f = FILE_A; f <= FILE_H; ++f) {
            int sq = r * 8 + f;
            fileBBMasks[f] |= (1ULL << sq);
            rankBBMasks[r] |= (1ULL << sq);

            passedPawnMasks[WHITE][sq] = 0ULL;
            passedPawnMasks[BLACK][sq] = 0ULL;
            isoPawnMasks[sq] = 0ULL;
        }
    }

    for(int sq = 0; sq < 64; ++sq) {
        passedPawnMasks[WHITE][sq] = forwardRanksBB(WHITE, sq) & (adjacentFilesBB(sq) | fileBBMasks[sq % 8]);
        passedPawnMasks[BLACK][sq] = forwardRanksBB(BLACK, sq) & (adjacentFilesBB(sq) | fileBBMasks[sq % 8]);
        isoPawnMasks[sq] = adjacentFilesBB(sq);
    }

    // printf("White pawn d5 passed pawn mask\n");
    // printBitboard(passedPawnMasks[WHITE][index120to64[D5]]);
    // printf("Black pawn h4 passed pawn mask\n");
    // printBitboard(passedPawnMasks[BLACK][index120to64[H4]]);
    // printf("Isolated pawn b3 mask\n");
    // printBitboard(isoPawnMasks[index120to64[B3]]);

}

U64 forwardRanksBB(int color, int sq) {
    if(color == WHITE) {
        return ( ~rankBBMasks[0] << (8 * (sq / 8)) );
    }
    else {
        return ( ~rankBBMasks[7] >> (8 * (7 - (sq / 8))) );
    }
}

U64 adjacentFilesBB(int sq) {
    U64 thisFileBB = fileBBMasks[sq % 8];
    U64 westFileBB = (thisFileBB & ~fileBBMasks[0]) >> 1;
    U64 eastFileBB = (thisFileBB & ~fileBBMasks[7]) << 1;
    return westFileBB | eastFileBB;
}

bool isPawnIsolated(const U64 pawns[3], int sq, int color) {
    if( (isoPawnMasks[index120to64[sq]] & pawns[color]) == 0 ) {
        return true;
    }
    return false;
}

bool isPawnPassed(const U64 pawns[3], int sq, int color) {
    if( (passedPawnMasks[color][index120to64[sq]] & pawns[color]) == 0 ) {
        return true;
    }
    return false;
}

bool isOpenFile(const U64 pawns[3], int sq) {
    if( (pawns[BOTH] & fileBBMasks[filesArr[sq]]) == 0 ) {
        return true;
    }
    return false;
}

bool isSemiOpenFile(const U64 pawns[3], int sq, int color) {
    if( (pawns[color] & fileBBMasks[filesArr[sq]]) == 0 ) {
        return true;
    }
    return false;
}

void clearBit(U64& bboard, int index) {
    bboard &= clearMask[index];
}

void setBit(U64& bboard, int index) {
    bboard |= setMask[index];
}

int popBit(U64 *bboard) {
    U64 b = *bboard ^ (*bboard - 1);
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
    *bboard &= (*bboard - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}


int countBits(U64 bboard) {
    int r;
    for(r = 0; bboard; ++r) {
        bboard &= bboard - 1;
    }
    return r;
}


void printBitboard(U64 bboard) {
    
    U64 shifter = 1ULL;

    printf("\n");
    int i = 63;
    printf("   a b c d e f g h\n");
    for(int rank = 7; rank >= 0; --rank) {
        printf("%d  ", rank+1);
        for(int file = 0; file < 8; ++file) {
            int ind120 = 21 + file + (rank * 10);
            int ind64 = index120to64[ind120];
            if((shifter << ind64) & bboard) {
                printf("X ");
            } 
            else {
                printf("- ");
            }
        }
        printf("\n");
    }
    printf("\n\n");

}