#include <array>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <fstream>
#include <future>
#include <iostream>
#include <ctime>
#include <istream>
#include <memory>
#include <mutex>
#include <ostream>
#include <streambuf>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "CWM/AM.h"
#include "Arduino.hpp"

template<size_t SIZE=(2<<16)>
class WindowStream : public std::streambuf
{
    std::array<char, SIZE> buffer;
public:
    WindowStream()
    {
    }

protected:
    int_type overflow(int_type c = traits_type::eof()) override
    {
        int bytes_read = gptr()-buffer.begin();
        if(!bytes_read==0) return traits_type::eof();

        int bytes_unread = pptr()-gptr();
        memmove(buffer.begin(), gptr(), bytes_unread);
        setp(buffer.begin(), buffer.begin()+bytes_unread, buffer.end());
        setg(buffer.begin(), buffer.begin(), pptr());
        if(c!=traits_type::eof())
            return sputc(c);
        return c;
    }

};



// efficient stdout
// ringbuffer -> memcpy to text renderer

// stdin
// single line window with text renderer -> SerialCallback -> on enter submit
// auch ringbuffer maybe mit blocking if full


// ==> Thread safe ringbuffer
// thread safe copy last n bytes
// thread safe insert ring

// Thread safe ringbuffer with read head for stdin

void SerialCallback(App self, char key, void *_);
class Serial{
    friend void SerialCallback(App self, char key, void *_);
    std::spanstream stdin, stdout;
    std::mutex in_mtx, out_mtx;
    std::condition_variable read_cond;
    bool init=false;
    int timeout_ms = 1000;
    int available_impl();
    void wait_for(std::unique_lock<std::mutex> &lk, std::function<bool()> pred);
    bool dirty=true;
public:
    void setTimeout(int ms);
    int available();
    int read();
    int readBytes(char *buffer, int max);
    String readString();
    int parseInt();
    float parseFloat();
    void begin(int _);
    template<typename ...Tp>
    void print(Tp... args);
    template<typename ...Tp>
    void println(Tp... args);
};
