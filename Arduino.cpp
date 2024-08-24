#include "Serial.hpp"
#include "CWM/CWM.h"
#include "CWM/Conscreen/Conscreen_console.h"
#include "Modules/Modules.hpp"
#include "CWM/AM.h"
#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <unistd.h>
#include <chrono>

AM app_manager;


static auto start_time = std::chrono::steady_clock::now();

long millis()
{
    auto now = std::chrono::steady_clock::now();
    return  std::chrono::duration_cast<std::chrono::milliseconds>(now-start_time).count();
}


static uint8_t pins[30]= {0};
static bool pinModes[30]= {0};

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
    if(pinModes[pin]!=OUTPUT)
        pins[pin]=value;
}
void pinMode(int pin, int mode){
    pinModes[pin]=mode;
}


void runArduino(int freq)
{
    setup();
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1)/freq);
        loop();
    }
}

void runEmu(std::vector<std::shared_ptr<Module>> modules)
{
    while(true){
        // update modules
        for(auto &m : modules)
            m->update();
        char inp = Conscreen_console_get_key();
        if(inp!=0) AM_key_event(app_manager, inp);
        CWM_render();
    }
}

int main(void)
{
    // init AM
    CWM_init();
    app_manager = AM_create();

    // Spawn Serial_console
	App serialApp = AM_app_create("Serial Console", SerialCallback);
    AM_register(app_manager, serialApp);

    // Launch Emulator Thread
    std::thread emulator(runEmu, setupEmulator());
    emulator.detach();

    // run ino Code
    while(true){
        runArduino(1000000);
    }
}

std::vector<std::shared_ptr<Module>> setupEmulator()
{
        auto me = std::make_shared<MotorEmu>(1,2,3);
        return {me,
           std::make_shared<PhotoEmu>(4, me)};
}
