// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#include "gps.h"
#include <cstring>
#include <tuple>

void GPS::resetbuf(char *buf)
{
  int n = read(fd, buf, sizeof(buf));
  if(n<0){GPGGA = false;}
}

void GPS::parseDataBuffer()
{
  char buf[256];
  char tbuf[16];
  int32_t tbl = 0; //temp buffer data length
  int32_t tbi = 0; //temp buffer index
  int32_t gpsdata[14] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int32_t gpst = 0; //gps data tempstorage
  int32_t gpsdc = 0; //gps data counter
  int32_t dcn = 0;
  int32_t randtemp;

  char checkSum;

  int n = read(fd, buf, sizeof(buf));
  if(n<0)
  {
    close(fd);
    return;
  }

  if(true)//fill in if statement later
  {
    int i = 0; int pc = 1; int ps;
    while(i<256||buf[i]!='$'){i++;}
    ps = i;
    int64_t type = ((int64_t) buf[ps+1]<<32) | ((int64_t) buf[ps+2]<<24) | ((int64_t) buf[ps+3]<<16) | ((int64_t) buf[ps+4]<<8) | ((int64_t) buf[ps+5]);
    if(type == 0x004750474741) //0x004750474741 is GPGGA in hex
    {
      checkSum = 0b01010110;
      i += 7;
      if(i<251) //UTC time
      {
        checkSum ^= buf[i] ^ buf[i+1] ^ buf[i+2] ^ buf[i+3] ^ buf[i+4] ^ buf[i+5];
        gpsdata[0] = (buf[i]<<8) | (buf[i+1]); //UTC hours
        gpsdata[1] = (buf[i+2]<<8) | (buf[i+3]); //UTC minutes
        gpsdata[2] = (buf[i+4]<<8) | (buf[i+5]); //UTC seconds
        gpsdc = 3;
        //cs = 0;
      }
      else
      {
        tbl = 256 - i;
        memcpy(&buf[i], &tbuf,tbl);
        resetbuf(buf);
        ps = 1;
        while(tbi < tbl) //add error checking to make sure it can handle if the buf only had 'GPGGA', and no data after. currently, it assumes at least one byte is present after the GPGGA header 
          {
            checkSum ^= tbuf[tbi];
            gpsdata[gpsdc] |= tbuf[tbi] << ps << 3;
            gpsdc += (ps ^ 1) & 0x1;
            ps ^= 1;
            tbi++;
          }
        int32_t addData = -((int8_t) ps ^ 1);
        randtemp = buf[0] & addData;
        checkSum ^= randtemp;
        gpsdata[gpsdc] |= randtemp;
        i = 0; i += addData & 0x1;
        gpsdc += i;
        
        gpst = (buf[i] << 8) | (buf[i+1]);
        randtemp = -((gpsdc) & 1);
        gpst &= randtemp;
        checkSum ^= (buf[i] ^ buf[i+1]) & randtemp;
        gpsdata[gpsdc] |= gpst;
        randtemp = (gpsdc & 1) << 1;
        gpsdc += randtemp; i += randtemp;
        
        gpst = (buf[i] << 8) | (buf[i+1]);
        checkSum ^= buf[i] ^ buf[i+1];
        gpsdata[gpsdc] |= gpst;
        gpsdc = 3; i += 2;
      }
      while(buf[i]!=',')
      {
        checkSum ^= buf[i];
        if(i<256){resetbuf(buf);}
        i++;
      }
      checkSum ^= ',';
      i++;
      if(i<255)
      {
        tbl = 256 - i;
        memcpy(&buf[i], &tbuf,tbl);
        resetbuf(buf);

        //if i has 1 byte left then it puts the deg from the temp buf into gpsdat for lat, else its zeroed out
        randtemp = (-(i & 0x1)) & 0xFF;
        ps = randtemp & 0x1;
        gpst = tbuf[0] & randtemp;
        checkSum ^= gpst;
        gpsdata[4] = gpst << 8;
        
        //if i had 0 bytes left the it did not copy from the tempt buf, therefore ~0 & 0xFF == 0xFF and we can use the buf at 0 to get the data and then zero out the data if we already copyied from the temp buf and not the newly read data
        randtemp = (~randtemp) & 0xFF;
        gpst = buf[0] & randtemp;
        checkSum ^= gpst;
        gpsdata[4] |= gpst << 8;
        
        //if we copied from the temp buf randtemp will be 0x0, so then we know the 2nd digit of degrees is the first byte of the buf, but if we didnt, then randtemp will be 0xFF and we can mask it to get the second byte from the new buf making us grab from the first byte or 2nd depending on which we need to get it from
        randtemp = buf[randtemp & 1];
        checkSum ^= randtemp;
        gpsdata[4] |= randtemp;
      }
    }
  }
}

void GPS::validateData(PLANE* plane)
{
  //NOTE: this function is not complete but rather serves as a outline for an anti-jamming/spoofing detection algorithm that will be integrated into ohter areas of the codebase toincrease the speed primarily by reducing the amount of data that must be calculated twice amongst other reasons and benifits. This function will most likely be integrated into the errorCalc function in the navsys file

  //sqrt assumes m/s^2
  float px, py, pz, rslope, pslope;
  int32_t temp;

  //calc est pos from old gps data. Note 'gps' data does not imply that it is actually from the gps but rather is gps if data is trusted, or blended data from multiple sources if data is not as trusted. This design allows us to handle only one input/output from location data but also blend it before it gets to this step, creating a injection point to blend data. This "anonimizes" the data's source for downstream processing, making the sure the most rusted data is recieved from one place regardless of how it was obtained.
  px = qsqrt(plane->pAngle[0][0]) * (float) plane->dt;
  temp = fti32 plane->pAngle[0][0] & signbit32;
  temp |= fti32 px; px = itf32 temp;
  px += plane->planeft.old.pxft; //plane.old... will be added at a later date. just a placeholder for now

  py = qsqrt(plane->pAngle[0][1]) * (float) plane->dt;
  temp = fti32 plane->pAngle[0][1] & signbit32;
  temp |= fti32 py; py = itf32 temp;
  py += plane->planeft.old.pyft;

  pz = qsqrt(plane->pAngle[0][2]) * (float) plane->dt;
  temp = fti32 plane->pAngle[0][2] & signbit32;
  temp |= fti32 pz; pz = itf32 temp;
  pz += plane->planeft.old.pzft;

  //slopes. 0x7F8 -> inf (up), 0xFF8 -inf (down)
  // if a slope == 0, the inverse will be +/- inf, so it will be looked at below when calculating the given gps pos, and if its reasonable or not

}


void GPS::punctuationMarker(const std::string gpsmsg)
{
  int i = 0;
  int pc = 1; //punctuation counter
  int gpsmsglen = gpsmsg.length();
  for(int i2=0; i<gpsmsglen; i2++)
  {
    if(gpsmsg[i2]=='$'){puncLocation[0]=i2; i = i2; break;}
  }

  i++;

  while(gpsmsg[i]!='\r')
    {
      puncLocation[pc] = i;
      pc = pc + !(gpsmsg[i]-',') + !(gpsmsg[i]-'*') + !(gpsmsg[i]-'.') + !(gpsmsg[i]-'-');
      i++;
    }
}

/*std::tuple<int, int, int, float, const char, float, const char, int, float> */ auto GPS::parser(const std::string gpsmsg, int* puncLocation)//-> std::tuple<int,int,int,float,char,float,char,int,float>
{
  //if(gpsmsg.substr(puncLocation[0]+1,6) != "GPGGA")
  //{
  //  GPGGA = true;
  //}
  GPGGA = (static_cast<int>(gpsmsg[0]+1) + static_cast<int>(gpsmsg[0]+2) + 
           static_cast<int>(gpsmsg[0]+3) + static_cast<int>(gpsmsg[0]+4) +   
           static_cast<int>(gpsmsg[0]+5) + static_cast<int>(gpsmsg[0]+6)) 
           - 356;

  int i=1;
  //UTC itme
  //hours
  int hrs = std::stoi(gpsmsg.substr(puncLocation[i]+1, 2));
  //minutes
  int min = std::stoi(gpsmsg.substr(puncLocation[i]+3, 2));
  //seconds
  int sec = std::stoi(gpsmsg.substr(puncLocation[i]+5, 2));

  i+=2;
  int t = (!(gpsmsg[i+1]-'-'));
  //lat in deg format DDMM.MMMM
  float lat = std::stof(gpsmsg.substr(puncLocation[i]+1, t+2));
  i+=1+t;

  float latMin = std::stof(gpsmsg.substr(puncLocation[i]-2,1+puncLocation[i+1]-puncLocation[i]));
  float gpsLatLocation = lat + latMin/60.0f;
  i++;
  char latHemisphere = gpsmsg[puncLocation[i]+1];
  t=(!gpsmsg[i+1]-'-');
  //long in deg in DDDMM.MMMM format
  float Long = std::stoi(gpsmsg.substr(puncLocation[i]+1,3+t));
  i+=1+t;
  float LongMin = std::stof(gpsmsg.substr(puncLocation[i]-2, 1+puncLocation[i+1]-puncLocation[i]));
  float gpsLongLocation = Long + LongMin/60.0f;
  i++;
  char LongHemisphere = gpsmsg[puncLocation[i]+1];
  i++;
  int gpsMeasureAccuracy = std::stoi(gpsmsg.substr(puncLocation[i]+1, 1));
  i++;

  int numSat = std::stoi(gpsmsg.substr(puncLocation[1+i]-puncLocation[i]));
  i++;
  int hdop = std::stoi(gpsmsg.substr(puncLocation[i], puncLocation[i+1]-puncLocation[i]));
  i++;
  float gpsAlt = std::stof(gpsmsg.substr(puncLocation[i], 1+puncLocation[i+1]-puncLocation[i]));
  i++;
  char gpsAltUnit = gpsmsg[puncLocation[i]+1];
  i++;

  //if(toupper(gpsAltUnit) == 'M')
  //{
  //  gpsAlt = toFeet(gpsAlt);
  //}
  //check below for accuracy to above code
  gpsAlt = gpsAlt * (1.0f + static_cast<float>((gpsAltUnit&0x1)*0x4011F948));

  float gpsGeoidalSep = std::stof(gpsmsg.substr(puncLocation[i], 1+puncLocation[i+1]-puncLocation[i]));

 char gpsGeoidalSepUnit = gpsmsg[puncLocation[i]+1];

  //if(toupper(gpsGeoidalSepUnit)== 'M')
  //{
  //  gpsAlt -= toFeet(gpsGeoidalSep);
  //}
  //else
  //{
  //  gpsAlt -= gpsGeoidalSep;
  //}
  //check below code for accuracy from code above
  gpsAlt -= gpsGeoidalSep * (1.0f + static_cast<float>((gpsGeoidalSepUnit&0x1)*0x4011F948));

  return std::make_tuple(hrs, min, sec, gpsLatLocation, latHemisphere, gpsLongLocation, LongHemisphere, gpsMeasureAccuracy, gpsAlt, numSat,hdop);
}

void GPS::update(PLANE* plane, int tries)//restructure to move oipening ttyS0 to setUp func and make a shutDown func too
{
  char buf[256];
  int n = read(fd, buf, sizeof(buf));
  if(n<0)
  {
    close(fd);
    bool resetup = configSerialPort();
    GPGGA = false;
    return;
  }
  const char* end = (const char*)memchr(buf, '\n', n);
  if (!end) 
  {
      GPGGA = false;
      return; // not a full sentence yet
  }
  int len = end - buf;
  std::string sentence(buf, len);
  update(plane, sentence);
  GPGGA = true;
}

void GPS::update(PLANE* plane)
{
  int tries = 0;
  while(tries<3)
    {
      update(plane, tries);
      if(GPGGA)
        break;
      else
        tries++;
    }
  SHUTDOWNERROR = !GPGGA;
}

void GPS::update(PLANE* plane, const std::string gpsmsg)
{
  this->punctuationMarker(gpsmsg);

  auto[hrs, min, sec, lat, hem1, Long, hem2, Acc, Alt, numSat, hdop] = parser(gpsmsg, puncLocation);

  plane->updateGPS(lat, Alt, Long, hem1, hem2, Acc, hrs, min, sec, numSat, hdop);
}

bool GPS::configSerialPort()
{
  fd = open("/dev/ttyS0", O_RDWR|O_NOCTTY|O_SYNC);
  
  speed_t baudRate=9600;

  termios tty{};
  //check tcgetaddr 
  if(tcgetattr(fd, &tty)!=0)
  {
    perror("error 1 from tcgetattr");
    close(fd);
    return false;
  }

  //set baud rate
  cfsetispeed(&tty, baudRate);

  //config bit rate etc
  tty.c_cflag &= ~PARENB; //no parity
  tty.c_cflag &= ~CSTOPB; //stop bit
  tty.c_cflag &= ~CSIZE; //check here and clear all bits
  tty.c_cflag |= CS8; //2 nibbles/cycle

  //flow ctrl
  tty.c_cflag &= ~CRTSCTS; //check here and no flow ctrl 

  //engage read & ignore
  tty.c_cflag |= CREAD | CLOCAL;

  //set raw data
  //no cname, echo etc
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  //more ctrl settings
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(ICRNL | INLCR);
  tty.c_oflag &= ~OPOST;

  if(tcsetattr(fd, TCSANOW, &tty)!=0)
  {
    perror("error 2 from tcsetattr");
    close(fd);
    return false;
  }
  close(fd);
  return true;
}
