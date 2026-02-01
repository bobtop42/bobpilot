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

int setUpUART();

void engageAP();
void disengageAP();

void integrateControls(PLANE* plane);

CONTROLLER();

uint8_t flags = 0x2;
/*
0x01 correct crc
0x02 AP control, SC = -100

0x04 set hold pos, SC = 100
0x08 SA bit on/off
0x10 adj flap
0x20 landing/takeoff bit
0x20 se(the push button) on/off
ox40 landing/takeoff bit
*/

int8_t toggleables = 0x00;

void integrateAileron(FLAP *ap);
void integrateElevator(FLAP *ep);
void integrateThrottle(ATC * atc);
void integrateFlap(FLAP *fp);

int16_t aileron = 0;
int16_t elevator = 0;
int16_t throttle = 0;

int8_t toggleable = 0x00;
int8_t sa = 0x00;


void toggleSA();

int16_t ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10;

int fd;
};

#endif
