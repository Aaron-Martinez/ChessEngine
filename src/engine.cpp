#include "engine.hpp"

#include "bitboard.hpp"
#include "move_gen.hpp"
#include "position.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "tactic.hpp"

#include "stdio.h"
#include <vector>


// the following arrays are used to convert indices between 8x8 and 12x10 board
int index120to64[BRD_NUM_SQ];
int index64to120[64];

int filesArr[BRD_NUM_SQ];
int ranksArr[BRD_NUM_SQ];

int isPieceN[13] = {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0};
int isPieceK[13] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};
int isPieceRQ[13] = {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0};
int isPieceBQ[13] = {0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0};

int isPieceBig[13] = {0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1};
int isPieceMaj[13] = {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};
int isPieceMin[13] = {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0};
int pieceValue[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int pieceColor[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};


void initAll() {
    initBoardIndexConversions();
    initBitMasks();
    initBitboards();
    initHashKeys();
    utils::initUtils();
    initMvvLva();
    printf("finished initialization\n\n");
}

void initBoardIndexConversions() {

    for(int i = 0; i < BRD_NUM_SQ; ++i) { 
        index120to64[i] = -1;
        filesArr[i] = OFFBOARD;
        ranksArr[i] = OFFBOARD;
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
            filesArr[sq120] = file;
            ranksArr[sq120] = rank;
            i++;
        }
    }
}

void run() {
    
    initAll();
    //utils::readFenFromFile("perftsuite.epd", 4);

    Position pos[1];
    initPVTable(pos->pvTable);

    //std::string a = WAC1;
    //std::string a = TRICKY_FEN;
    //std::string a = START_FEN;

    std::string a = PINS_FEN1;


    parseFEN(a.c_str(), pos);
    std::cout << "\n\nFEN used: " << a << "\n\n";
    printBoard(pos);
    
    //utils::doMovesCmd(pos);

    //MoveList list[1];
    //generateAllMoves(pos, list);
    //utils::printMoveList(list);

    //utils::perftTest(5, pos);

    //UCI::loop();

    std::vector<Pin> pins = getAllPins(pos);

}

int getIndex(int file, int rank) {
    int index = 21 + file + (rank * 10);
    ASSERT(index <= 120 && index >= 0);
    return index;
}