#include "plane.h"

void PLANE::flapPos::rad(flapPos& myFlapPos)
{
  fL = (myFlapPos.fL / 360.0f);
  fR = (myFlapPos.fR / 360.0f);
};

void PLANE::EP::elevatorAdj(EP ep)
{
  if(ep.elevator.fL > 60.0f)
  {
    ep.elevator.fL = 60.0f;
    ep.elevator.fR = 60.0f;
    return;
  }
  else if(ep.elevator.fL < -60.0f)
  {
    ep.elevator.fL = -60.0f;
    ep.elevator.fR = -60.0f;
  }
};

void PLANE::AP::aileronAdj(AP ap)
{
  if(ap.aileron.fL > 60.0f)
  {
    ap.aileron.fL = 60.0f;
    ap.aileron.fR = -60.0f;
    return;
  }
  else if(ap.aileron.fL < -60.0f)
  {
    ap.aileron.fL = -60.0f;
    ap.aileron.fR = 60.0f;
  }
};
