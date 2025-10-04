#include "pca9685.h"

int PCA::wakeSequence()
{
  if((fd = open("/dev/i2c-1", O_RDWR))!=0)
  {
    close(fd);
    if((fd = open("/dev/i2c-1", O_RDWR))!=0)
    {
      close(fd);
      return -1;
    }
  }
  if(ioctl(fd, I2C_SLAVE, PCA_ADDRESS)!=0)
  {
    if(ioctl(fd, I2C_SLAVE, PCA_ADDRESS)!=0)
    {
      close(fd);
      return -1;
    }
  }
  uint8_t mode1[2] = {0x00, 0x00};
  if(write(fd, 0x00, 1)!= 1) 
  {
    close(fd);
    return -1;
  }
  if(read(fd, mode1, 1)!= 1) 
  {
    close(fd);
    return -1;
  }
  
  if((mode1[0] & 0x80)==0x80)
  {
    mode1[1] = mode1[0] | 0x08;
    mode1[0] = 0x00;
    if(write(fd, mode1, 2)!= 1)
    {
      close(fd);
      return -1;
    }
    usleep(5000);
    return 0;
  }
  else if((mode1[0] & 0x10)==0x10)
  {
    mode1[1] = mode1[0] | 0x80;
    mode1[0] = 0x00;
    if(write(fd, mode1, 2)!= 1)
    {
      close(fd);
      return -1;
    }
    usleep(5000);
    return 0;
  }
  else
  {
    close(fd);
    return -2;
  }
}

auto PCA::radianToStep(float radian)
{
  radian = ((1.570795f - radian)/6.2831f)*8192.0f;
  int value = round(radian);
  uint8_t L = static_cast<uint8_t>(value & 0xFF);
  uint8_t H = static_cast<uint8_t>((value >> 8) & 0xFF);
  return std::make_tuple(L, H);
}

int PCA::writeReg(int fd, uint8_t reg, float radian)
{
  auto [L, H] = radianToStep(radian);
  uint8_t buf[2] = {reg, 0x00};
  if(write(fd, buf, 2)!= 2) return -3;
  buf[0]++; buf[1] = 0x00;
  if(write(fd, buf, 2)!= 2) return -3;
  buf[0]++; buf[1] = L;
  if(write(fd, buf, 2)!= 2) return -3;
  buf[0]++; buf[1] = H;
  if(write(fd, buf, 2)!= 2) return -3;
  return 0;
}

void PCA::updateServos(PLANE* plane)
{
  bool servoUpdated = false;
  while(!servoUpdated)
    {
      fd = open("/dev/i2c-1", O_RDWR);

      uint8_t catchbuf[1];
      
      try
      {

        if(fd!=0)
          throw -2;

        ioctl(fd, I2C_SLAVE, PCA_ADDRESS);

        int wake = wakeSequence();
        if(wake!=0)
          throw wake;

        float aileronL = plane->ap.aileron.fL;
        float aileronR = plane->ap.aileron.fR;
        float elevatorL = plane->ep.elevator.fL;
        float elevatorR = plane->ep.elevator.fR;

        int aL = writeReg(fd, 0x06, aileronL);
        if(aL!=0)
          throw -1;
        int aR = writeReg(fd, 0x0A, aileronR);
        if(aR!=0)
          throw -1;
        int eL = writeReg(fd, 0x0E, elevatorL);
        if(eL!=0)
          throw -1;
        int eR = writeReg(fd, 0x12, elevatorR);
        if(eR!=0)
          throw -1;

        uint8_t sleepR[1] = {0x00};
        if(write(fd, sleepR, 1)!= 1) throw -1;
        if(read(fd, sleepR, 1)!= 1) throw -1;
        sleepR[0] |= 0x80;
        if(write(fd, sleepR, 1)!= 1) throw -1;
        servoUpdated = true;
        
      }
        catch(int error)
        {
          switch(error)
            {
              case -1:
                {
                  catchbuf[0] = 0x00;
                  if(write(fd, catchbuf, 1)!= 1)
                  {
                    close(fd);
                    SHUTDOWNERROR = true;
                    servoUpdated = true;
                    break;
                  }
                  if(read(fd, catchbuf, 1)!= 1)
                  {
                    close(fd);
                    SHUTDOWNERROR = true;
                    servoUpdated = true;
                    break;
                  }
                  else
                  {
                    catchbuf[0] |= 0x80;
                    if(write(fd, catchbuf, 1)!= 1)
                    {
                      close(fd);
                      SHUTDOWNERROR = true;
                      servoUpdated = true;
                      break;
                    }
                    else{close(fd);}
                  }
                }
              case -2:
                {
                  close(fd);
                  break;
                }
            }
        }
    }
}
