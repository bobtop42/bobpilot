#pragma once

#include "MATHLIB.h"
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <chrono>
#include <thread>

class GPS
{
public:
    bool goodRead = true;

    void openSerialFD();
    bool configureSerialPort();

    void markPunctuationPositions(const std::string gpsmsg);
    auto parseGPGGA(const std::string gpsmsg, int puncLocation[67]);

    void processGPSMessage(PLANE* plane, const std::string gpsmsg);
    void readAndParseGPS(PLANE* plane, int tries);
    void refreshGPSData(PLANE* plane);

    float gpsAltitude;
    int fd;

    int puncLocation[67];
    bool gpsERRORepen = false;
    bool gpsERRORread = false;
    bool GPGGA = true;
};
