#include <cmath>
#include <iostream>
#include <utility>
#include "Arduino.hpp"

#define PI 3.14159265358979

struct Point
{
    double x,y;
};

class Arm
{
    double l1,l2;

    public:
        Arm(double l1, double l2):
            l1(l1), l2(l2)
        {}

        auto angles(Point p)
        {
            double ph2= phi2(p);
            double phi1 = atan2(p.y,p.x)
                -atan2(l2*sin(ph2),
                       (l1+l2*cos(ph2)));
            return std::make_pair(phi1/PI*180, ph2/PI*180);
        }
    private:
        double phi2(Point p)
        {
            return acos(
                (p.x*p.x+p.y*p.y-l1*l1-l2*l2)/
                    (2*l1*l2)
                );
        }
};


void setup()
{
    Arm a(2,1);
    auto [phi1, phi2] = a.angles(Point{1,2});
    std::cout << phi1 << " " << phi2 << std::endl;
}

void loop()
{}
