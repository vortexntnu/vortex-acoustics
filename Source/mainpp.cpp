#include "mainpp.h"
#include <ros.h>

ros::NodeHandle nh;

std_msgs::Float32 x_pos_es, y_pos_es;
std_msgs::time_t time_of_es;

ros::init("acoustics");

ros::Publisher pub("Position", &pressure);