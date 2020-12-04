#include "utils.hpp"

#include <cstdlib>
#include <iostream>
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"
#include <string>
#include <cstring>

namespace utils {

    string fileStrs[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    string rankStrs[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    string sq120Strings[BRD_NUM_SQ];
    // for move algebraic notation
    string pieceAlgNot[13] = {"", "", "N", "B", "R", "Q", "K", "", "N", "B", "R", "Q", "K"};
    char pieceAlgNotChar[13] = {' ', ' ', 'N', 'B', 'R', 'Q', 'K', ' ', 'N', 'B', 'R', 'Q', 'K'};
    
    long leafNodes = 0;  // for perft testing


    int inputWaiting() {
        fd_set readfds;
        struct timeval tv;
        FD_ZERO (&readfds);
        FD_SET (fileno(stdin), &readfds);
        tv.tv_sec=0; tv.tv_usec=0;
        select(16, &readfds, 0, 0, &tv);
        return (FD_ISSET(fileno(stdin), &readfds));
    }

    void readInput(SearchInfo &info) {
        int bytes;
        char input[256] = "", *endc;
        if(inputWaiting()) {
            info.stopped = true;
            do {
                bytes = read(fileno(stdin), input, 256);
            } while(bytes < 0);
            endc = strchr(input, '\n');
            if(endc)  *endc = 0;

            if(strlen(input) > 0) {
                if(!strncmp(input, "quit", 4)) {
                    info.quit = true;
                }
            }
            return;
        }
    }
    
    int parseMove(char *chars, Position *pos) {

        if(chars[1] > '8' || chars[1] < '1' || chars[3] > '8' || chars[3] < '1') {
            return 0;
        }
        if(chars[0] > 'h' || chars[0] < 'a' || chars[2] > 'h' || chars[2] < 'a') {
            return 0;
        }

        int originSQ = getIndex(chars[0] - 'a', chars[1] - '1');
        int targetSQ = getIndex(chars[2] - 'a', chars[3] - '1');

        MoveList list[1];
        generateAllMoves(pos, list);
        
        for(int moveNum = 0; moveNum < list->count; ++moveNum) {
            int move = list->moves[moveNum].move;
            if(getOriginSQ(move) == originSQ && getTargetSQ(move) == targetSQ) {
                int promoted = getPromoted(move);
                if(promoted == EMPTY) {
                    return move;
                }
                else if(promoted != EMPTY && tolower(pieceAlgNotChar[promoted]) == tolower(chars[4])) {
                    return move;
                }
            }
        }

        return NOMOVE;

    }

    int parseMove(string moveStr, Position *pos) {

        if(moveStr[1] > '8' || moveStr[1] < '1' || moveStr[3] > '8' || moveStr[3] < '1') {
            return 0;
        }
        if(moveStr[0] > 'h' || moveStr[0] < 'a' || moveStr[2] > 'h' || moveStr[2] < 'a') {
            return 0;
        }

        int originSQ = getIndex(moveStr[0] - 'a', moveStr[1] - '1');
        int targetSQ = getIndex(moveStr[2] - 'a', moveStr[3] - '1');

        MoveList list[1];
        generateAllMoves(pos, list);
        
        for(int moveNum = 0; moveNum < list->count; ++moveNum) {
            int move = list->moves[moveNum].move;
            if(getOriginSQ(move) == originSQ && getTargetSQ(move) == targetSQ) {
                int promoted = getPromoted(move);
                if(promoted == EMPTY) {
                    return move;
                }
                else if(promoted != EMPTY && tolower(pieceAlgNotChar[promoted]) == tolower(moveStr[4])) {
                    return move;
                }
            }
        }

        return NOMOVE;

    }

    void doMovesCmd(Position *pos) {

        int move = 0;
        char input[6];
        int depth = 4;
        int pvCount = 0;
        SearchInfo info;

        while(true) {
            printf("\n");
            printBoard(pos);
            printf("Enter a move > ");
            fgets(input, 6, stdin);

            if(input[0] == 'q') {
                break;
            }
            else if(input[0] == 'u') {
                undoMove(pos);
            } 
            else if(input[0] == 'r') {
                pvCount = pos->pvTable.getPV(depth, pos);
                printf("PV with %d moves is: ", pvCount);
                for(int pvIndex = 0; pvIndex < pvCount; ++pvIndex) {
                    move = pos->pvArr[pvIndex];
                    printf(" %s", makeMoveStr(move).c_str());
                }
                printf("\n");
            }
            else if(input[0] == 'p') {
                perftTest(depth, pos);
            }
            else if(input[0] == 's') {
                info.depth = 6;
                info.startTime = currentTimeMillis();
                info.stopTime = info.startTime + 200000;
                search(pos, info);
            } 
            else {
                move = parseMove(input, pos);
                if(move != 0) {
                    printf("Move is: %s\n", makeMoveStr(move).c_str());
                    // pos->pvTable.save(pos, move);
                    bool legal = makeMove(move, pos);
                    if(!legal) {
                        printf("illegal move made\n");
                    }
                }
                else {
                    printf("\nmove parsing unsuccessful\n");                    
                }
            }

            fflush(stdin);
        }
    }

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
        //if(capture) {
        //    moveStr += "x";
        //}
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
        exTime_t startTime = std::chrono::steady_clock::now();
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
            long nodes = leafNodes;
            perft(depth - 1, pos);
            undoMove(pos);
            //printf("undo move %s\n", makeMoveStr(move).c_str());
            long oldNodes = leafNodes - nodes;
            printf("move %d: %s : %ld\n", moveNum + 1, makeMoveStr(move).c_str(), oldNodes);
        }
        exTime_t endTime = std::chrono::steady_clock::now();
        printf("\nTest complete total nodes visited: %ld\n", leafNodes);
        printExTime(startTime, endTime, "Perft test took:");
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

    void readFenFromFile(string fileName, int depth) {
        //int depth = 5;
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


    void printExTime(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime, std::string desc) {
        long millis = exTimeMillis(startTime, endTime);
        int minutes = millis / 60000;
        millis -= minutes * 60000;
        int seconds = millis / 1000;
        millis -= seconds * 1000;
        if(minutes > 0) {
            printf("%s %d minutes, %d seconds, and %ld milliseconds\n", desc.c_str(), minutes, seconds, millis);
        }
        else if(seconds > 0) {
            printf("%s %d seconds, and %ld milliseconds\n", desc.c_str(), seconds, millis);
        }
        else {
            printf("%s %ld milliseconds\n", desc.c_str(), millis);
        }
    }

    void printExTime(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime) {
        printExTime(startTime, endTime, "Execution time:");
    }

    void printExTime(long startTime, long endTime) {
        printExTime(startTime, endTime, "Execution time:");
    }

    void printExTime(long startTime, long endTime, std::string desc) {
        long millis = endTime - startTime;
        int minutes = millis / 60000;
        millis -= minutes * 60000;
        int seconds = millis / 1000;
        millis -= seconds * 1000;
        if(minutes > 0) {
            printf("%s %d minutes, %d seconds, and %ld milliseconds\n", desc.c_str(), minutes, seconds, millis);
        }
        else if(seconds > 0) {
            printf("%s %d seconds, and %ld milliseconds\n", desc.c_str(), seconds, millis);
        }
        else {
            printf("%s %ld milliseconds\n", desc.c_str(), millis);
        }
    }

    long exTimeMillis(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime) {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()); 
    }

    long currentTimeMillis() {
        long millis = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
        return millis;
    }

};