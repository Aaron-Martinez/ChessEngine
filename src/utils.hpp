#ifndef UTILS_HPP
#define UTILS_HPP

#include "defs.h"
#include "position.hpp"
#include "move_gen.hpp"
#include "bitboard.hpp"

#include <string>
#include <iostream>
#include <fstream>

using std::string;


namespace utils {
    
    void initUtils();
    string sq120Str(int sq120);
    void printSq120(int sq120);
    string moveAlgNot(int move, int piece);
    string moveAlgNot(int move, Position *pos);
    void printMoveAlgNot(int move, int piece);
    void printMoveAlgNot(int move, Position *pos);
    void printMove(int move);
    string makeMoveStr(int move);
    void printMoveList(const MoveList *list);
    bool sqOnBoard(const int sq);
    bool validateSide(const int side);
    bool validateRankFile(const int rankOrFile);
    bool validatePiece(const int piece);
    bool validatePieceNonempty(const int piece);
    void perft(int depth, Position *pos);
    void perftTest(int depth, Position *pos);
    void multiplePerftTest(int depth, Position *pos, string fen, string fullLine);
    void readFenFromFile(string fileName);
};

#endif