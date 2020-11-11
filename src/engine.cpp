#include "stdio.h"
//#include "bitboard.hpp"
#include "position.hpp"
#include "engine.hpp"


// the following arrays are used to convert indices between 8x8 and 12x10 board
int index120to64[BRD_NUM_SQ];
int index64to120[64];


void initAll() {
    initBoardIndexConversions();
    //initBitMasks();
    initHashKeys();
    printf("finished initialization\n\n");
}

void initBoardIndexConversions() {

    for(int i = 0; i < BRD_NUM_SQ; ++i) {
        index120to64[i] = -1;
    }
    for(int i = 0; i < 64; ++i) {
        index64to120[i] = -1;
    }

    int i = 0;
    for(int rank = 0; rank < 8; ++rank) {
        for(int file = 0; file < 8; ++file) {
            int sq120 = 21 + file + (rank * 10);
            index64to120[i] = sq120;
            index120to64[sq120] = i;
            i++;
        }
    }
}

void run() {
    
    position pos[1];
    std::string a = START_FEN;
    parseFEN(a.c_str(), pos);
    printBoard(pos);

}

int getIndex(int file, int rank) {
    int index = 21 + file + (rank * 10);
    ASSERT(index <= 120 && index >= 0);
    return index;
}