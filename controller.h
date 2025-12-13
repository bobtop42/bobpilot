#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "MATHLIB.h"
#include "atc.h"
#include "sys/ioctl.h"
#include "sys/types.h"
#include "stdint.h"
#include <fcntl.h>                  
#include <cstdint>        

class CONTROLLER
{
public:
void update(PLANE* plane);
void inputChannelData(PLANE* plane, uint8_t* buf, int start);

void engageAP();
void disengageAP();

void fuseControls(PLANE* plane);

CONTROLLER();

uint8_t flags = 0x24;
/*
0x01 correct crc
0x02 AP control, SC = -100
0x04 human control, SC = 0
0x04 set hold pos, SC = 100
0x08 SA bit
0x10 adj flap?
0x20 landing/takeoff bit
*/

void fuseAileron(FLAP *ap);
void fuseElevator(FLAP *ep);
void fuseThrottle(ATC * atc);

void setAileron(FLAP* ap);
void setElevator(FLAP* ep);
void setThrottle(ATC* atc);

void engageFlaps();
void disengageFlaps();

uint16_t ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10;
};

#endif
