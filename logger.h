// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef LOGGER_H
#define LOGGER_H
#include "MATHLIB.h"
#include "io.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <tuple>

class LOGGER
{
public:
void log(PLANE* plane);
void setFilename(std::string filetype);
auto fillPlaneData(const std::string data, int logPos[67], PLANE* plane);
auto parseLog(const std::string data, int logPos[67]);

LOGGER(std::string file_):file(file_){}

private:
std::string file;

};

#endif
