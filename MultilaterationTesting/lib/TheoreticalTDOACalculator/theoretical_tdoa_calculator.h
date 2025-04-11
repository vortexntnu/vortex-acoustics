#pragma once

#include <iostream>
#include <cmath>
#include <vector>
using namespace std;


double calculate_tdoa(vector<double> ref, vector<double> pos, vector<double> pinger_pos);
vector<double> calculate_t_differences(vector<double> pinger_pos);
void compare_answer(vector<double> calculated_coordinates, vector<double> pinger_pos);