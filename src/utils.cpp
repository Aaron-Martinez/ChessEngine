#include "utils.hpp"

#include <cstdlib>
#include <iostream>


namespace utils {

    string fileStrs[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    string rankStrs[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    string sq120Strings[BRD_NUM_SQ];
    // for move algebraic notation
    string pieceAlgNot[13] = {"", "", "N", "B", "R", "Q", "K", "", "N", "B", "R", "Q", "K"};

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

        int origin = originSQ(move);
        int target = targetSQ(move);
        int capture = captured(move);
        int promote = promoted(move);

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

};