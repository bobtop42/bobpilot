#ifndef GPS_H
#define GPS_H
#include "realio.h"
#include "plane.h"
#include "MATHLIB.h"
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <termios.h>

class GPS
{
public:
void update(PLANE* plane, const std::string gpsmsg);
void update(PLANE* plane);
bool configSerialPort(int fd, speed_t baudRate=9600);

private:
int punctuationMarker(const std::string gpsmsg);
auto parser(const std::string gpsmsg, int puncLocation[67]);
bool gpsERRORepen = false;
bool gpsERRORread = false;
};

#endif
