#include <cmath>

#ifndef NAVSYS_H
#define NAVSYS_H

//NOTE: add npc counter. Burrently only a hardcoded int 

struct pt3D{float x = 0.0f; float y = 0.0f; float z = 0.0f;};

struct angle{float pitch = 0.0f; float roll = 0.0f;};

struct flapPos
{
  float fL = 0.0f; float fR = 0.0f;
  flapPos& operator+=(const flapPos& other)
  {
    fL += other.fL;
    fR += other.fR;
    return *this;
  }
  void rad(flapPos& myFlapPos);
};

struct EP
{
  flapPos elevator;
  const float MAX_POS = 60.0f; // placeholder until we get real data
  void elevatorAdj(EP ep);
};

struct AP
{
  flapPos aileron;
  const float MAX_POS = 60.0f; // placeholder until we get real data
  void aileronAdj(AP ep);
};

struct Plane
{
  pt3D loc;
  angle WPA;
  angle PA;
  EP ep;
  AP ap;
  float speed;
};

class NAVSYS
{
public:
Plane plane;

int npc = 1;

const float PI = 3.14159f;

void wayPointAngleFinder();
void planeAngleFinder(float kX[3][0]); //fill in with kalman filter stuff later

private:
float WPXYZ[14][3];

void updateEP(Plane& plane, float value);
void updateAP(float value);

};

#endif
