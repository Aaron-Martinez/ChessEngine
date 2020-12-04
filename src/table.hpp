#ifndef TABLE_HPP
#define TABLE_HPP

#include "defs.h"

#include <vector>


struct Position;

enum { HFNONE, HFALPHA, HFBETA, HFEXACT };

struct PVEntry {
    U64 posKey;
    int move;
    
    int score;
    int depth;
    int flags;
};

struct PVTable {
    std::vector<PVEntry> pvTable;
    int numEntries;
    int newWrite;
    int overWrite;
    int hit;
    int cut;

    // void save(Position *pos, const int move);
    void save(Position *pos, const int move, int score, const int depth, const int flags);
    int probe(Position *pos);
    bool probe(Position *pos, int &move, int &score, int alpha, int beta, int depth);
    int getPV(const int depth, Position *pos);
};


void initPVTable(PVTable &table);
void clearPVTable(PVTable &table);

#endif