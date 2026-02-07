#include "gps.h"
#include <tuple>

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
  if(gpsmsg.substr(puncLocation[0]+1,6) != "GPGGA")
  {
    GPGGA = true;
  }

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
  
  if(toupper(gpsAltUnit) == 'M')
  {
    gpsAlt = toFeet(gpsAlt);
  }

  float gpsGeoidalSep = std::stof(gpsmsg.substr(puncLocation[i], 1+puncLocation[i+1]-puncLocation[i]));

 char gpsGeoidalSepUnit = gpsmsg[puncLocation[i]+1];
  
  if(toupper(gpsGeoidalSepUnit)== 'M')
  {
    gpsAlt -= toFeet(gpsGeoidalSep);
  }
  else
  {
    gpsAlt -= gpsGeoidalSep;
  }

  return std::make_tuple(hrs, min, sec, gpsLatLocation, latHemisphere, gpsLongLocation, LongHemisphere, gpsMeasureAccuracy, gpsAlt, numSat,hdop);
}

void GPS::update(PLANE* plane, int tries)
{
  int fd = open ("/dec/ttyS0", O_RDWR|O_NOCTTY|O_SYNC);

  if(fd==-1)
  {
    GPGGA = false;
  }
  else
  {
    char buf[256];
    std::string sentence="";
  
    int n = read(fd, buf, sizeof(buf));

    if(n>0)
    {
      for(int i=0; i<n; ++i)
        {
          char ch = buf[i];
          if(ch == '\n')
          {
            break;
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
      GPGGA = false;
    }
    update(plane, sentence);
    sentence.clear();
    GPGGA = true;
  }
  close(fd);
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
  if(!GPGGA)
    SHUTDOWNERROR = true;
}

void GPS::update(PLANE* plane, const std::string gpsmsg)
{
  this->punctuationMarker(gpsmsg);

  auto[hrs, min, sec, lat, hem1, Long, hem2, Acc, Alt, numSat, hdop] = parser(gpsmsg, puncLocation);

  plane->updateGPS(lat, Alt, Long, hem1, hem2, Acc, hrs, min, sec, numSat, hdop);
}

bool GPS::configSerialPort(int fd)
{
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
