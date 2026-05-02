#include "i2c.h"
//MPU 6050 driver thingy

I2C::I2C() {}

int I2C::readdata()
{
    buf[0] = 0x3B;
    if (write(fd, buf, 1) != 1)
        return -1;
    if (read(fd, buf, 14) != 14)
        return -1;
    return 0;
}

int I2C::update(PLANE* plane)
{
    if (readdata() != 0)
        return -1;

    int16_t ax, ay, az, gx, gy, gz;
    ax = (buf[0] << 8) | buf[1];
    ay = (buf[2] << 8) | buf[3];
    az = (buf[4] << 8) | buf[5];
    gx = (buf[8] << 8) | buf[9];
    gy = (buf[10] << 8) | buf[11];
    gz = (buf[12] << 8) | buf[13];

    plane->pAngle[0][0] = ((float)ax / 16384.0f * 9.80665f) - axm;
    plane->pAngle[0][1] = ((float)ay / 16384.0f * 9.80665f) - aym;
    plane->pAngle[0][2] = ((float)az / 16384.0f * 9.80665f) - azm;

    constexpr float torad = 131.0f * (3.14156f / 180.0f);
    plane->pAngle[2][0] = ((float)gx / torad) - gxm;
    plane->pAngle[2][1] = ((float)gy / torad) - gym;
    plane->pAngle[2][2] = ((float)gz / torad) - gzm;

    using clock = std::chrono::steady_clock;
    static clock::time_point last = clock::now();
    clock::time_point now = clock::now();
    plane->dt = std::chrono::duration<float>(now - last).count();
    last = now;

    int t =
        (!static_cast<int>(plane->dt + 0.9997f) * 1) +
        (!!static_cast<int>(plane->dt + 0.99f) * 2);

    plane->dt =
        plane->dt * static_cast<float>(!t) +
        (static_cast<float>(t & 0x1) * 0.0002f) +
        (static_cast<float>(t & 0x2) * 0.01f);

    return 0;
}


int I2C::errorhandler(int error)
{

  int solved;
  int step = 0;
  int s;
  int sc;
  int errorseq[4] = {-1,-3,-4,-5};
  s = 0x4-(!(error-3)) - (!(error&0x03));
  s-= (!(error&0x4))*3;

  sc = (!(~error&0x6))<<2;
  sc |= (!(error&0x2))<<1;
  sc |= (!((error+1)&0x1))&(!(error&0x2)>>1);

  errorseq[0]-=!(error+2);

  int i = 0;

  while(i < sc)
  {
    solved = errorhandlerswitchtable(errorseq[i+sc]);
    step = i;
    i = (!!solved*i) | (!solved*sc) | ((!((i+sc)-3)*sc));
  }

  if((step+s)<=3){close(fd); SHUTDOWNERROR = true; return -1;}

  step-=1;

  for(int i=step; i>=0; --i)
    {
      if((solved = errorhandlerswitchtable(errorseq[i]))!=0)
      {
        close(fd);
        SHUTDOWNERROR = true;
        return -1;
      }
    }
  return 0;
}

int I2C::errorhandlerswitchtable(int errorlvl)
{
  uint8_t bufval = 0x75 ^ (!(((errorlvl) - 3) & 0x1E));
  uint8_t buf[1] = {bufval};

  int32_t el2 = ~errorlvl; el2 = (el2&0x1) | (!el2);
  el2 |= ((~errorlvl) & 0x4) | ((!((~errorlvl) - 2)) << 1);

  switch(el2)
    {
      case 1:
        {
          int retval = errorlvl & 2; retval |= retval >> 1; retval |= -4;
          if(write(fd, buf, 1) != 1) return retval;
          if(read(fd, buf, 1) != 1) return -1 ^ ((!(retval&0x2)) << 1);
          uint8_t retcheck = !(errorlvl + 5);
          retcheck = (buf[0] * (!retcheck)) | (0x68 * retcheck);
          if(retcheck != 0x68) return -1;
          buf[0] |= 0x80;
          if(errorlvl == -4){if(write(fd,buf,1)!=1)return -7;}
          auto sleept = (((~errorlvl) + 1) >> 1);
          sleept <<= (sleept & 0x2); sleept |= (sleept&0x8) >> 1;
          sleept *= 10;
          std::this_thread::sleep_for(std::chrono::milliseconds(sleept));
          return 0;
        }
      case 2:
        {
          if((ioctl(fd, I2C_SLAVE, DEVICE_ADDR)) < 0) return -3;
          return 0;
        }
      case 3:
        {
          close(fd);
          if((fd = open("/dev/i2c-1", O_RDWR))==-1) return -6;
          return 0;
        }
      default:
        {
          return -8;
        }
    }
  return 0;
}

auto I2C::calibrate(int loops)
{
    int16_t zero[6] = {0,0,0,0,0,0};
    uint16_t ax; uint16_t  ay; uint16_t az; uint16_t gx; uint16_t gy; uint16_t gz;

    for(int i=0; i<loops; i++)
    {
        if(readdata()!=0)
            return std::make_tuple(NAN, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

        ax = (buf[0] << 8) | buf[1];
        ay = (buf[2] << 8) | buf[3];
        az = (buf[4] << 8) | buf[5];
        gx = (buf[8] << 8) | buf[9];
        gy = (buf[10] << 8) | buf[11];
        gz = (buf[12] << 8) | buf[13];

        zero[0] += (double)ax/16384.0 * 9.8665;
        zero[1] += (double)ax/16384.0 * 9.8665;
        zero[2] += (double)ax/16384.0 * 9.8665;

        constexpr double torad = 131.0f * (3.14159/180.0);
        zero[3] += (double)gx/torad;
        zero[4] += (double)gx/torad;
        zero[5] += (double)gx/torad;
    }

    zero[0]/=static_cast<double>(loops); zero[1]/=static_cast<int>(loops);
    zero[2]/=static_cast<double>(loops); zero[3]/=static_cast<int>(loops);
    zero[4]/=static_cast<double>(loops); zero[5]/=static_cast<int>(loops);

    return std::make_tuple((float)zero[0], (float)zero[1],
                           (float)zero[2], (float)zero[3],
                           (float)zero[4], (float)zero[5]);
}

void I2C::updatePA(PLANE* plane)
{
    int check = update(plane);
    if (check == -1)
    {
        check = errorhandler(-2);
        if (check == 0)
        {
            update(plane);
        }
    }

    busHealthCheck++;
    if (busHealthCheck == 2000)
    {
        check = errorhandler(-1);
        busHealthCheck = 0;
    }
}

  int I2C::setUp()
  {
      fd = open(dev.c_str(), O_RDWR);
      if (fd < 0)
      {
          std::cerr << "failed to connect\n";
          errorI2Cdevice = true;
          return -2;
      }
      if (ioctl(fd, I2C_SLAVE, DEVICE_ADDR) < 0)
      {
          close(fd);
          std::cerr << "failed to connect\n";
          errorI2Cdevice = true;
          return -3;
      }

      uint8_t config[5][2] =
      {
          {0x6B, 0x00},
          {0x1B, 0x00},
          {0x1C, 0x00},
          {0x1A, 0x03},
          {0x19, 0x09}
      };

      for (int i = 0; i < 5; ++i)
      {
          if (write(fd, config[i], 2) != 2)
          {
              std::cerr << "failed to config\n";
              errorI2Cdevice = true;
              return -1;
          }
      }

      auto[axmt, aymt, azmt, gxmt, gymt, gzmt] = calibrate(3000);
      if(std::isnan(axmt))
          return -1;

      axm = axmt; aym = aymt; azm = azmt;
      gxm = gxmt; gym = gymt; gzm = gzmt;

      return 0;
  }

void I2C::shutdown(){close(fd);}



//old errorhandlerswitchtable below:
/*
int I2C::errorhandlerswitchtable(int errorlvl)
{
  switch(errorlvl)
    {
      case -1:
        {
          uint8_t buf[1] = {0x75};
          if(write(fd, buf, 1) != 1) return -1;
          if(read(fd, buf, 1) != 1) return -1;
          //check this below
          if(buf[0]!=0x68)return -1;
          return 0;
        }
      case -2:
        {
          for(int i=0; i<3; i++)
            {
              uint8_t buf[1] = {0x75};
              if(write(fd, buf, 1) != 1) return -1;
              if(read(fd, buf, 1) != 1) return -1;
              //check this below
              if(buf[0]!=0x68)return -1;
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
              return 0;
            }
          return -2;
        }
      case -3:
        {
          if((ioctl(fd, I2C_SLAVE, DEVICE_ADDR)) < 0) return -3;
          return 0;
        }
      case -4:
        {
          uint8_t buf[1] = {0x6B};
          if(write(fd, buf, 1)!=1)
            return -4;
          if(read(fd, buf, 1)!=1)
            return -5;
          *buf|=0x80;
          if(write(fd, buf, 1)!= 1)
            return -7;
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          return 0;
        }
      case -5:
        {
          close(fd);
          if((fd = open("/dev/i2c-1", O_RDWR))==-1) return -6;
          return 0;
        }
      default:
        return -8;

    }
  return 0;
};
*/
