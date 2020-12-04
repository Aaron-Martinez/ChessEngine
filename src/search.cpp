#include "search.hpp"

#include "evaluate.hpp"
#include "move_gen.hpp"
#include "position.hpp"
#include "utils.hpp"


// find best scoring move and swap it to the front of the list
static void pickNextMove(int moveNum, MoveList *list) {
    Move tempMove;
    int index = 0;
    int bestScore = 0;
    int bestIndex = moveNum;

    for(index = moveNum; index < list->count; ++index) {
        if(list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestIndex = index;
        }
    }

    tempMove = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestIndex];
    list->moves[bestIndex] = tempMove;
}

void search(Position *pos, SearchInfo &info) {
    
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvIndex = 0;
    
    //int alpha = -INFINITE;
    //int beta = INFINITE;

    clearSearch(pos, info);

    // iterative deepening
    for(currentDepth = 1; currentDepth <= info.depth; ++currentDepth) {
        
        bestScore = alphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, true);

        if(info.stopped) {
            break;
        }

        pvMoves = pos->pvTable.getPV(currentDepth, pos);
        bestMove = pos->pvArr[0];

        //printf("Depth: %d, pvMoves: %d, score: %d, move: %s, nodes: %ld  ", currentDepth, pvMoves, bestScore, utils::makeMoveStr(bestMove).c_str(), info.nodes);
        printf("info score cp %d depth %d nodes %ld time %ld ", bestScore, currentDepth, info.nodes, utils::currentTimeMillis() - info.startTime);
        printf("and PV is:");
        for(pvIndex = 0; pvIndex < pvMoves; ++pvIndex) {
            printf(" %s", utils::makeMoveStr(pos->pvArr[pvIndex]).c_str());
        }
        if(bestScore > 28000) {
            int mateIn = MATE - bestScore;
            printf(" (mate in %d!)", mateIn);
        }
        printf("\n");
        if(info.fh != 0) {
            //printf("Ordering: %.2f\n", (info.fhf/info.fh));
        }
    }
    printf("bestmove %s\n", utils::makeMoveStr(bestMove).c_str());
}

static int alphaBeta(int alpha, int beta, int depth, Position *pos, SearchInfo &info, bool doNull) {
    
    ASSERT(checkBoard(pos));
    if(depth <= 0) {
        //info.nodes++;
        return quiescence(alpha, beta, pos, info);
        //return evaluate(pos);
    }

    if(info.nodes % 2048 == 0) {
        checkStatus(info);
    }
    info.nodes++;
    if((isRepitition(pos) || pos->fiftyMove >= 100) && pos->ply) {
        return 0;
    }

    if(pos->ply >= MAXDEPTH) {
        return evaluate(pos);
    }

    bool inCheck = isSqAttacked(pos->kingSq[pos->side], pos->side^1, pos);
    if(inCheck) {
        depth++;
    }

    int score = -INFINITE;
    int pvMove = NOMOVE;

    if(pos->pvTable.probe(pos, pvMove, score, alpha, beta, depth)) {
        pos->pvTable.cut++;
        return score;
    }
    // null move pruning
    if(doNull && !inCheck && pos->ply && (pos->bigPieces[pos->side] > 2) && depth >= 4) {
        makeNullMove(pos);
        score = -alphaBeta(-beta, -beta + 1, depth - 4, pos, info, false);
        undoNullMove(pos);
        if(info.stopped) {
            return 0;
        }
        if(score >= beta && abs(score) < MATE) {
            return beta;
        }
    }

    MoveList list[1];
    generateAllMoves(pos, list);

    int moveNum = 0;
    int legalMoves = 0;
    int prevAlpha = alpha;
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    score = -INFINITE;
    
    //int pvMove = pos->pvTable.probe(pos);
    if(pvMove != NOMOVE) {
        for(moveNum = 0; moveNum < list->count; ++moveNum) {
            if(list->moves[moveNum].move == pvMove) {
                list->moves[moveNum].score = 2000000;
                break;
            }
        }
    }

    for(moveNum = 0; moveNum < list->count; ++moveNum) {
        pickNextMove(moveNum, list);
        if(makeMove(list->moves[moveNum].move, pos)) {
            legalMoves++;
            score = -alphaBeta(-beta, -alpha, depth-1, pos, info, true);
            undoMove(pos);

            if(info.stopped) {
                return 0;
            }

            if(score > bestScore) {
                bestScore = score;
                bestMove = list->moves[moveNum].move;

                if(score >= beta) {
                    if(legalMoves == 1) {
                        info.fhf++;
                    }
                    info.fh++;                
                    if(!(list->moves[moveNum].move & moveFlagIsCapture)) {
                        pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                        pos->searchKillers[0][pos->ply] = list->moves[moveNum].move;
                    }
                    // store hash entry
                    pos->pvTable.save(pos, bestMove, beta, depth, HFBETA);

                    return beta;
                }
                if(score > alpha) {
                    alpha = score;
                    //bestMove = list->moves[moveNum].move;
                    if(!(list->moves[moveNum].move & moveFlagIsCapture)) {
                        pos->searchHistory[pos->allPieces[getOriginSQ(bestMove)]][getTargetSQ(bestMove)] += depth;
                    }
                }
            } // end score > bestScore
        }
    }

    // check for stalemate/checkmate
    if(legalMoves == 0) {
        if(inCheck) {
            return -MATE + pos->ply;
        }
        else {
            return 0;
        }
    }

    if(alpha != prevAlpha) {
        // store hash entry
        pos->pvTable.save(pos, bestMove, bestScore, depth, HFEXACT);
        //pos->pvTable.save(pos, bestMove);
    } 
    else {
        // store hash entry
        pos->pvTable.save(pos, bestMove, alpha, depth, HFALPHA);
    }

    return alpha;
}

static int quiescence(int alpha, int beta, Position *pos, SearchInfo &info) {
    
    ASSERT(checkBoard(pos));
    if(info.nodes % 2048 == 0) {
        checkStatus(info);
    }
    info.nodes++;

    if(isRepitition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if(pos->ply >= MAXDEPTH) {
        return evaluate(pos);
    }

    int score = evaluate(pos);
    if(score >= beta) {
        return beta;
    }
    if(score > alpha) {
        alpha = score;
    }

    MoveList capsList[1];
    generateCaptureMoves(pos, capsList);

    int moveNum = 0;
    int legalMoves = 0;
    int prevAlpha = alpha;
    int bestMove = NOMOVE;
    score = -INFINITE;

    for(moveNum = 0; moveNum < capsList->count; ++moveNum) {
        pickNextMove(moveNum, capsList);
        if(makeMove(capsList->moves[moveNum].move, pos)) {
            legalMoves++;
            score = -quiescence(-beta, -alpha, pos, info);
            undoMove(pos);

            if(info.stopped) {
                return 0;
            }

            if(score >= beta) {
                if(legalMoves == 1) {
                    info.fhf++;
                }
                info.fh++;                
                return beta;
            }
            if(score > alpha) {
                alpha = score;
                bestMove = capsList->moves[moveNum].move;
            }
        }
    }

/*
    if(alpha != prevAlpha) {
        pos->pvTable.save(pos, bestMove);
    }
*/

    return alpha;
}

// get ready for fresh new search
void clearSearch(Position *pos, SearchInfo &info) {
    
    int i = 0;
    int j = 0;
    
    for(i = 0; i < 13; ++i) {
        for(j = 0; j < BRD_NUM_SQ; ++j) {
            pos->searchHistory[i][j] = 0;
        }
    }

    for(i = 0; i < 2; ++i) {
        for(j = 0; j < MAXDEPTH; ++j) {
            pos->searchKillers[i][j] = 0;
        }
    }

    // dont clear now that this is transposition table
    //clearPVTable(pos->pvTable);

    pos->ply = 0;

    //info.startTime = std::chrono::steady_clock::now();
    info.stopped = 0;
    info.nodes = 0L;
    info.fh = 0;
    info.fhf = 0;
}

static bool isRepitition(const Position *pos) {

    for(int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly - 1; ++i) {
        if(pos->posKey == pos->history[i].posKey) {
            return true;
        }
    }
    return false;

}

// check for time up or interrupt from GUI
static void checkStatus(SearchInfo &info) {
    if((info.timeSet) && (utils::currentTimeMillis() > info.stopTime)) {
        info.stopped = true;
    }
    utils::readInput(info);
}

