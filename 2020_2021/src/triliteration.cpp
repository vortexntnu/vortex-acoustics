#include "../include/triliteration.hpp"

/**
 * Constructor
 */
Pos::Pos(double x, double y, double z) : 
        x(x), y(y), z(z) {}



/**
 * Operator overload for Pos
 */
Pos operator*(double val, const Pos& pos){
    return Pos(pos.x * val, pos.y * val, pos.z * val);
}

Pos operator/(const Pos& pos, const double& den){
    return Pos(pos.x / den, pos.y / den, pos.z / den);
}


Pos operator-(const Pos& pos1, const Pos& pos2){
    return Pos(pos2.x - pos1.x, pos2.y - pos1.y, 
        pos2.z - pos1.z);
}

Pos operator+(const Pos& pos1, const Pos& pos2){
    return Pos(pos1.x + pos2.x, pos1.y + pos2.y, 
        pos1.z + pos2.z);
}



/**
 * Helper functions for Pos and linalg
 */
Pos pos_cross(const Pos& pos1, const Pos& pos2){
    //Find a good way to implement this
}


double pos_norm(const Pos& pos){
    return std::sqrt(
        pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2));
}


double pos_dot(const Pos& pos1, const Pos& pos2){
    return (pos1.x * pos2.x + 
        pos1.y * pos2.y + pos1.z * pos2.z);
}


std::pair<Pos, Pos> triliterate(Pos pos1, Pos pos2, Pos pos3,
                double rad1, double rad2, double rad3){
    
    //Just what the fuck does these varible-names mean???
    // Change them, please!

    Pos temp1 = pos2 - pos1;
    Pos e_x = temp1 / pos_norm(temp1);
    Pos temp2 = pos3 - pos1;

    double i = pos_dot(e_x, temp2);
    Pos temp3 = temp2 - i * e_x;
    Pos e_y = temp3 / pos_norm(temp3);

    Pos e_z = pos_cross(e_x, e_y);
    double d = pos_norm(pos2 - pos1);
    double j = pos_dot(e_y, temp2);
    double x = (pow(rad1, 2) - pow(rad2, 2) + pow(d, 2)) /
                (2 * d);
    double y = (pow(rad1, 2) - pow(rad3, 2) - 2 * i * x +
                 pow(i, 2) + pow(j, 2)) / (2 * d);
    double temp4 = pow(rad1, 2) - pow(x, 2) - pow(y, 2);
    if(temp4 < 0){
        throw("The three spheres do not intersect");
    }
    double z = std::sqrt(temp4);
    Pos p_12_a = pos1 + x * e_x + y * e_y + z * e_z;
    Pos p_12_b = pos2 + x * e_x + y * e_y - z * e_z;

    return std::make_pair(p_12_a, p_12_b);
}