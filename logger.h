#ifndef LOGGER_H
#define LOGGER_H
#include "plane.h"
#include "io.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include<ctime>
#include<iomanip>
#include <tuple>

class LOGGER
{
public:
void log(PLANE* plane, std::string filename, int num);
void setFilename(std::string filetype);
auto fillPlaneData(const std::string data, int logPos[67], PLANE* plane);
auto parseLog(const std::string data, int logPos[67]);

LOGGER(std::string file_):file(file_){}

private:
std::string file;

};

#endif
