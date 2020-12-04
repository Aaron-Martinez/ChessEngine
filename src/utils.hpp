#ifndef UTILS_HPP
#define UTILS_HPP

#include "defs.h"
#include "position.hpp"
#include "move_gen.hpp"
#include "bitboard.hpp"
// #include "table.hpp"
#include "search.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>


//#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed ", #n); \
printf("On %s ", __DATE__); \
printf("At %s ", __TIME__); \
printf("In File %s ", __FILE__); \
printf("At Line %d\n", __LINE__); \
exit(1);}

#endif

using std::string;
typedef std::chrono::steady_clock::time_point exTime_t;

struct Position;
struct SearchInfo;
struct MoveList;

namespace utils {
    
    int parseMove(char *ptrChar, Position *pos);
    int parseMove(string moveStr, Position *pos);
    void doMovesCmd(Position *pos);
    int inputWaiting();
    void readInput(SearchInfo &info);

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
    void readFenFromFile(string fileName, int depth);

    void printExTime(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime, std::string desc);
    void printExTime(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime);
    void printExTime(long startTime, long endTime, std::string desc);
    void printExTime(long startTime, long endTime);
    long exTimeMillis(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime);
    long currentTimeMillis();
};

#endif