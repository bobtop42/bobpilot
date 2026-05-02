#include "MATHLIB.h"
#include "realio.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <tuple>
#include <variant>



int startUpInput()
{
  int mode;
  std::cout << "Choose bobpilot operating mode: \n Press 1 for real mode (realio)\n Press 4 to quit"
  << std::endl;
  std::cin >> mode;
  return mode;
}

int cmdSetUp()
{
  int noConsole = system("sudo raspi_config");
  int noConsoleBoot = system("sudo nano /boot/commandline.txt");
  int enableUART = system("sudo nano /boot/config.txt");
  int checkSerialDevice = system("sudo ls /dev/ttyS0 /dev/ttyS0 /dev/ttyAMA0");
  int enabled1 = system("sudo usermod -a -G dialout $USER");

  if(noConsole==0 || noConsoleBoot==0 || enableUART==0 || checkSerialDevice==0 || enabled1==0)
    return 0;
  else
    return -1;
}

int main(int argc, char**argv)
{
  std::cout << "Launching bobpilot..." << std::endl;
  int cmdSetUpCheck = cmdSetUp();
  if(cmdSetUpCheck==-1)
  {
    std::cout<<"ERROR: cmdSetUp() failed. \n please try again." << std::endl;
    SHUTDOWNERROR = true;
    return -1;
  }

  bool notQuit = false;


  /*
  this next loop loops through all io w/ noQuit loop running. a secondary loop can exit out of an io of a the users choice allowing the user to switch b/t io and not exiting the program as a whole. 'setUp''s  "engaged" keeps the current io running until the user decides to exit out of the io, or quit the program as a whole. everytime a user exits out of a io, the program will loop through the setUp() function again, allowing a io swap to occur, and letting the user exit altogether if they choose to do so.
  */

  //basic program loop function
  REAL *realio = nullptr;
  while(!notQuit)
    {
      int mode = startUpInput();
      switch(mode)
        {
          case 1:
            {
              realio = new REAL;
              realio->setUp();
              if(!realio->setUp_)
                std::cout<<"ERROR: realio failed to start up. \n please try again." << std::endl;
              while(realio->engaged_)
                {
                  realio->update();
                }
              delete realio;
              break;
            }/*
          case 2:
            {
              SIM *simio = new SIM;
              simio->setUp();
              if(!simio->setUp_)
                std::cout<<"ERROR: simio failed to start up. \n please try again." << std::endl;
              while(simio->engaged_)
                {
                  simio->update();
                }
              delete simio;
              break;
            }
          case 3:
            {
              REPLAY *replayio = new REPLAY;
              replayio->setUp();
              if(!replayio->setUp_)
                std::cout<<"ERROR: replayio failed to start up. \n please try again." << std::endl;
              while(replayio->engaged_)
                {
                  replayio->update();
                }
              delete replayio;
              break;
            }*/
          case 4:
            {
        std::cout<<"EXITING BOBPILOT...\exiting now"<<std::endl;
        return 0;
            }
        }
    }
  return 0;
};
