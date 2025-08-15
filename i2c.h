#ifndef I2C_H
#define I2C_H
//#device DEVICE_ADDR = 0x68
#define DEVICE_ADDR = 0x68
#include "fcntl.h"
#include "sys/ioctl.h"
#include "linux/i2c-dev.h"
#include "unistd.h"
#include "stdint.h"
#include <iostream>
#include <ctime>

class I2C
{
public:
float pAngle[3][3];
bool errorI2Cdevice;
bool errorI2Cdata;
int fd = -1;
std::string dev = "/dev/i2c-1";

I2C();

void updatePA(int fd, float (&pAngle)[3][3]);
int setUp();

private:
//std::string dev = "/dev/i2c-1";
uint8_t config[5][2];
int16_t read16b9(int fd, uint buf_reg);
};

#endif
