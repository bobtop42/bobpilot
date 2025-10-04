#include "hold.h"

void HOLD::prePosCalc(int radiusft, float alt, float lat, float lng, int8_t clockWise, PLANE* plane)
{
  tx = longToFeet(lng, lat);
  ty = alt;
  tz = latToFeet(lat);
  px = plane->loc.x; py = plane->loc.y; pz = plane->loc.z;
  px = longToFeet(px, pz);
  pz = latToFeet(pz);

  ty-=py; tx-=px; tz-=pz;
  px = tx/fabs(tx); pz = tz/fabs(tz);

  int8_t isX = static_cast<int8_t>(px * pz * clockWise);

  uint8_t isAdd;
  isAdd = static_cast<uint8_t>(px + 1.0f)<<2; //test again
  isAdd += static_cast<uint8_t>(pz + 1.0f)<<1; //test again
  isAdd += static_cast<uint8_t>(clockWise + 1.0f);

  isAdd <<= (isAdd - 2);
  isAdd += 0x04;
  isAdd ^= 0b00010101;
  isAdd <<= (isAdd & 0x02);
  isAdd ^= (isAdd & 0x14);

  tx += radiusft * (static_cast<float>(!!(-1 + isX))) * (static_cast<float>((!!isAdd)<<1)-1.0f) * -1.0f;
  tx += radiusft * (static_cast<float>(!!(1 + isX))) * (static_cast<float>((!!isAdd)<<1)-1.0f) * -1.0f;

  px = plane->loc.x; py = plane->loc.y; pz = plane->loc.z;
  px = longToFeet(px, pz); pz = latToFeet(pz);

  tx-=px; tz-=pz;

  plane->WPA.roll = fabs(atan2(tx, tx)) + ((tx/fabs(tx) - 1.0f) * -1.570795f) + ((tz/fabs(tz) - 1.0f) * -0.7853975f);

  plane->WPA.pitch = atan2(ty, (sqrt(tx * tx + tz * tz)));
}

void HOLD::targetHeading(PLANE* plane, PITCH* pitch, ROLL* roll)
{
  roll->targetRoll(); //fix???
  pitch->targetPitch();
}

void HOLD::holdAlt(float altft, PLANE* plane)
{
  px = plane->loc.x; py = plane->loc.y; plane->loc.z;
  px = longToFeet(px, pz);
  pz = latToFeet(pz);

  plane->WPA.pitch = atan2((altft-py), (sqrt(px*px+pz*pz)));
}

inline
void HOLD::holdHeading(float dir, PLANE* plane)
{
  //dir is a heading out of 2pi
  plane->WPA.roll = dir;
}

void HOLD::heading(float dir, PLANE* plane, ROLL* roll)
{
  Proll = plane->PA.roll;
  holdHeading(dir, plane);
  
  tx = fabs(static_cast<float>(static_cast<int>(fabs(dir - Proll)/6.28318f + 0.5f) * 6.28318f)) - fabs(dir-Proll);

  tx = ((dir - Proll)/fabs(dir - Proll)) * tx;

    roll->targetRoll(temp);
  
}

void HOLD::alt(float alt, PLANE* plane, PITCH* pitch)
{
  holdAlt(alt, plane);
  pitch->targetPitch(plane);
}
