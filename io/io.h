#ifndef IO_H
#define IO_H
#include "ap/atc/atc.h"

class io {
    public:
        ATC atc;
        io();
        virtual bool isRunning();
        virtual void update();
        virtual void printStatus();

    private:
        bool running;
};
class simIO : public io {
    public:
        simIO();
        
        bool isRunning();
        void update();
        void printStatus();

};

#endif