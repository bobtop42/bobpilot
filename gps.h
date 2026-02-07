#ifndef GPS_H
#define GPS_H
#include "MATHLIB.h"
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <termios.h>
#include <tuple>


class GPS
{
public:
void update(PLANE* plane, const std::string gpsmsg);
void update(PLANE* plane);
bool configSerialPort(int fd);

GPS();

private:
void update(PLANE* plane, int tries);
void punctuationMarker(const std::string gpsmsg);
auto parser(const std::string gpsmsg, int puncLocation[67]);

int puncLocation[67];
bool gpsERRORepen = false;
bool gpsERRORread = false;
bool GPGGA = true;
};

#endif
