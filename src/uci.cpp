#include "uci.hpp"

#include "position.hpp"
#include "search.hpp"
#include "utils.hpp"


void UCI::loop() {
    
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    string cmd, token;

    Position pos[1];
    SearchInfo info;
    initPVTable(pos->pvTable);

    do {
        getline(cin, cmd);
        istringstream iss(cmd);
        token.clear();

        iss >> std::skipws >> token;

        if(token == "quit" || token == "stop")  {
            info.quit = true;
        }
        else if (token == "ucinewgame") {
            clearPVTable(pos->pvTable);
            resetBoard(pos);
            parseFEN(START_FEN, pos);
            clearSearch(pos, info);
            initPVTable(pos->pvTable);
        }
        else if (token == "isready")     printf("readyok\n");
        else if (token == "position")    position(pos, iss);
        else if (token == "go")          go(pos, info, iss);
        else if (token == "uci") {
            printf("id name %s\n", NAME);
            printf("id author Aaron\n");
            printf("uciok\n");
        }

    } while(!info.quit);
}

void UCI::go(Position *pos, SearchInfo &info, std::istringstream &iss) {
    string token;
    info.timeSet = false;
    int winc = 0, binc = 0, wtime = 0, btime = 0, time = -1;
    int movestogo = 50, movetime = -1, depth = -1;
    //info.startTime = utils::currentTimeMillis();

    while(iss >> token) {
        if(token == "infinite")         info.timeSet = false;
        else if(token == "wtime")       iss >> wtime;
        else if(token == "btime")       iss >> btime;
        else if(token == "winc")        iss >> winc;
        else if(token == "binc")        iss >> binc;
        else if(token == "movestogo")   iss >> movestogo;
        else if(token == "movetime")    iss >> movetime;
        else if(token == "depth")       iss >> depth;
    }

    if(time == -1 && (wtime != 0 || btime != 0)) {
        if(pos->side == WHITE) {
            time = wtime;
        }
        else {
            time = btime;
        }
    }
    if(movetime != -1) {
        time = movetime;
        movestogo = 1;
    }
    // improve this later
    int inc = winc;

    info.startTime = utils::currentTimeMillis();
    info.depth = depth;

    if(pos->hisPly < 18) {
        movestogo = 30;
    } else if(pos->hisPly < 28) {
        movestogo = 18;
    } else if(pos->hisPly < 48) {
        movestogo = 8;
    } else if(pos->hisPly < 60) { 
        movestogo = 13;
    } else if(pos->hisPly < 80) {
        movestogo = 22;
    } else {
        movestogo = 30;
    }

    if(time != -1) {
        info.timeSet = true;
        time = (time / movestogo) - 50;
        info.stopTime = info.startTime + time + inc;
    }

    if(depth == -1) {
        info.depth = MAXDEPTH;
    }

    printf("time: %d start: %ld stop: %ld depth: %d timeset: %d\n", time, info.startTime, info.stopTime, info.depth, info.timeSet);
    search(pos, info);
}

void UCI::position(Position *pos, std::istringstream &iss) {
    // check for ucinewgame
    string token, fen;
    iss >> token;

    if(token == "startpos") {
        fen = START_FEN;
        iss >> token;
    }
    else if(token == "fen") {
        while(iss >> token && token != "moves") {
            fen += token + " ";
        }
    }

    parseFEN(fen.c_str(), pos);
    int move;

    printf("token is %s\n", token.c_str());
    // parse list of moves if any
    while(iss >> token) {
        if(token == "moves") { 
            continue;
        }
        move = utils::parseMove(token, pos);
        if(move == NOMOVE) {
            break;
        }
        makeMove(move, pos);
        pos->ply = 0;
    }

    printBoard(pos);

}