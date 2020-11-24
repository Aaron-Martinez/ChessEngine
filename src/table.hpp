#ifndef TABLE_HPP
#define TABLE_HPP

#include "defs.h"
#include "engine.hpp"
//#include "position.hpp"

#include <vector>


struct Position;

struct PVEntry {
    U64 posKey;
    int move;
};

struct PVTable {
    std::vector<PVEntry> pvTable;
    int numEntries;

    void save(Position *pos, const int move);
    int probe(Position *pos);
    int getPV(const int depth, Position *pos);
};


void initPVTable(PVTable &table);
void clearPVTable(PVTable &table);

#endif