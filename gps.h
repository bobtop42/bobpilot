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
void punctuationMarker(const std::string gpsmsg);
auto parser(const std::string gpsmsg, int puncLocation[67])-> std::tuple<int,int,int,float,char,float,char,int,float>;

int puncLocation[67];
bool gpsERRORepen = false;
bool gpsERRORread = false;
};

#endif
