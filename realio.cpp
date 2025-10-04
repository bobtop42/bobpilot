#include "realio.h"

void REAL::setUp()
{
  while(setUpCounter <4)
    {
      try
        {
          if(setUpCounter != 0)
          {
            std::cerr << "retrying setup..." << std::endl;
          }

          int mpuCheck = mpu.setUp();

          if(mpuCheck != 0)
            throw "ERROR: MPU6050 failed to set up";

          int setUpFd = open("/dev/ttyS0", O_RDWR|O_NOCTTY|O_SYNC);
          if(setUpFd != 0)
            throw "ERROR: failed to open fd to set up GPS";
          
          bool gpsCheck = gps.configSerialPort(setUpFd);
          
          if(!gpsCheck)
            throw "ERROR: GPS failed to set up";

          logger.setFilename(".txt");

          //work on M2H(motors) set up sequence later
          //uint16_t setUpM2H = motors.setUp();
          //int motorSetUp = motors.command()

          setUp_ = true;

        }
      catch(const char* msg)
      {
        std::cerr << msg << std::endl;
        setUpCounter++;
      }
      catch(...)
      {
        SHUTDOWNERROR = true;
      }
      if (setUp_)
      {
        setUpCounter = 0;
        mpu.updatePA(plane);
        gps.update(plane);
        flightComputer.planeAngleFinder(plane->ckalman, plane, plane->hmc);
        flightComputer.wayPointAngleFinder();
        roll.engaged();
        pitch.engaged();
        roll.targetRoll(plane);
        pitch.targetPitch(plane);
        logger.log(plane);
      }
    }
}

void REAL::engaged()
{
  engaged_ = true;
}

void REAL::disengaged()
{
  engaged_ = false;
}

void REAL::engageAll()
{
  roll.engaged();
  pitch.engaged();
  atc.engaged();
  engaged();
}

void REAL::disengageAll()
{
  roll.disengaged();
  pitch.disengaged();
  atc.disengaged();
  disengaged();
}

void REAL::targetAll()
{
  roll.targetRoll(plane);
  pitch.targetPitch(plane);
  atc.targetSpeed(plane);
}

void REAL::update()
{
  if(engaged_)
  {
    mpu.updatePA(plane);
    kalman.loop(plane->X, plane->P, plane->K, plane->Y, plane);
    gps.update(plane);
    
    flightComputer.planeAngleFinder(plane->ckalman, plane, plane->hmc);
    flightComputer.wayPointAngleFinder();

    pitch.update(plane);
    roll.update(plane);
    //atc.update(plane);

    //motors.setSpeed(atc.speed);
    servos.updateServos(plane);

    logger.log(plane);
    
  }
}

void REAL::loop()
{
  //put in hold pos/reg loop switches etc
}
