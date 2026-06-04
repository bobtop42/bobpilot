// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#include "MATHLIB.h" //REPLACE TRIG WITH BTRIG
#include "blib/bmath.h"

using namespace HMC;

int write(int fd, uint8_t* command)
{

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
  if(ConfigWrite!=0)
  {
    close(fd);
    return -1;
  }
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

  float cosp = bcos(pitch); float sinr = bsin(roll);
  float Xh = pX * cosp + pZ * cosp;
  float Yh = pX * sinr * bsin(pitch) + pY * bcos(roll) - pZ * sinr * cosp;

  return batan2(Yh, Xh);
}
