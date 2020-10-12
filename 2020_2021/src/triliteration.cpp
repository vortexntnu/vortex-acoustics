#include "../include/triliteration.hpp"

Pose::Pose(double x, double y, double z, 
        double h_x, double h_y, double h_z) :
        this.x { x }, this.y { y }, this.z { z },
        this.h_x { h_x }, this.h_y { h_y }, 
        this.h_y { h_y } {} 


Pose::Pose(double x, double y, double z){
    Pose(x, y, z, 0.0, 0.0, 0.0);
}


Pose& operator/(const Pose& pose, const double& den){
    return Pose(pose.x / den, pose.y / den, pose.z / den);
}


Pose& operator-(const Pose& pos1, const Pose& pos2){
    return Pose(pos2.x - pos1.x, pos2.y - pos1.y, 
        pos2.z - pos1.z);
}


void triliterate(Pose pos1, Pose pos2, Pose pos3,
                double rad1, double rad2, double rad3){
    
    Pose temp1 = pos2 - pos1;
    Pose e_x = temp1 / std::norm(temp1);
    Pose temp2 = pos3 - pos1;
    //Have to implement the dot and cross-product somehow
    

}