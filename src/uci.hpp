#ifndef UCI_HPP
#define UCI_HPP

#include "defs.h"

#include <string>
#include <fstream>
#include <sstream>

#define INPUTBUFFER 24000

using std::string;
using std::cin;
using std::getline;
using std::istringstream;

struct Position;
struct SearchInfo;

namespace UCI {

    void loop();
    void go(Position *pos, SearchInfo &info, istringstream &iss);
    void position(Position *pos, istringstream &iss);


};

#endif