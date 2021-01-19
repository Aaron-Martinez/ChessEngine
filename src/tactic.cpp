#include "tactic.hpp"

#include "position.hpp"
#include "utils.hpp"
#include "move_gen.hpp"



int slidingPiecesIndex2[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
const int bigPieceDirs2[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, // white knight
    {-9, -11, 11, 9, 0, 0, 0, 0},       // white bishop
    {-1, -10, 1, 10, 0, 0, 0, 0},       // white rook
    {-1, -10, 1, 10, -9, -11, 11, 9},   // white queen
    {-1, -10, 1, 10, -9, -11, 11, 9},   // white king
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, // black knight
    {-9, -11, 11, 9, 0, 0, 0, 0},       // black bishop
    {-1, -10, 1, 10, 0, 0, 0, 0},       // black rook
    {-1, -10, 1, 10, -9, -11, 11, 9},   // black queen
    {-1, -10, 1, 10, -9, -11, 11, 9}    // black king
};
const int bigPieceNumDirs2[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

string pieceNames[13] = {"square", "pawn", "knight", "bishop", "rook", "queen", "king", 
                                    "pawn", "knight", "bishop", "rook", "queen", "king"};


vector<Pin> getAllPins(Position *pos) {

    vector<Pin> pins;
    int side = pos->side;
    // string p = "Piece on a2 is " + pieceNames[pos->allPieces[A2]] + "\n";
    // printf(p.c_str());
   
    // iterate through the opponents sliding pieces
    for(int i = 0; i < 3; ++i) {
        
        int piece = slidingPiecesIndex2[side^1][i];
        
        for(int pieceNum = 0; pieceNum < pos->numPieces[piece]; ++pieceNum) {
            int sq = pos->pList[piece][pieceNum];
            ASSERT(utils::sqOnBoard(sq));
            int numDirs = bigPieceNumDirs2[piece];

            int maybePinningPiece = sq;

            for(int j = 0; j < numDirs; ++j) {
                int dir = bigPieceDirs2[piece][j];
                int targetSQ = sq + dir;
                bool pinFound = false;
                bool noPins = false;
                // std::cout << "maybe pinning piece = " << std::to_string(maybePinningPiece) << ", dir = " << std::to_string(dir) << std::endl;

                while(utils::sqOnBoard(targetSQ)) {
                    if(pinFound || noPins) {
                        break;
                    }
                    if(pos->allPieces[targetSQ] != EMPTY) {
                        if(pieceColor[pos->allPieces[targetSQ]] == side) {
                            // my piece on this square - check if its pinned
                            int maybePinnedPiece = targetSQ;
                            targetSQ += dir;
                            while(utils::sqOnBoard(targetSQ)) {
                                if(pinFound || noPins) {
                                    // pinFound = false;
                                    // noPins = false;
                                    break;
                                }
                                // if your king is on this square, it's an absolute pin
                                if(targetSQ == pos->kingSq[side]) {
                                     // pinned to the king
                                    Pin pin;
                                    pin.pinnedPiece = maybePinnedPiece;
                                    pin.pinningPiece = maybePinningPiece;
                                    pin.pinnedToSq = targetSQ;
                                    pin.isPinAbsolute = true;
                                    pins.push_back(pin);
                                    printPin(pin, pos);
                                    pinFound = true;
                                    break;
                                }

                                
                                if(pos->allPieces[targetSQ] != EMPTY && pieceColor[pos->allPieces[targetSQ]] != side) {
                                    // another of opponents pieces - this direction won't have a pin
                                    // std::cout << "here" << std::endl;
                                    noPins = true;
                                    break;
                                }

                                // check if opponent moving pinningPiece here would be checkmate
                                // after us moving pinned piece to each of its legal squares
                                MoveList list[1];
                                generateAllMoves(pos, list);
                                
                                for(int moveNum = 0; moveNum < list->count; ++moveNum) {
                                    bool pinDetected = false;
                                // for(Move m : list->moves) {
                                    int move = list->moves[moveNum].move;
                                    if(getOriginSQ(move) == maybePinnedPiece) {
                                        makeMove(move, pos);
                                        int checkmateAttempt;
                                        if(pos->allPieces[targetSQ] == EMPTY || pieceColor[pos->allPieces[targetSQ]] == side) {
                                            // std::cout << "test1\n";
                                            checkmateAttempt = createMove(maybePinningPiece, targetSQ, pos->allPieces[targetSQ], 0, 0);
                                            if(makeMove(checkmateAttempt, pos)) {

                                                // if(maybePinningPiece == G2) {
                                                    // string checkmateAttStr = "Is this checkmate - just played: " + utils::makeMoveStr(checkmateAttempt) + "\n";
                                                    // std::cout << checkmateAttStr;
                                                    // printBoard(pos);
                                                // }

                                                if(isCheckmate(pos)) {
                                                    undoMove(pos);
                                                    undoMove(pos);
                                                    Pin pin;
                                                    pin.pinnedPiece = maybePinnedPiece;
                                                    pin.pinningPiece = maybePinningPiece;
                                                    pin.pinnedToSq = targetSQ;
                                                    pin.isPinAbsolute = false;
                                                    pinDetected = true;
                                                    pins.push_back(pin);
                                                    // printPin(pin, pos);
                                                    printCheckMatePin(pin, pos, move, checkmateAttempt);
                                                    
                                                    pinFound = true;
                                                    // break;
                                                } 
                                                else {
                                                    undoMove(pos);
                                                }
                                            }
                                            
                                        }
                                        if(!pinDetected) {
                                            undoMove(pos);
                                        } 
                                    }
                                }
                                if(pinFound || noPins) {
                                    // std::cout << "pin found in this dir " << std::endl;
                                    // pinFound = false;
                                    // noPins = false;
                                    break;
                                }

                                // std::cout << "now check high value or undefended pieces" << std::endl;

                                // now check for high value or undefended pieces
                                if(pos->allPieces[targetSQ] != EMPTY) {
                                    if(pieceColor[pos->allPieces[targetSQ]] == side) {

                                        if(pieceValue[pos->allPieces[targetSQ]] > pieceValue[pos->allPieces[maybePinningPiece]]) {
                                            // pinned to piece more valuable than pinning piece
                                            Pin pin;
                                            pin.pinnedPiece = maybePinnedPiece;
                                            pin.pinningPiece = maybePinningPiece;
                                            pin.pinnedToSq = targetSQ;
                                            pin.isPinAbsolute = false;
                                            pins.push_back(pin);
                                            printPin(pin, pos);
                                            pinFound = true;
                                            // break;
                                        }

                                        else if(isPieceBig[pos->allPieces[targetSQ]]) {
                                            if(!isSqAttacked(targetSQ, side, pos)) {
                                                // an undefended piece of ours
                                                Pin pin;
                                                pin.pinnedPiece = maybePinnedPiece;
                                                pin.pinningPiece = maybePinningPiece;
                                                pin.pinnedToSq = targetSQ;
                                                pin.isPinAbsolute = false;
                                                pins.push_back(pin);
                                                printPin(pin, pos);
                                                pinFound = true;
                                                // break;
                                            }
                                        }
                                        
                                    }
                                    noPins = true;
                                    break;
                                }
                                targetSQ += dir;
                            }

                            //break;
                        }
                        else {
                            break;
                        }
                    }

                    targetSQ += dir;
                }
            } // iterating through directions
        }
    } // iterate through sliding pieces

    return pins;
}


bool isPin(Position *pos, int move) {
    vector<Pin> allPins = getAllPins(pos);
    vector<Pin> allPins2;
    if(makeMove(move, pos)) {
        allPins2 = getAllPins(pos);
    }
    // look for the new pins
    return false;
}


void printPin(Pin pin, Position *pos) {

    // printBoard(pos);

    int side = pos->side;

    int pinnedPieceSq = pin.pinnedPiece;
    int pinningPieceSq = pin.pinningPiece;
    int pinnedToSq = pin.pinnedToSq;
    bool isPinAbsolute = pin.isPinAbsolute;

    int pinnedPieceType = pos->allPieces[pinnedPieceSq];
    int pinningPieceType = pos->allPieces[pinningPieceSq];
    int pinnedToPieceType = pos->allPieces[pinnedToSq];

    // printf("pinned piece type %d\n", pinnedPieceType);

    string pinnedPieceSqStr = utils::sq120Str(pinnedPieceSq);
    string pinningPieceSqStr = utils::sq120Str(pinningPieceSq);
    string pinnedToSqStr = utils::sq120Str(pinnedToSq);

    string pinnedPieceStr = pieceNames[pinnedPieceType];
    string pinningPieceStr = pieceNames[pinningPieceType];
    string pinnedToPieceStr = pieceNames[pinnedToPieceType];

    string pinStr = "";
    pinStr += "The " + pinningPieceStr + " on " + pinningPieceSqStr + " pins the " + pinnedPieceStr + " on ";
    pinStr += pinnedPieceSqStr + " to the " + pinnedToPieceStr + " on " + pinnedToSqStr + "\n";
    
    // printf(pinStr.c_str());
    std::cout << pinStr;
}


void printCheckMatePin(Pin pin, Position *pos, int preventedMove, int move) {
    int side = pos->side;

    int pinnedPieceSq = pin.pinnedPiece;
    int pinningPieceSq = pin.pinningPiece;
    int pinnedToSq = pin.pinnedToSq;
    bool isPinAbsolute = pin.isPinAbsolute;

    int pinnedPieceType = pos->allPieces[pinnedPieceSq];
    int pinningPieceType = pos->allPieces[pinningPieceSq];
    int pinnedToPieceType = pos->allPieces[pinnedToSq];

    // printf("pinned piece type %d\n", pinnedPieceType);

    string pinnedPieceSqStr = utils::sq120Str(pinnedPieceSq);
    string pinningPieceSqStr = utils::sq120Str(pinningPieceSq);
    string pinnedToSqStr = utils::sq120Str(pinnedToSq);

    string pinnedPieceStr = pieceNames[pinnedPieceType];
    string pinningPieceStr = pieceNames[pinningPieceType];
    string pinnedToPieceStr = pieceNames[pinnedToPieceType];

    string preventedTargetSQ = utils::sq120Str(getTargetSQ(preventedMove));

    string pinStr = "";
    pinStr += "The " + pinningPieceStr + " on " + pinningPieceSqStr + " prevents the " + pinnedPieceStr + " on ";
    pinStr += pinnedPieceSqStr + " from moving to " + preventedTargetSQ + " because otherwise ";
    pinStr += pinningPieceStr + " to " + pinnedToSqStr + " would be checkmate\n";
    // " to the " + pinnedToPieceStr + " on " + pinnedToSqStr + "\n";
    
    // printf(pinStr.c_str());
    std::cout << pinStr;
}
