#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "defs.h"


struct Position;
struct MoveList;


struct SearchInfo {
    long startTime;
    long stopTime;
    bool timeSet;

    int depth;
    int depthSet;
    
    int movesToGo;
    bool infinite;
    bool quit;
    bool stopped;

    long nodes;

    float fh;
    float fhf;
};


static void pickNextMove(int moveNum, MoveList *list);
void search(Position *pos, SearchInfo &info);
static int alphaBeta(int alpha, int beta, int depth, Position *pos, SearchInfo &info, bool doNull);
static int quiescence(int alpha, int beta, Position *pos, SearchInfo &info);
void clearSearch(Position *pos, SearchInfo &info);
static bool isRepitition(const Position *pos);
static void checkStatus(SearchInfo &info);


#endif