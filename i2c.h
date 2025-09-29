#ifndef I2C_H
#define I2C_H
//#device DEVICE_ADDR = 0x68
#define DEVICE_ADDR 0x68

#include "plane.h"

#include <fcntl.h>
#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include <unistd.h>
#include "stdint.h"
#include <iostream>
#include <ctime>
#include <fcntl.h>        
#include <unistd.h>       
#include <sys/ioctl.h>    
#include <linux/i2c-dev.h>
#include <cstdint>        
#include <iostream>
#include <chrono>


class I2C
{
public:
bool errorI2Cdevice;
bool errorI2Cdata;
int fd = -1;
std::string dev = "/dev/i2c-1";

I2C();

void updatePA(int fd, PLANE*  plane);
int setUp();

private:
//std::string dev = "/dev/i2c-1";
uint8_t config[5][2];
int16_t read16b(int fd, int buf_reg);
};

#endif
