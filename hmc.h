#ifndef HMC_H
#define HMC_H
#include "plane.h"
#include "MATHLIB.h"
#include <iostream>
#include <unistd.h>
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

namespace HMC
{
  class HMC
  {
  public:
  double heading(PLANE* plane);

  private:
  int write(int fd, uint8_t* command);
  int config(int fd);
  int cycle(uint16_t command[3]);
  //look into conv. X/Y/Z to compass dir.
  };
}

#endif
