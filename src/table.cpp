#include "table.hpp"
#include "position.hpp"


const int pvSizeTemp = 0x100000 * 2;


int PVTable::probe(Position *pos) {

    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    if(pos->pvTable.pvTable[index].posKey == pos->posKey) {
        return pos->pvTable.pvTable[index].move;
    }

    return 0;

}

void PVTable::save(Position *pos, const int move) {

    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    pos->pvTable.pvTable[index].move = move;
    pos->pvTable.pvTable[index].posKey = pos->posKey;
}

int PVTable::getPV(const int depth, Position *pos) {
    
    ASSERT(depth < MAXDEPTH);
    int move = probe(pos);
    int count = 0;
    while(move != 0 && count < depth) {
        if(moveExists(pos, move)) {
            makeMove(move, pos);
            pos->pvArr[count++] = move;
        }
        else {
            break;
        }
        move = probe(pos);
    }

    while(pos->ply > 0) {
        undoMove(pos);
    }

    return count;
}

void initPVTable(PVTable &table) {

    table.numEntries = pvSizeTemp / sizeof(PVEntry) - 2;
    printf("Table initialized with numEntries = %d\n", table.numEntries);
    table.pvTable = std::vector<PVEntry>(table.numEntries);
    clearPVTable(table);
    
}

void clearPVTable(PVTable &table) {
    for(PVEntry entry : table.pvTable) {
        entry.posKey = 0ULL;
        entry.move = 0;
    }
}