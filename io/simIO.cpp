#include "io.h"
#include <iostream>

bool io::isRunning() {
    return running;
}
void io::printStatus() {
    std::cout << "default IO in use!" << std::endl;
}
void io::update(){
    atc.update();
}


void simIO::printStatus() {
    std::cout << "sim IO in use, throttle cmd: " << atc.throttle << "(" << atc.engaged ? "auto" : "manual" << ")" 
    << std::endl;
}