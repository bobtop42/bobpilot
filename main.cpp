#include "plane.h"
#include "realio.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <tuple>

int startUpInput()
{
  int mode;
  std::cout << "Choose bobpilot operating mode: \n
                Press 1 for real mode (realio) \n
                Press 2 for simulation mode (simio)\n
                Press 3 for replay mode (replayio)\n
                Press 4 to quit"
                << std::endl;
  std::cin >> mode;
  return mode;
}

auto setUp()
{
  bool isSetUp = false;
  //QUIT quit;
  while(!isSetUp || !SHUTDOWNERROR)
    {
      try
        {
          int mode = startUpInput();
          if(mode > 4 || mode < 1)
            throw mode;
          switch (mode)
            {
              case 1:
                {
                  REAL realio;
                  realio.setUp();
                  if(!realio.setUp_)
                    throw "realio";

                  auto* io = &realio;
                  bool* engaged = &realio.engaged_;
                  return std::make_tuple(io, engaged, mode);
                }
              case 2:
                {
                  SIM simio;
                  simio.setUp();
                  if(!simio.setUp_)
                    throw "simio";
                  auto* io = &simio;
                  bool* engaged = &simio.engaged_;
                  return std::make_tuple(io, engaged, mode);
                }
              case 3:
                {
                  REPLAY replayio;
                  replayio.setUp();
                  if(!replayio.setUp_)
                    throw "replayio";
                  auto* io = &replayio;
                  bool* engaged = &replayio.engaged_;
                  return std::make_tuple(io, engaged, mode);
                }
              case 4:
                {
                  char quitYorN;
                  std::cerr << "are you sure you want to quit? (y/n)." << std::endl;
                  std::cin >> quitYorN;
                  if(toupper(quitYorN) == 'Y') exit(0);
                  else throw "NOQUIT";
                  break;
                }
            }
        }
        catch(int modeError)
        {
          std::cerr << "ERROR: " << modeError << " is not a valid mode. \n Please try again." << std::endl;
          break;
        }
        catch(const char* msg)
        {
          std::cerr << "ERROR: " << msg << " failed to set up. \n please try again." << std::endl;
          break;
        }
    }
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
  auto[io, engaged, mode] = setUp();

  if(SHUTDOWNERROR)
    exit(0);

  bool notQuit = false;

  /*
  this next loop loops through all io w/ noQuit loop running. a secondary loop can exit out of an io of a the users choice allowing the user to switch b/t io and not exiting the program as a whole. 'setUp''s  "engaged" keeps the current io running until the user decides to exit out of the io, or quit the program as a whole. everytime a user exits out of a io, the program will loop through the setUp() function again, allowing a io swap to occur, and letting the user exit altogether if they choose to do so.
  */

  //basic program loop function
  while(!notQuit)
    {
      switch(mode)
        {
          case 1:
            {
              while(*engaged)
                {
                  io->update();
                }
              break;
            }
          case 2:
            {
              while(*engaged)
                {
                  io->update();
                }
              break;
            }
          case 3:
            {
              while(*engaged)
                {
                  io->update();
                }
              break;
            }
        }
      //add way to not do cmdSetUp() based on mode and a way to reverse it befor the program terminates
      auto [io, engaged, mode] = setUp();
    }
};
