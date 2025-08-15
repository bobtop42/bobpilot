#include "pid.h"
#ifndef PITCH_H
#define PITCH_H

class PITCH
{
public:

enum class Dir{Up, Level, Down};
Dir PitchDir;

DESCRETE PitchPID;

PITCH(float targetPitch, float currentPitch, bool engaged);

void engaged(PITCH& myPitch);
void disengaged(PITCH& myPitch);
void update(DESCRETE *myDescrete);
void setUp();

private:
float targetPitch_;
float currentPitch_;
bool engaged_;

//add navsys stuff to parathesis 
void targetPitch();

};


#endif 
