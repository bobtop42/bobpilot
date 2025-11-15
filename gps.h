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
void update(PLANE* plane, const std::string gpsmsg, int* puncLocation);
void update(PLANE* plane, int* puncLocation);
bool configSerialPort(int fd);

GPS();

private:
void punctuationMarker(const std::string gpsmsg, int* puncLocation);
auto parser(const std::string gpsmsg, int puncLocation[67]);
int puncLocation[67];
bool gpsERRORepen = false;
bool gpsERRORread = false;
};

#endif
