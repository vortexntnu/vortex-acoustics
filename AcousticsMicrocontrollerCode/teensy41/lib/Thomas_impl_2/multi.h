#include <vector>
#include <cmath>
#include <iostream>
#ifndef MULTI_H
#define MULTI_H


class Pos {
    public:
        double x;
        double y;
        double z;

        double dist; // distance

        Pos(double x, double y, double z): x(x), y(y), z(z){dist = std::sqrt(x*x+y*y+z*z);}
        Pos(std::vector<double> pos);
};

std::ostream& operator <<(std::ostream& os, const Pos& pos);





Pos multi(std::vector<Pos> hydrophone_array, std::vector<double> TDOA, const double speed_of_sound=1500);

#endif