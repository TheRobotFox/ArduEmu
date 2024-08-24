#include "Serial.hpp"

// Global Serial Object
decltype(Serial) Serial;

void Serial::begin(int _)
{init=true;}

void Serial::wait_for(std::unique_lock<std::mutex> &lk, std::function<bool()> pred)
{
    while(!pred() && read_cond.wait_for(lk,
                                        std::chrono::milliseconds(timeout_ms))!=std::cv_status::timeout);
}

template<typename ...Tp>
void Serial::print(Tp... args)
{
    out_mtx.lock();
    ((stdout << args), ...);
    out_mtx.unlock();
}
template<typename ...Tp>
void Serial::println(Tp... args)
{
    out_mtx.lock();
    ((stdout << args), ...);
    stdout << std::endl;
    out_mtx.unlock();
}

int Serial::available_impl()
{
    std::streampos currentPos = stdin.tellg();
    stdin.seekg(0, std::ios::end);
    int available = stdin.tellg()-currentPos;

    stdin.seekg(currentPos, std::ios::beg);

    return available;
}
int Serial::available()
{
    in_mtx.lock();
    int res = this->available_impl();
    in_mtx.unlock();
    return res;
}
int Serial::read()
{
    int res = stdin.get();
    if(res==std::char_traits<char>::eof()) return -1;
    return res;
}
int Serial::readBytes(char *buffer, int max)
{
    std::unique_lock<std::mutex> lk(in_mtx);
    this->wait_for(lk, [this, max]{return this->available_impl()>=max;});
    int bytes_read = stdin.read(buffer, max).gcount();
    lk.unlock();
    return  bytes_read;
}
String Serial::readString()
{
    String s;
    std::unique_lock<std::mutex> lk(in_mtx);
    this->wait_for(lk, [this]{return this->available_impl();});
    stdin >> s;
    lk.unlock();
    return s;
}
int Serial::parseInt()
{
    int i;
    std::unique_lock<std::mutex> lk(in_mtx);
    this->wait_for(lk, [this]{return this->available_impl()>=sizeof(int);});
    stdin >> i;
    lk.unlock();
    return i;
}
float Serial::parseFloat()
{
    float f;
    std::unique_lock<std::mutex> lk(in_mtx);
    this->wait_for(lk, [this]{return this->available_impl()>=sizeof(float);});
    stdin >> f;
    lk.unlock();
    return f;
}

void SerialCallback(App self, char key, void *_)
{
    Serial.stdin << key;
}
