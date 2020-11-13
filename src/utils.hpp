#ifndef UTILS_HPP
#define UTILS_HPP

#include "defs.h"
#include "position.hpp"

#include <string>

using std::string;


namespace utils {
    
    void initUtils();
    string sq120Str(int sq120);
    void printSq120(int sq120);
    string moveAlgNot(int move, int piece);
    string moveAlgNot(int move, Position *pos);
    void printMoveAlgNot(int move, int piece);
    void printMoveAlgNot(int move, Position *pos);
};

#endif