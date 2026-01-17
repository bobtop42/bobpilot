#pragma once

#define PCA_ADDRESS 0x40
#define MODE1 0x00

#include "MATHLIB.h"
#include "atc.h"
#include <iostream>
#include <unistd.h>
#include <tuple>
#include <cmath>
#include <fcntl.h>
#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include <stdint.h>
#include <ctime>
#include <cstdint>
#include <cstdlib>

class PCA
{
private:
    int fd;

    int wakeSequence();
    auto radianToStep(float radian);
    int writeReg(uint8_t reg, float radian);

public:
    bool PCASHUTDOWNERROR = false;

    PCA();

    int toggleAutoIncrament(uint8_t togglesettings);
    int setUp();
    void updateServos(PLANE* plane, ATC *atc);
};
