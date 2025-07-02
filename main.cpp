#include <iostream>
#include "ap/atc/atc.h"
#include "io/io.h"
using namespace std;

int main(){

    io IO = simIO();

    while(IO.isRunning()) {
        IO.update();
        IO.printStatus();
    }
    // cout << "BobPilot is running!" << endl;

    return 0;
}