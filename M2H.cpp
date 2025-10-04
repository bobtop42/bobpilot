#include "M2H.h"

//using namespace M2H;
  
void M2H::M2H::setAllSpeeds(uint16_t speed, uint8_t* command)
{
  command[0] = 0xE1;
  command[1] = speed & 0x7F;
  command[2] = (speed >> 7) & 0x7F;
};

void M2H::M2H::setBrake(uint16_t brake, int num, uint8_t* command)
{
  command[0] = 0xB1;
  command[1] = (0x01<<num)>>1;
  command[2] = brake & 0x7F;
  command[3] = (brake >> 7) & 0x7F;
}

void M2H::M2H::crcOn(uint8_t* command)
{
  command[0] = 0x8B;
  command[1] = 0x04;
  command[2] = 0xFB;
}

void M2H::M2H::crcOff(uint8_t* command)
{
  command[0] = 0x8B;
  command[1] = 0x00;
  command[2] = 0xFF;
}

void M2H::M2H::clearLatchedFlags(uint16_t flags, uint8_t* command)
{
  command[0] = 0xA9;
  command[1] = ~(flags & 0x3F);
  command[2] = ~((flags>>8) & 0x02);
}

void M2H::M2H::setLatchedFlags(uint16_t flags, uint8_t* command)
{
  command[0] = 0xA9;
  command[1] = flags & 0x3F;
  command[2] = (flags>>8) & 0x02;
}

void M2H::M2H::setCmdTimeout(uint8_t seconds, uint8_t* command)
{
  seconds*=250;
  command[0] = 0x9C;
  command[1] = 0x00;
  command[2] = 0x05;
  command[3] = seconds & 0xFF;
  command[4] = (seconds>>8) & 0xFF;
}

void M2H::M2H::maxForwardAccel(uint16_t speed, int num, uint8_t* command)
{
  command[0] = 0x9C;
  command[1] = (0x01<<num)>>1;
  command[2] = 0x0A;
  command[3] = speed & 0xFF;
  command[4] = (speed>>8) & 0xFF;
}

void M2H::M2H::maxForwardDeAccel(uint16_t speed, int num, uint8_t* command)
{
  command[0] = 0x9C;
  command[1] = (0x01<<num)>>1;
  command[2] = 0x0E;
  command[3] = speed & 0xFF;
  command[4] = (speed>>8) & 0xFF;
}

void M2H::M2H::startForwardSpeed(uint16_t start, int num, uint8_t* command)
{
  command[0] = 0x9C;
  command[1] = (0x01<<num)>>1;
  command[2] = 0x12;
  command[3] = start & 0xFF;
  command[4] = (start>>8) & 0xFF;
}

//NOTE:check command[3] later
void M2H::M2H::errorResponse(uint8_t error, uint8_t* command)
{
  command[0] = 0x9C;
  command[1] = 0x00;
  command[2] = 0x07;
  command[3] = error & 0x7F;
}

void M2H::M2H::setSpeed(uint16_t speed, uint8_t motor, uint8_t* command)
{
  command[0] = 0xD1;
  command[1] = motor & 0x03;
  command[2] = speed & 0x7F;
  command[3] = (speed>>7) & 0x7F;
}

uint8_t M2H::M2H::crcAppend(uint8_t * msg, uint8_t len)
{
  uint8_t crc = 0;
  for(uint8_t i=0; i<len; ++i)
    {
      crc ^= msg[i];
      for(uint8_t j=0; j<8; ++j)
        {
          if(crc & 1){crc^=0x91;}
          crc >>= 1;
        }
    }
  return crc;
}

int M2H::M2H::Write(uint8_t * msg, uint8_t& len, uint8_t motor, bool& crc)
//int Write(uint8_t * msg, uint8_t& len, uint8_t motor)
{
  
  std::string dev = "/dev/i2c-1";
  const uint M2H_ADDR = 0x10;
  //bool& crc = *CRC;// fix later
  int addlen = 0;
  if(crc) { addlen++;}
  
  
  uint8_t command[len+addlen];
  

  int fd = open("/dev/i2c-1", O_RDWR);
  if(motor!=0x03){motor = 0x01;}

  ioctl(fd, I2C_SLAVE, M2H_ADDR);
  uint8_t crcAppended;

  try
  {
    if(fd!=0) throw -1;
    
    if(crc)
    {
      crcAppended = crcAppend(msg, len);

      len+=1;
      for(uint8_t i=0; i<len; ++i)
        {
          command[i] = msg[i];
        }
      command[len+1] = crc;

      switch(motor)
        {
          case 0x01:
            {
              if (write(fd, command, len) != 0) throw -10; //FIX LATER
            }
          case 0x03:
            {
              command[1] = 0x01;
              crcAppended = crcAppend(command, len);
              if(write(fd, command, len) != 0) throw -11; //FIX LATER
              command[1]<<=1;
              crcAppended = crcAppend(command, len);
              if(write(fd, command, len) != 0) throw -12; //FIX LATER
            }
        }
      close(fd);
      return 0;
    }
    if(!crc)
    {

      uint8_t command[len];
      for(uint8_t i=0; i<len; ++i)
        {
          command[i] = msg[i];
        }

      switch(motor)
        {
          case 0x01:
            {
              if (write(fd, command, len) != 0) throw -13; //FIX LATER
            }
          case 0x03:
            {
              uint8_t command[len];
              for(uint8_t i=0; i<len; ++i)
                {
                  command[i] = msg[i];
                }

              command[1] = 0x01;
              if(write(fd, command, len) != 0) throw -14; //FIX LATER
              command[1]<<=1;
              if(write(fd, command, len) != 0) throw -15;
            }
        }
      close(fd);
      return 0;
    }
  }

  catch(int error)
    {
      /*note sto self:add code to check flags, find if any error, and then clear etc. then have it ready for code below*/
      switch(error)
        {
          case -1:
            {
              close(fd);
              break;
            }
          case -10:
            {
              usleep(500);
              if(write(fd, command, len) != 0)
              close(fd);
              return 0;
            }
          case -11:
            {
              usleep(500);
              if(write(fd, command, len) != 0) {close(fd); return -1;}
              else
              {
                command[1]<<=1;
                crcAppended = crcAppend(command, len);;
                if(write(fd, command, len) != 0) {close(fd); return -1;}
                else {close(fd); return 0;}
              }
            }
          case -12:
            {
              usleep(500);
              crcAppended = crcAppend(command, len);
              if(write(fd, command, len) != 0) {close(fd); return -1;}
              else {close(fd); return 0;}
            }
          case -13:
            {
              usleep(500);
              if(write(fd, command, len) != 0) {close(fd); return -1;}
              else {close(fd); return 0;}
            }
          case -14:
            {
              usleep(500);
              if(write(fd, command, len) != 0) {close(fd); return -1;}
              else 
              {
                command[1]<<=1;
                if(write(fd, command, len) != 0) {close(fd); return -1;}
                else {close(fd); return 0;}
              }
            }
          case -15:
            {
              usleep(500);
              if(write(fd, command, len) != 0) {close(fd); return -1;}
              else{close(fd); return 0;}
            }
        }
    }

  catch(...)
  {
    close(fd); 
    return -100;
  }
} // NOTE:make sure everything returns for every path

int M2H::M2H::command(uint16_t command, uint16_t value, uint8_t motor)
{
  /*when putting int a value for command and the command = 0x9c, put in the offset. if addtional command are added, make sure there is no overlappin gb/t the offset and command val. if you want to use a "general" command set motor to 0x00, or if you are trying to use a feature and want to set both motors at one , assign motot to 0x03. if that command has natiuve support "setAll" feature use this command insteead. I if you use a antive set-all command, set motor to 0x00, or it will not work. If you want to clear latched flags set motor 0, to clear latched flags set motot to 1 */
  int r;
  uint8_t cl;
  if(command == 0x8B || command ==  0xA9 || command == 0xE1 ) { cl = 3;}
  else if (command == 0xB1 || command ==  0x07 || command == 0xD1) { cl = 4;}
  else {cl = 5;}
  int Return;
  int Try = 0;
  //bool crc = *CRC; //fix later
  uint8_t cmd[cl];
  //check/fix command all values above here
  while(Try<4)
    {
      try
        {
          if ((command == 0x8B) && CRC)
          {
            crcOn(cmd);
            r = Write(cmd, cl, motor, CRC);
            break;
          }
          else if(command==0x8B && !CRC)
          {
            crcOff(cmd); 
            r = Write(cmd, cl, motor, CRC);
            break;
          }
          if(command==0x8B)
          {
            if(r==0){return 0;}
          }
          else if(r!=-100)
          {
            Try++;
            throw Try;
          }
          else
          {
            return -100;
          }
          
          switch(command)
            {
              case 0xE1:
                {
                  setAllSpeeds(value, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0xD1:
                {
                  setSpeed(value, motor, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0x16:
                {
                  startForwardSpeed(value, motor, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0x0A:
                {
                  maxForwardAccel(value, motor, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0x0E:
                {
                  maxForwardDeAccel(value, motor, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0x05:
                {
                  uint8_t time = value & 0xFF;
                  setCmdTimeout(time, cmd);
                  break;
                }
              case 0xA9:
                {
                  if(motor==0x00)
                  {
                    clearLatchedFlags(value, cmd);
                    break;
                  }
                  else if(motor==0x01)
                  {
                    setLatchedFlags(value, cmd);
                    break;
                  }
                  else
                  {
                    Try++;
                    throw Try;
                  }
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0xB1:
                {
                  setBrake(value, motor, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
              case 0x07:
                {
                  uint8_t error = value & 0xFF;
                  errorResponse(error, cmd);
                  r = Write(cmd, cl, motor, CRC);
                  break;
                }
            }
          if(r==0){return 0;}
          else if(r==-1)
          {
            Try++;
            throw Try;
          }
          else if(r==-100)
          {
            return -100;
          }
        }
        catch(int error)
        {
          if(error==4)
          {
            std::cerr<<"command failed"<<std::endl;
            return -1;
          }
          else
          {
            std::cerr<<"command failed. try:"<<error<<"/4"<<std::endl;
          }
        }
      catch(...)
        {
          return -100;
        }
    }
}

uint16_t M2H::M2H::setUp()
{
  uint16_t flag;
  flag = command(0x8B, 0x00,0x00);
  flag |= command(0xA9, 0x00, 0x00);
  flag |= command(0x05, 0x20, 0x00);
  flag |= command(0x0A, 0x0C80, 0x03);
  flag |= command(0x0E, 0x0C80, 0x03);
  flag |= command(0x16, 0xC8, 0x03);
  return flag;
}

void M2H::M2H::setSpeed(uint16_t speed)
{
  int check;
  speed*=8;
  speed&=0x0320;
  while(check!=0 || check!=-100)
    {
      check = command(0xD1, speed, 0x03);
    }
    if(check==-100)
    {
      //SHUTDOWNERROR will be in main
      SHUTDOWNERROR = true;
      return;
    }
}
