// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
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
int fd;
std::string dev = "/dev/i2c-1";

I2C();

void updatePA(PLANE* plane);
int update(PLANE *plane);
int setUp();
void shutdown();
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
