
#include <vector>
#include <cmath>
#include <iostream>



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





std::vector<double> multilateration(std::vector<Pos> hydrophone_array, std::vector<double> TDOA);

