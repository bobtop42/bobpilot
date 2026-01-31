#ifndef I2C_H
#define I2C_H
#define DEVICE_ADDR 0x68
#include "MATHLIB.h"
#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include <unistd.h>
#include "stdint.h"
#include <ctime>
#include <fcntl.h>
#include <cstdint>        
#include <iostream>
#include <chrono>
#include <tuple>
#include <thread>


class I2C
{
public:
bool errorI2Cdevice;
bool errorI2Cdata;
int fd = 0;
std::string dev = "/dev/i2c-1";

I2C();

void updatePA(PLANE* plane);
int update(PLANE *plane);
int setUp();
auto calibrate(int loops);

int errorhandler(int error);
int errorhandlerswitchtable(int errorlvl);

private:
//std::string dev = "/dev/i2c-1";
uint8_t config[5][2];

uint8_t buf[14];
int readdata();
int16_t axm, aym, azm, gxm, gym, gzm;

int busHealthCheck;
};

#endif
