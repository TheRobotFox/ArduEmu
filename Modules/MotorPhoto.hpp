#include "../Arduino.hpp"

class MotorEmu : public Module {
        int enable,a,b;
        double rot=0;
        double vel=0;
        double accel=0.5;
        double deccel=1.00000001;
        double last_millis=0;
public:
        MotorEmu(int enable, int a, int b)
        : enable(enable), a(a), b(b)
        {}
        void update() override {
                double time = millis(),
                       delta = time-last_millis;
                last_millis = time;

                int direction = digitalRead(a)-digitalRead(b);

                vel+=accel*sqrt(analogRead(enable))*direction*delta/1000.0;

                vel-=deccel*vel*delta/1000.0;
                rot+=vel*delta/1000.0;
        }
        double getRot(){
                return rot;
        }

        float getVel(){
                return vel;
        }

        float getRPM(){
                return vel*60.0/2.0/3.141;
        }

};

class PhotoEmu : public Module
{
        std::shared_ptr<MotorEmu> m;
        int pin;
public:
        PhotoEmu(int pin, std::shared_ptr<MotorEmu> m)
        : m(m), pin(pin)
        {}

        void update() override
        {
                // Serial.println(m.getRot(), " : ", std::abs(std::sin(m.getRot()))>0.9);
                analogWrite(pin,(std::abs(std::sin(m->getRot()))>0.8)*25);
        }
};
