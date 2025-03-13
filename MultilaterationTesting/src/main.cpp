#include <iostream>
#include "multilateration_simulator.h"
using namespace std;

int main()
{
    vector<double> pinger_pos = {1.23, 2.64, 3.03};

   
    vector<double> t_differences = calculate_t_differences(pinger_pos);
    //Pass your calculated coordinates into compare_answer
    vector<double> calculated_coordinates = {1.20, 2.60, 3.00};
    compare_answer(calculated_coordinates, pinger_pos);
    // run_simulation();
    return 0;
}