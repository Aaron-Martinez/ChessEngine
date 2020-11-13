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