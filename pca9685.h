#ifndef PCA_H
#define PCA_H
#include "plane.h"
#include "MATHLIB.h"
#include "ckalman.h"

#define PCA_ADDRESS 0x40
#define MODE1 0x00

#include <iostream>
#include <unistd.h>
#include <tuple>

//check the files below
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

class PCA
{
public:
void updateServos(PLANE* plane);

private:
int wakeSequence();
auto radianToStep(float radian);
int writeReg(int fd, uint8_t reg, float value);

int fd;
};

#endif
