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

void integrateControlls(PLANE* plane);

CONTROLLER();

uint8_t flags = 0x2;
/*
0x01 correct crc
0x02 AP control, SC = -100
0x04 set hold pos, SC = 100
0x08 SA bit on/off
0x10 adj flap
0x20 landing/takeoff bit
0x40 SE(push botton) on/off
*/

void integrateAileron(FLAP *ap);
void integrateElevator(FLAP *ep);
void integrateThrottle(ATC * atc);
void integrateFlap(FLAP *fp);
void integrateRudder(FLAP *rp);

void updateControllMode();
void updateLandingMode();

int16_t airleron = 0;
int16_t elevator = 0;
int16_t throttle = 0;
int16_t rudder = 0;

int8_t toggleable = 0x00;
int8_t sa;

uint16_t ail, ele, thr, rud, SA,SB, SC, SD, SE, SF;

int fd;
};

#endif
