#include "gps.h"

void GPS::punctuationMarker(const std::string gpsmsg, int* puncLocation)
{
  int i = 0;
  int pc = 1; //punctuation counter

  while(puncLocation[0] != '$')
    {
      char ch = gpsmsg[i];
      if(ch == '$')
      {
        puncLocation[0] = ch;
      }
      ++i;
    }
  while(gpsmsg[i] != '\r')
    {
      char ch = gpsmsg[i];
      if(ispunct(ch))
      {
        puncLocation[pc] = ch;
      }
      ++i;
    }
}

auto GPS::parser(const std::string gpsmsg, int* puncLocation)
{
  //UTC itme
  //hours
  int hrs = std::stoi(gpsmsg.substr(puncLocation[1]+1, 2));
  //minutes
  int min = std::stoi(gpsmsg.substr(puncLocation[1]+3, 2));
  //seconds
  int sec = std::stoi(gpsmsg.substr(puncLocation[1]+5, 2));

  //lat in deg format DDMM.MMMM
  float lat = std::stoi(gpsmsg.substr(puncLocation[3]+1, 2));

  float latMin = floatPositionReturn(gpsmsg, puncLocation, 4, 2);
  float gpsLatLocation = lat + latMin/60.0f;

  const char latHemisphere = charPositionReturn(gpsmsg, puncLocation, 5);

  //long in deg in DDDMM.MMMM format
  float Long = std::stoi(gpsmsg.substr(puncLocation[6]+1,3));

  float LongMin = floatPositionReturn(gpsmsg, puncLocation, 6, 3);

  float gpsLongLocation = Long + LongMin/60.0f;
  
  const char LongHemisphere = charPositionReturn(gpsmsg, puncLocation, 8);

  int gpsMeasureAccuracy = std::stoi(gpsmsg.substr(puncLocation[9]+1, 1));

  float gpsAlt = floatPositionReturn(gpsmsg, puncLocation, 13);

  const char gpsAltUnit = charPositionReturn(gpsmsg, puncLocation, 15);

  if(toupper(gpsAltUnit) == 'M')
  {
    gpsAlt = toFeet(gpsAlt);
  }

  float gpsGeoidalSep = floatPositionReturn(gpsmsg, puncLocation, 16);

  const char gpsGeoidalSepUnit = charPositionReturn(gpsmsg, puncLocation, 18);

  if(toupper(gpsGeoidalSepUnit)== 'M')
  {
    gpsAlt -= toFeet(gpsGeoidalSep);
  }
  else
  {
    gpsAlt -= gpsGeoidalSep;
  }

  return std::make_tuple(hrs, min, sec, gpsLatLocation, latHemisphere, gpsLongLocation, LongHemisphere, gpsMeasureAccuracy, gpsAlt);
}

int GPS::update(PLANE* plane, int* puncLocation)
{
  int fd = open ("/dec/ttyS0", O_RDWR|O_NOCTTY|O_SYNC);

  if(fd!=0)
  {
    close(fd);
    fd = open ("/dec/ttyS0", O_RDWR|O_NOCTTY|O_SYNC);
    if(fd!=0)
    {
      return -1;
    }
  }

  char buf[256];
  std::string sentence;

  int n = read(fd, buf, sizeof(buf);

  if(n>0)
  {
     for(auto i:buf)
      {
        char ch = buf[i];
        if(ch == '\n')
        {
           update(plane, sentence, puncLocation);
          sentence.clear();
        }
        else if(ch!='\n')
        {
          sentence += ch;
        }
      }
  }
  else
  {
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

void GPS::update(PLANE* plane, const std::string gpsmsg, int* puncLocation)
{
  this->punctuationMarker(gpsmsg, puncLocation);

  auto[hrs, min, sec, lat, hem1, Long, hem2, Acc, Alt];

  plane->updateGPS(lat, Alt, Long, hem1, hem2, Acc, hrs, min, sec);
}

bool GPS::configSerialPort(int fd, speed_t baudRate=9600)
{
  termios tty{};
  //check tcgetaddr 
  if(tcgetattr(fd, &tty)!=0)
  {
    perror("error 1 from tcgetattr");
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
    return false;
  }
  return true;
}
