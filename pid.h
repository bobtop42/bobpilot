#ifndef PID_H
#define PID_H

class PID
{
public:
    PID();
    void returnAdd();

    struct PIDadd
    {
        float add;
        float prev_add;

        void shift(PIDadd& myPIDadd);
    };

    struct PIDerror
    {
        float error;
        float prev_error;
        float prev_error2;

        void shift(PIDerror& myError, float newError);
    };
};

class VELOCITY : public PID
{
public:
    VELOCITY(float kp, float ki, float kd)
        : kp_(kp), ki_(ki), kd_(kd) {}

    PIDadd add;
    PIDerror error;

    void calc(VELOCITY *myVelocity, float newError);

private:
    float kp_;
    float ki_;
    float kd_;
};

class DESCRETE : public PID
{
public:
    DESCRETE(float kp, float ki, float kd)
        : kp_(kp), ki_(ki), kd_(kd) {}

  
    PIDadd add;
    PIDerror error;

    void calc(DESCRETE *myDescrete, float newError);

private:
    float kp_;
    float ki_;
    float kd_;
};

#endif
