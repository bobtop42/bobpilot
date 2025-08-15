#include "pid.h"

// PID base
PID::PID() {}
void PID::returnAdd() {}

// PID::PIDadd
void PID::PIDadd::shift(PIDadd& myPIDadd)
{
    prev_add = add;
    add = 0.0f;
}

// PID::PIDerror
void PID::PIDerror::shift(PIDerror& myError, float newError)
{
    prev_error2 = prev_error;
    prev_error = error;
    error = newError;
}

// VELOCITY::Velocity
void VELOCITY::calc(VELOCITY *myVelocity, float newError)
{
    error.shift(error, newError);

    float KP = myVelocity->kp_ * (error.error - error.prev_error);
    float KI = myVelocity->ki_ * error.error * 0.1f;
    float KD = myVelocity->kd_ * (
        (error.error - 2.0f * error.prev_error + error.prev_error2) / 0.1f
    );

    this->add.add = KP + KI + KD;
}

// DESCRETE::Descrete
void DESCRETE::calc(DESCRETE *myDescrete, float newError)
{
    myDescrete->error.shift(myDescrete->error, newError);

    float KP = kp_ * (error.error - error.prev_error);
    float KI = ki_ * error.error * 0.1f;
    float KD = kd_ * (
        (error.error - 2.0f * error.prev_error + error.prev_error2) / 0.1f
    );

    this->add.add = KP + KI + KD;
}
