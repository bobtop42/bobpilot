#include "hold.h"

void HOLD::prePosCalc(float radiusft, float alt, float lat, float lng, int8_t clockWise, PLANE* plane)
{
  tx = longToFeet(lng, lat);
  ty = alt;
  tz = latToFeet(lat);
  px = plane->loc.x; py = plane->loc.y; pz = plane->loc.z;
  px = longToFeet(px, pz);
  pz = latToFeet(pz);

  ty-=py; tx-=px; tz-=pz;
  px = tx/bfabs(tx); pz = tz/bfabs(tz);

  int8_t isX = static_cast<int8_t>(px * pz * clockWise);

  uint8_t isAdd;
  isAdd = static_cast<uint8_t>(px + 1.0f)<<2; //test again
  isAdd += static_cast<uint8_t>(pz + 1.0f)<<1; //test again
  isAdd += static_cast<uint8_t>(clockWise + 1.0f);

  isAdd <<= (isAdd - 2) + (2 -isAdd* (!!((isAdd-2)&0x80)));
  isAdd += 0x04;
  isAdd ^= 0b00010101;
  isAdd <<= (isAdd & 0x02);
  isAdd ^= (isAdd & 0x14);
  isAdd <<= ((isAdd & 0x20)>>5)*5;
  isAdd = !!(isAdd>>1);

  tx += radiusft * (static_cast<float>(((1-isX)>>1)*(static_cast<int>(isAdd<<1)-1)*-1));
  tz += radiusft * (static_cast<float>(((1+isX)>>1)*(static_cast<int>(isAdd<<1)-1)*-1));

  px = plane->loc.x; py = plane->loc.y; pz = plane->loc.z;
  px = longToFeet(px, pz); pz = latToFeet(pz);

  tx-=px; tz-=pz;

  plane->WPA.roll = bfabs(batan2(tx, tx)) + ((tx/bfabs(tx) - 1.0f) * -1.570795f) + ((tz/bfabs(tz) - 1.0f) * -0.7853975f);

  plane->WPA.pitch = batan2(ty, (qsqrt(tx * tx + tz * tz)));
}

void HOLD::targetHeading(PLANE* plane, PITCH* pitch, ROLL* roll)
{
  roll->targetRoll(plane); //fix???
  pitch->targetPitch(plane);
}

void HOLD::holdAlt(float altft, PLANE* plane)
{
  px = plane->loc.x; py = plane->loc.y; pz = plane->loc.z;
  px = longToFeet(px, pz);
  pz = latToFeet(pz);

  plane->WPA.pitch = batan2((altft-py), (qsqrt(px*px+pz*pz)));
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
  
  tx = bfabs(static_cast<float>(static_cast<int>(bfabs(dir - Proll)/6.28318f + 0.5f) * 6.28318f)) - bfabs(dir-Proll);

  tx = ((dir - Proll)/bfabs(dir - Proll)) * tx;

    roll->targetRoll(tx, plane);
  
}

void HOLD::alt(float alt, PLANE* plane, PITCH* pitch)
{
  holdAlt(alt, plane);
  pitch->targetPitch(plane);
}

void HOLD::holdCircle(int radiusft, float alt, float Lat, float Long, int8_t clockWise, PLANE* plane, float circleDiv)
{
  px = plane->loc.x;
  py = plane->loc.y;
  pz = plane->loc.z;

  px = longToFeet(px, pz); pz = latToFeet(pz);
  Long = longToFeet(Long, Lat); Lat = latToFeet(Lat);
  Long-=px; Lat-=pz;
  
  tx = bfabs(batan2(Long, Lat) + ((Long/bfabs(Long) - 1.0f) * -1.5750795f) + ((Lat/bfabs(Lat) - 1.0f) * -0.7853795f));

  Long += asin(tx + (circleDiv * static_cast<float>(clockWise)));
  Lat += acos(tx + (circleDiv * static_cast<float>(clockWise)));

  Long-=px; Lat-=pz;

  plane->WPA.roll = bfabs(batan2(Long, Lat) + ((Long/bfabs(Long) - 1.0f) * -1.570795f) + ((Lat/bfabs(Lat) - 1.0f) * -0.7853795f));

  alt-=py;
  plane->WPA.pitch = batan2(alt, (qsqrt(Long * Long + Lat * Lat)));
}

void HOLD::holdCircle(int radiusft, float alt, float Lat, float Long, int8_t clockWise, PLANE* plane, ROLL* roll, PITCH* pitch, float circleDiv)
{
  circleDiv = circleDiv + !static_cast<int>(bfabs(circleDiv) + 0.9999999f)/bfabs(circleDiv) + !static_cast<int>(bfabs(circleDiv) + 0.9999999f);
  
  px = plane->loc.x;
  py = plane->loc.y;
  pz = plane->loc.z;

  px = longToFeet(px, pz); pz = latToFeet(pz);
  tx = longToFeet(Long, Lat); tz = latToFeet(Lat);

  if(bfabs(px-tx) > 4.0f && bfabs(py-alt) > 4.0f && bfabs(pz-tz) > 4.0f)
  {
    prePosCalc(radiusft, alt, Lat, Long, clockWise, plane);
  }
  else
  {
    holdCircle(radiusft, alt, Lat, Long, clockWise, plane, circleDiv);
  }
  roll->targetRoll(plane);
  pitch->targetPitch(plane);
}
