#ifndef M2H_H
#define M2H_H
#define I2C_DEV "/dev/i2c-1"

#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include <unistd.h>
#include "stdint.h"
#include <fcntl.h>        
#include <unistd.h>       
#include <sys/ioctl.h>    
#include <linux/i2c-dev.h>
#include <cstdint>        
#include <iostream>
#include <chrono>
#include <cstring>
//namespace M2H
//{
  
class M2H
{
private:
void setAllSpeeds(uint16_t speed, uint8_t* command);
void setBrake(uint16_t brake, int num, uint8_t* command);
void crcOn(uint8_t* command);
void crcOff(uint8_t* command);
void clearLatchedFlags(uint16_t flags, uint8_t* command);
void setLatchedFlags(uint16_t flags, uint8_t* command);
void setCmdTimeout(uint8_t timeout, uint8_t* command);
void maxForwardAccel(uint16_t speed, int num, uint8_t* command);
void maxForwardDeAccel(uint16_t speed, int num, uint8_t* command);
void startForwardSpeed(uint16_t start, int num, uint8_t* command);
void errorResponse(uint8_t error, uint8_t* command);
void setSpeed(uint16_t speed, uint8_t motor, uint8_t* command);
uint8_t crcAppend(uint8_t * msg, uint8_t len);
//int write(uint8_t * msg, uint8_t& len, uint8_t motor);
int Write(uint8_t * msg, uint8_t& len, uint8_t motor, bool& crc);

public:
int command(uint16_t command, uint16_t value, uint8_t motor);
int setUp();
void setSpeed(uint16_t speed);
//bool CRC = false;
private:
std::string dev = "/dev/i2c-1";
const uint M2H_ADDR = 0x10;
bool CRC = false;
  
};

//}

#endif
