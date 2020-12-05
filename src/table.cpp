#include "table.hpp"

#include "move_gen.hpp"
#include "position.hpp"
#include "utils.hpp"


const int pvSizeTemp = 0x100000 * 16;



void initPVTable(PVTable &table) {

    table.numEntries = pvSizeTemp / sizeof(PVEntry) - 2;
    printf("Table initialized with numEntries = %d\n", table.numEntries);
    table.pvTable = std::vector<PVEntry>(table.numEntries);
    clearPVTable(table);
    
}

void clearPVTable(PVTable &table) {
    for(PVEntry entry : table.pvTable) {
        entry.posKey = 0ULL;
        entry.move = NOMOVE;
        entry.depth = 0;
        entry.score = 0;
        entry.flags = 0;
    }
}


int PVTable::probe(Position *pos) {

    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    if(pos->pvTable.pvTable[index].posKey == pos->posKey) {
        return pos->pvTable.pvTable[index].move;
    }

    return 0;

}


bool PVTable::probe(Position *pos, int &move, int &score, int alpha, int beta, int depth) {
    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    if(pos->pvTable.pvTable[index].posKey == pos->posKey) {
        PVEntry entry = pos->pvTable.pvTable[index];
        move = entry.move;
        if(entry.depth >= depth) {
            // hit++
            score = entry.score;
            if(score > MATE) {
                score -= pos->ply;
            } else if(score < -MATE) {
                score += pos->ply;
            }

            switch(entry.flags) {
                case HFALPHA:
                    if(score <= alpha) {
                        score = alpha;
                        return true;
                    }
                    break;
                case HFBETA:
                    if(score >= beta) {
                        score = beta;
                        return true;
                    }
                    break;
                case HFEXACT:
                    return true;
                default:
                    ASSERT(false);
                    break;
            }
        }
    }

    return false;
}

/*
void PVTable::save(Position *pos, const int move) {

    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    pos->pvTable.pvTable[index].move = move;
    pos->pvTable.pvTable[index].posKey = pos->posKey;
}
*/

void PVTable::save(Position *pos, const int move, int score, const int depth, const int flags) {

    ASSERT(pos->pvTable.numEntries > 0);
    int index = pos->posKey % pos->pvTable.numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable.numEntries - 1);

    if(pos->pvTable.pvTable[index].posKey == 0) {
        pos->pvTable.newWrite++;
    } else {
        pos->pvTable.overWrite++;
    }

    if(score > MATE) {
        score += pos->ply;
    } else if(score < -MATE) {
        score -= pos->ply;
    }

    pos->pvTable.pvTable[index].move = move;
    pos->pvTable.pvTable[index].posKey = pos->posKey;
    pos->pvTable.pvTable[index].score = score;
    pos->pvTable.pvTable[index].depth = depth;
    pos->pvTable.pvTable[index].flags = flags;
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
