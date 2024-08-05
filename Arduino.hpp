#pragma once
#include <cstdint>
#include <vector>
#include "CWM/CWM.h"
#include <string>


//Arduino
#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1
#define A0 4
using String = std::string;

long millis();

void analogWrite(int pin, uint8_t value);
uint8_t analogRead(int pin);
bool digitalRead(int pin);
void digitalWrite(int pin, bool value);
void pinMode(int pin, int mode);

void setup();
void loop();

class Serial;
extern Serial Serial;


// Emulators
class Module
{
        public:
                const std::string name="No Name";
                virtual void update()=0;
                virtual std::vector<std::pair<std::string, std::string>> getAttributes(){ return {};}
                virtual std::vector<CWM_window> getWindows(){ return {};}
};

std::vector<std::shared_ptr<Module>> setupEmulator();
