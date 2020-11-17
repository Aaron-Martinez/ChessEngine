#include "utils.hpp"

#include <cstdlib>
#include <iostream>


namespace utils {

    string fileStrs[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    string rankStrs[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    string sq120Strings[BRD_NUM_SQ];
    // for move algebraic notation
    string pieceAlgNot[13] = {"", "", "N", "B", "R", "Q", "K", "", "N", "B", "R", "Q", "K"};
    
    long leafNodes = 0;  // for perft testing

    void initUtils() {

        int i = 0;
        for(int rank = 0; rank < 8; ++rank) {
            for(int file = 0; file < 8; ++file) {
                int index120 = index64to120[i];
                sq120Strings[index120] = fileStrs[file] + rankStrs[rank];
                i++;
            }
        }
    }

    string sq120Str(int sq120) {
        return sq120Strings[sq120];
    }

    void printSq120(int sq120) {
        std::cout << sq120Str(sq120) << std::endl;
    }

    // algebraic notation of move
    string moveAlgNot(int move, int piece) {
        /// todo
        // remember to deal with ambiguous moves (i.e. Nbd2, R2xf7)
    }

    void printMoveAlgNot(int move) {
        /// todo
    }

    // temp method ugly printmove
    void printMove(int move) {
        string moveStr = makeMoveStr(move);
        std::cout << moveStr << std::endl;
    }

    string makeMoveStr(int move) {

        string moveStr;

        int origin = getOriginSQ(move);
        int target = getTargetSQ(move);
        int capture = getCaptured(move);
        int promote = getPromoted(move);

        moveStr += sq120Str(origin);
        if(capture) {
            moveStr += "x";
        }
        moveStr += sq120Str(target);

        if(promote) {
            string p = "Q";
            if(isPieceN[promote]) {
                p = "N";
            }
            else if(isPieceRQ[promote] && !isPieceBQ[promote]) {
                p = "R";
            }
            else if(isPieceBQ[promote] && !isPieceRQ[promote]) {
                p = "B";
            }
            moveStr += "=" + p;
        }

        return moveStr;
    }


    void printMoveList(const MoveList *list) {

        std::cout << "\nMoveList:\n";
        for(int i = 0; i < list->count; ++i) {
            int move = list->moves[i].move;
            int score = list->moves[i].score;
            printf("Move #%d: %s (score:%d)\n", i+1, makeMoveStr(move).c_str(), score);
        }
    }


    // Validation methods
    bool sqOnBoard(const int sq) {
        return filesArr[sq] == OFFBOARD ? false : true;
    }

    bool validateSide(const int side) {
        return (side == WHITE || side == BLACK) ? true : false;
    }
    
    bool validateRankFile(const int rankOrFile) {
        return (rankOrFile >= 0 && rankOrFile <= 7) ? true : false;
    }
    
    bool validatePiece(const int piece) {
        return (piece >= EMPTY && piece <= bK) ? true : false;
    }
    
    bool validatePieceNonempty(const int piece) {
        return (piece >= wP && piece <= bK) ? true : false;
    }

    void perft(int depth, Position *pos) {

        if(depth == 0) {
            leafNodes++;
            return;
        }

        MoveList list[1];
        generateAllMoves(pos, list);
        int moveNum = 0;
        //printf("MOVES: %d\n", list->count);
        ///printBoard(pos);
        ///printBitboard(pos->pawns[0]);
        ///printBitboard(pos->pawns[1]);
        ///printBitboard(pos->pawns[2]);
    
        for(moveNum = 0; moveNum < list->count; ++moveNum) {
            int move = list->moves[moveNum].move;
            //printf("trying move %s\n", makeMoveStr(move).c_str());
            if(!makeMove(move, pos)) {
                continue;
            }
            //printf("made move %s\n", makeMoveStr(move).c_str());
            perft(depth - 1, pos);
            undoMove(pos);
           // printf("undo move %s\n", makeMoveStr(move).c_str());
        }

        return;
    }

    void perftTest(int depth, Position *pos) {
        printf("\nStarting testing to depth:%d\n", depth);
        leafNodes = 0;
        MoveList list[1];
        generateAllMoves(pos, list);
        printf("moves: %d\n", list->count);

        int move;
        int moveNum = 0;
        for(moveNum = 0; moveNum < list->count; ++moveNum) {
            move = list->moves[moveNum].move;
            if(!makeMove(move, pos)) {
                printf("move %d: %s  is illegal\n", moveNum+1, makeMoveStr(move).c_str());
                continue;
            }
            //printf("made move %s\n", makeMoveStr(move).c_str());
            ///printBoard(pos);
            ///printBitboard(pos->pawns[0]);
            ///printBitboard(pos->pawns[1]);
            ///printBitboard(pos->pawns[2]);
            long nodes = leafNodes;
            perft(depth - 1, pos);
            undoMove(pos);
            //printf("undo move %s\n", makeMoveStr(move).c_str());
            long oldNodes = leafNodes - nodes;
            printf("move %d: %s : %ld\n", moveNum + 1, makeMoveStr(move).c_str(), oldNodes);
        }

        printf("\nTest complete total nodes visited: %ld\n", leafNodes);
    }

    void multiplePerftTest(int depth, Position *pos, string fen, string fullLine) {

        leafNodes = 0;
        MoveList list[1];
        generateAllMoves(pos, list);

        int move;
        int moveNum = 0;
        for(moveNum = 0; moveNum < list->count; ++moveNum) {
            move = list->moves[moveNum].move;
            if(!makeMove(move, pos)) {
                continue;
            }
            long nodes = leafNodes;
            perft(depth - 1, pos);
            undoMove(pos);
        }

        printf("%s\nNODES:  %ld\n\n", fullLine.c_str(), leafNodes);
    }

    void readFenFromFile(string fileName) {
        int depth = 5;
        string fen = "";
        std::fstream file;
        file.open(fileName);
        string line;
        while(getline(file, line)) {
            int p = line.find(';');
            fen = line.substr(0, p);
            Position pos[1];
            parseFEN(fen.c_str(), pos);
            multiplePerftTest(depth, pos, fen, line);
        }

    }

};