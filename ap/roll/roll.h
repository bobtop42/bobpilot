#include "pid.h"
#ifndef ROLL_H
#define ROLL_H

class ROLL
{
public:

enum class Dir { Right, Straight, Left};
Dir RollDir;

DESCRETE RollPID;

ROLL(float targetRoll, float currentRoll, bool engaged);

void engaged(ROLL& myRoll);
void disengaged(ROLL& myRoll);
void update(DESCRETE* myDescrete);
void setUp();

private:
float targetRoll_;
float currentRoll_;
bool engaged_;

// add navsys stuff to parentheses
void targetRoll();

};

#endif
