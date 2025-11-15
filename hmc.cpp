#include "hmc.h"

using namespace HMC;

int write(int fd, uint8_t* command)
{
  int len = sizeof(command);

  if(write(fd, command, sizeof(command))!= 0)
  {
    if(write(fd, command, sizeof(command))!= 0) return -1;
  }
  return 0;
}

int config(int fd)
{
  uint8_t config[3] = {0x3C, 0x00, 0x70};
  if(write(fd, config)!= 0) return -1;
  config[1] = 0x01; config[2] = 0x40;
  if(write(fd, config)!= 0) return -1;
  config[1] = 0x02; config[2] = 0x01;
  if(write(fd, config)!= 0) return -1;
  usleep(5000);
  return 0;
}

int cycle(uint16_t command[3])
{
  int fd = open("/dev/i2c-1", O_RDWR);
  if(fd!=0)
  {
    close(fd);
    fd = open("/dev/i2c-1", O_RDWR);
    if(fd!=0)
    {
      close(fd);
      return -1;
    }
  }
  if(ioctl(fd, I2C_SLAVE, 0x1E)!=0)
  {
    if(ioctl(fd, I2C_SLAVE, 0x1E)!=0)
    {
      close(fd);
      return -1;
    }
  }

  int ConfigWrite = config(fd);
  uint8_t buf[6];
  buf[0] = 0x3D; buf[1] = 0x06;
  if(read(fd, buf, sizeof(buf))!=0)
  {
    close(fd);
    return -1;
  }

  command[0] = combine(buf, 0);
  command[1] = combine(buf, 2);
  command[2] = combine(buf, 4);

  close(fd);
  return 0;
}

double heading(PLANE* plane)
{
  //float x = plane->pAngle[0][0];
  //float y = plane->pAngle[0][1];
  //float z = plane->pAngle[0][2];

  float pitch = plane->PA.pitch;
  float roll = plane->PA.roll;

  uint16_t pos[3];
  int Cycle = cycle(pos);
  if(Cycle!=0) return -1;
  {
    Cycle = cycle(pos);
    if(Cycle!=0) return -10.0;
  }

  float pX = pos[0]; float pY = pos[1]; float pZ = pos[2];

  float Xh = pX * cos(pitch) + pZ * cos(pitch);
  float Yh = pX * sin(roll) * sin(pitch) + pY * cos(roll) - pZ * sin(roll) * cos(pitch);

  return atan2(Yh, Xh);
}

