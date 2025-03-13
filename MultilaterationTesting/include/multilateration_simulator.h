#pragma once

#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

// // Speed of sound underwater: 1500 m/s
// const double v = 1500.0;

// // Hydrophone positions. 20cm between the hydrophones is optimal.
// extern vector<double> hydrophone_0;
// extern vector<double> hydrophone_1;
// extern vector<double> hydrophone_2;
// extern vector<double> hydrophone_3;
// extern vector<double> hydrophone_4;

// Function prototypes
double calculate_tdoa(vector<double> ref, vector<double> pos, vector<double> pinger_pos);
vector<double> calculate_t_differences(vector<double> pinger_pos);
void compare_answer(vector<double> calculated_coordinates, vector<double> pinger_pos);