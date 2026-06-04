// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#include "controller.h"

bool CONTROLLER::setUp()
{
  fd = open("/DEV/SERIAL0", O_RDWR | O_NOCTTY | O_NDELAY);
  if(fd!= 0)
  {
    close(fd);
    fd = open("/DEV/SERIAL0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd!=0)
    {
      close(fd);
      return false;
    }
  }
  return true;
}

void CONTROLLER::shutDown()
{
  close(fd);
}

void CONTROLLER::update(PLANE* plane)
{
  uint8_t buf[128];
  int i = 0;

  read(fd, buf, 128);

  while(i<127)
    {
      if(buf[i]==0xC8 && buf[i+1]>1 && buf[i+1]<129)
      {
        inputChannelData(plane, buf, i+3);
      };
    }
}

void CONTROLLER::inputChannelData(PLANE *plane, uint8_t *buf, int start)
{

  ail = ((buf[start]<<3) | (buf[start+1]>>5)) & 0x07FF;
  ele = ((buf[start+1]<<6) | (buf[start+2]>>2)) & 0x07FF;
  thr = ((buf[start+2]<<9) | (buf[start+3]<<1) | (buf[start+4]>>7)) & 0x07FF;
  rud = ((buf[start+4]<<4) | (buf[start+5]>>4)) & 0x07FF;
  SA = ((buf[start+5]<<7) | (buf[start+6]>>1)) & 0x07FF;
  SB = ((buf[start+6]<<10) | (buf[start+7]<<2) | (buf[start+8]>>6)) & 0x07FF;
  SC = ((buf[start+8]<<5) | (buf[start+9]>>3)) & 0x07FF;
  SD = ((buf[start+9]<<8) | buf[start+10]) & 0x07FF;
  SE = ((buf[start+11]<<1) | (buf[start+12]>>7)) & 0x07FF;
  SF = ((buf[start+12]<<4) | (buf[start+13]>>4)) & 0x07FF;
};

void CONTROLLER::toggleSA()
{
  toggleable = !(sa&0x8) * !toggleable;
}

void CONTROLLER::integrateAileron(FLAP *ap)
{
  int16_t t = (flags&0x2)>>1;
  ap->flap.fL = (static_cast<float>(t) * ap->flap.fL) + (static_cast<float>((!!((flags&0x20)+!t))*aileron*-1)/1.6666666f);
  ap->flap.fR = ap->flap.fL;
  ap->flapAdj(*ap);
}

void CONTROLLER::integrateElevator(FLAP *ep)
{
  int16_t t = (flags&0x2)>>1;
  ep->flap.fL = (static_cast<float>(t) * ep->flap.fL) + (static_cast<float>((!!((flags&0x20)+!t))*aileron*-1)/1.6666666f);
  ep->flap.fR = ep->flap.fL;
  ep->flapAdj(*ep);
}

void CONTROLLER::integrateThrottle(ATC *atc)
{
  int16_t t = (flags&0x2)>>1;
  atc->atcMod_ = (atc->atcMod_ * static_cast<float>(t)) + (static_cast<float>((!!((flags&0x20)+!t)) * throttle + (100*t)) / 200.0f);
  atc->targetSpeed(atc->atcMod_);
}

void CONTROLLER::integrateFlap(FLAP *fp)
{
  fp->flap.fR = fp->flap.fL = 20.0f * static_cast<float>((0x20&flags)>>5);
}

void CONTROLLER::integrateRudder(FLAP *rp)
{
  rp->flap.fR = rp->flap.fL = rudder * static_cast<float>(!(0x2&flags));
}

void CONTROLLER::updateControlMode()
{
  flags = (((flags&0x2)*!toggleable)|flags) | (((flags&0xFC)*toggleable)|(toggleable*((SA&0x7000)>>14)));
}

void CONTROLLER::updateLandingMode()
{
  flags = (((flags&0x20)*!toggleable)|flags) | (((flags&0xCF)*toggleable)|((!((SA-50)&0x7000)))<<5);
}
