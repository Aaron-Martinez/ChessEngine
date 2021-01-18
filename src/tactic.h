#ifndef TACTIC_HPP
#define TACTIC_HPP

#include "defs.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using std::string;
using std::cin;
using std::getline;
using std::istringstream;
using std::vector;

struct Position;
struct SearchInfo;


struct Pin {
    int pinnedPiece;
    int pinningPiece;
    int pinnedToSq;
    bool isPinAbsolute;
};

vector<Pin> getAllPins(Position *pos);
bool isPin(Position *pos, int move);



#endif