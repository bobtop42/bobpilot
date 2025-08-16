#ifndef PLANE_H
#define PLANE_H

class PLANE
{
public:
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

//waypoint data for gps
angle WPA;
pt3D loc;
float WPXYZ[14][3];
//plane data from MPU 6050
angle PA;
float pAngle[3][3];

EP ep;
AP ap;
//aspeed 
float speed;

};

#endif
