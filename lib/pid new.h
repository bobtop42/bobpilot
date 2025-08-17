#ifndef PIDNEW_H
#define PIDNEW_H

class PID{
    private:
        float kp;
        float ki; 
        float kd;

        float target;

        float error = 0.0f;
        float cum_error = 0.0f;
        float prev_error = 0.0f;
    public:
    PID(float kp = 0.0f, float ki = 0.0f, float kd = 0.0f)
        : kp(kp), ki(ki), kd(kd) {};

    void setKp(float newKp) { kp = newKp; }
    void setKi(float newKi) { ki = newKi; }
    void setKd(float newKd) { kd = newKd; }

    void setTarget(float newTarget) {
        target = newTarget;
    }

    float claculate(float val, float target, float period){
        setTarget(target);
        calculate(val, period);
    }

    float calculate(float val, float period){
        error = target - val;
        cum_error += error;
        prev_error = error;
        return (kp * error) + (ki * cum_error)/period + (kd * (error - prev_error));
    }

    void reset() {
        error = 0.0f;
        cum_error = 0.0f;
        prev_error = 0.0f;
    }
    
};


#endif
