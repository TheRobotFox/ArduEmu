#include <bits/chrono.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ctime>


auto start_time = std::chrono::steady_clock::now();

long millis()
{
        auto now = std::chrono::steady_clock::now();
        return  std::chrono::duration_cast<std::chrono::nanoseconds>(now-start_time).count()/1000000;
}

class Serial{
public:
        static void begin(int _)
        {}
        template<typename ...Tp>
        static void print(Tp... args)
        {
                ((std::cout << args), ...);
        }
        template<typename ...Tp>
        static void println(Tp... args)
        {
            Serial::print(args...);
            std::cout << std::endl;
        }

};

Serial Serial;

uint8_t pins[30]= {0};

void analogWrite(int pin, uint8_t value){
        pins[pin]=value;
}

uint8_t analogRead(int pin){
        return pins[pin];
}

bool digitalRead(int pin){
        return pins[pin];
}

void digitalWrite(int pin, bool value){
        pins[pin]=value;
}
#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1
#define A0 4
void pinMode(int pin, int mode){}

class MotorEmu{
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
        void update(){
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

class PhotoEmu
{
        MotorEmu &m;
        int pin;
public:
        PhotoEmu(int pin, MotorEmu &m)
        : m(m), pin(pin)
        {}

        void update()
        {
                // Serial.println(m.getRot(), " : ", std::abs(std::sin(m.getRot()))>0.9);
                analogWrite(pin,(std::abs(std::sin(m.getRot()))>0.8)*25);
        }
};

void setup();
void loop();



int main(void){
        MotorEmu me(1,2,3);
        PhotoEmu ph(4, me);
        digitalWrite(2, 1);
        digitalWrite(3, 0);
        analogWrite(1, 100);

        setup();
        while(true){
                me.update();
                ph.update();

                if((int)millis()%50==0){
                        Serial.print((int)millis(), ": ");
                        Serial.print(me.getRPM(), " ");
                }
                loop();
        }
}
