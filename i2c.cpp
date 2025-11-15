#include "i2c.h"
#include "io.h"
//MPU 6050 driver thingy

//I2C::I2C():fd(-1), dev("/dev/i2c-1") {}
I2C::I2C() {};


int I2C::setUp()
{
  fd = open("/dev/i2c-1", O_RDWR);
  ioctl(fd, I2C_SLAVE, DEVICE_ADDR);
  uint8_t config[5][2] = {{0x6B, 0x00}, {0x1B, 0x00}, {0x1C, 0x00}, {0x1A, 0x03}, {0x19, 0x09}};

  for(int i=0; i<5; ++i)
    {
      if(write(fd, config[i], 2) != 2)
      {
        std::cerr << "failed to config";
        errorI2Cdevice = true;
        return -1;
      }
    }
  if(fd < 0)
  {
    std::cerr <<"failed to connect";
    errorI2Cdevice = true;
    return -2;
  }
  if(ioctl(fd, I2C_SLAVE, DEVICE_ADDR) < 0)
  {
    close(fd);
    std::cerr << "failed to connect";
    errorI2Cdevice = true;
    return -3;
  }
}

int16_t I2C::read16b(int fd, int buf_reg)
{
  uint8_t buf[2];
  buf[0] = buf_reg;
  if(write(fd, buf, 1) != 1) return 0;
  if(read(fd, buf, 2) != 2) return 0;
  return (int16_t)(buf[0] << 8 | buf[1]);
}

void I2C::updatePA(int fd,PLANE* plane)
{
  //read accel data
  int16_t ax = read16b(fd, 0x3B);
  int16_t ay = read16b(fd, 0x3D);
  int16_t az = read16b(fd, 0x3F);
//work on this time stamp stuff
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
  
  long long ms_count = ms.count();

  //read temp data
  int16_t temp = read16b(fd, 0x41);

  //read gyro data
  int16_t gx = read16b(fd, 0x43);
  int16_t gy = read16b(fd, 0x45);
  int16_t gz = read16b(fd, 0x47);

  //accel dat -> m/s^2
  plane->pAngle[0][0] = ax / 16384.0f * 9.80665f;
  plane->pAngle[0][1] = ay / 16384.0f * 9.80665f;
  plane->pAngle[0][2] = az / 16384.0f * 9.80665f;

  //temp and time data
  plane->pAngle[1][0] = temp / 340.0f + 36.53f;
  plane->pAngle[1][2] = plane->pAngle[1][1];
  //plane->pAngle[1][1] = plane->time.timeUpdate;

  //gyro data -> rad/s
  plane->pAngle[2][0] = gx / 131.0f;
  plane->pAngle[2][1] = gy / 131.0f;
  plane->pAngle[2][2] = gz / 131.0f;
}
