#include "controller.h"

void CONTROLLER::update(PLANE* plane)
{
  int fd = open("/DEV/SERIAL0", O_RDWR | O_NOCTTY | O_NDELAY);
  if(fd!= 0)
  {
    close(fd);
    fd = open("/DEV/SERIAL0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd!= 0)
    {
      close(fd);
      return;
    }
  }

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
  
  ch1 = ((buf[start]<<3) | (buf[start+1]>>5)) & 0x07FF;
  ch2 = ((buf[start+1]<<6) | (buf[start+2]>>2)) & 0x07FF;
  ch3 = ((buf[start+2]<<9) | (buf[start+3]<<1) | (buf[start+4]>>7)) & 0x07FF;
  ch4 = ((buf[start+4]<<4) | (buf[start+5]>>4)) & 0x07FF;
  ch5 = ((buf[start+5]<<7) | (buf[start+6]>>1)) & 0x07FF;
  ch6 = ((buf[start+6]<<10) | (buf[start+7]<<2) | (buf[start+8]>>6)) & 0x07FF;
  ch7 = ((buf[start+8]<<5) | (buf[start+9]>>3)) & 0x07FF;
  ch8 = ((buf[start+9]<<8) | buf[start+10]) & 0x07FF;
  ch9 = ((buf[start+11]<<1) | (buf[start+12]>>7)) & 0x07FF;
  ch10 = ((buf[start+12]<<4) | (buf[start+13]>>4)) & 0x07FF;
};

void  CONTROLLER::engageAP()
{
  flags^=0x06;
}

void  CONTROLLER::disengageAP()
{
  flags^=0x06;
}

void  CONTROLLER::fuseAileron(FLAP *ap)
{
  ap->flap.fR = ap->flap.fL+=ch1;
  ap->flapAdj(*ap);
}

void CONTROLLER::fuseElevator(FLAP *ep)
{
  ep->flap.fR = ep->flap.fL += ch2;
  ep->flapAdj(*ep);
}

void CONTROLLER::fuseThrottle(ATC *atc)
{
  
}

void CONTROLLER::setAileron(FLAP *ap)
{
  
}
