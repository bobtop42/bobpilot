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
        mpu.updatePA(&plane);
        gps.update(&plane);
        flightComputer.planeAngleFinder(&plane.ckalman, &plane, &plane.hmc);
        flightComputer.wayPointAngleFinder();
        roll.engaged();
        pitch.engaged();
        roll.targetRoll(&plane);
        pitch.targetPitch(&plane);
        logger.log(&plane);
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
  roll.targetRoll(&plane);
  pitch.targetPitch(&plane);
  atc.targetSpeed(plane);
}

void REAL::update()
{
  if(engaged_)
  {
    mpu.updatePA(&plane);
    kalman.loop(&plane);
    gps.update(&plane);
    
    flightComputer.planeAngleFinder(&plane.ckalman, &plane, &plane.hmc);
    flightComputer.wayPointAngleFinder();

    pitch.update(&plane);
    roll.update(&plane);
    //atc.update(plane);

    //motors.setSpeed(atc.speed);
    servos.updateServos(&plane);

    logger.log(&plane);
    
  }
}

void REAL::checkLoopType()
{
  
}

/*
void REAL::loop()
{
  if(isRoute)
  {
    while(!flightComputer.routeCompleted)
    {
      update();
    }
    if(flightComputer.routeCompleted)
    {
      isRoute = false;
      float Long = plane.loc.x;
      float Lat = plane.loc.z;
      float ln = longToFeet(Long, Lat);
      float lt = latToFeet(Lat);
      while(true)
        {
          if(!controller.interupt())
          {
            holdComputer.holdCircle(50.0f, 150, Lat, Long, 0x01, plane, roll, pitch, 10.0f);
          }
          else{break;}
        }
    }
  }
}
*/

void REAL::loop()
{
  if(isRoute)
  {
    engageAll();
    if(flightComputer.routeCompleted!=0x00)
    {
      while(flightComputer.routeCompleted!=0x00&&!SHUTDOWNERROR)
        {
          update();
          if(controller.checkInput(false))
          {
            break;
          }
        }
    };
    if(flightComputer.routeCompleted==0x00)
    {
      isRoute = false;
      secondaryLoop();
    }
  }
  else
  {
    if(controller.humanControl())
    {
      controller.loop();
    }
  }
}

void REAL::secondaryLoop()
{
  while(true)
    {
      if(!controller.interupt()||!controller.exitAutoPilot()||!SHUTDOWNERROR)
      {
        if(engaged_)
        {
          mpu.updatePA(&plane);
          kalman.loop(&plane);
          gps.update(&plane);

          holdComputer.holdCircle(50.0f, 150.0f, plane.loc.z, plane.loc.x, 0x01, &plane, &roll, &pitch, 10.0f);
          atc.update;

          motors.setSpeed(atc.speed);
          servos.updateServos(&plane);
          
          logger.log(&plane); 
          controller.update(false);
        }
      }
      else
      {
        disengaged();
        break;
      }
    }
}
