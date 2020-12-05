#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "defs.h"


struct Position;
struct MoveList;


struct SearchInfo {
    long startTime;
    long stopTime;
    bool timeSet;
    int movesToGo;

    int depth;
    int depthSet;
    long nodes;

    // fail-high count to test effectiveness of our move ordering
    float fh;
    float fhf;
    
    bool infinite;
    bool quit;
    bool stopped;
};


static void pickNextMove(int moveNum, MoveList *list);
void search(Position *pos, SearchInfo &info);
static int alphaBeta(int alpha, int beta, int depth, Position *pos, SearchInfo &info, bool doNull);
static int quiescence(int alpha, int beta, Position *pos, SearchInfo &info);
void clearSearch(Position *pos, SearchInfo &info);
static bool isRepitition(const Position *pos);
static void checkStatus(SearchInfo &info);


#endif