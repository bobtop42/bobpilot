#ifndef ATC_H//auto throttle
#define ATC_H

class ATC {
    public:
        float targetSpeed;
        float currentSpeed;
        float throttle;
        bool engaged;
        const float Kp = 0.1; // look up pid control

        ATC();

        void engage(float target);
        void disengage();
        void setTarget(float value);
        void update();
};

#endif