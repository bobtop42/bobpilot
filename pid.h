#ifndef PID_H
#define PID_H

class PID
{
private:
float kp_;
float ki_;
float kd_;
int period = 0;
float target;

float temp;
float error = 0.0f;
float cum_error = 0.0f;
float prev_error = 0.0f;

public:

float calculate(float value)
{
    cum_error += error = value;
    period++;
    return (kp_ * error) + (ki_ * cum_error/period) + (kd_ * (error - prev_error));
    
}
PID(float KP, float KI, float KD): kp_(KP), ki_(KI), kd_(KD) {};

};

#endif
