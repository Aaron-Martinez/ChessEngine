#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include "defs.h"


struct Position;


int evaluate(const Position *pos);
int evalPieces(const Position *pos);
int evalPawnStructure(const Position *pos);
int evalKings(const Position *pos);
bool isMaterialDraw(const Position *pos);

#endif