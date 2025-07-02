#include "ATC.h"

ATC::ATC() : targetSpeed(0.0f), currentSpeed(0.0f), throttle(0.0f), engaged(false) {}

void ATC::engage(float target) {
    targetSpeed = target;
    engaged = true;
}
void ATC::disengage() {
    engaged = false;
}
void ATC::setTarget(float value) {
    targetSpeed = value;
}
void ATC::update(){
    if(engaged){
        throttle = (currentSpeed-targetSpeed) * Kp;
    }
}