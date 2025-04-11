#include <iostream>
#include "theoretical_tdoa_calculator.h"
using namespace std;

int main()
{
    //Change these to whatever you want (relative to reference hydrophone)
    vector<double> pinger_pos = {1.23, 2.64, 3.03};
    vector<double> t_differences = calculate_t_differences(pinger_pos);

    //Pass your calculated coordinates into compare_answer
    //Thomas fix multilateration function soon plz :(
    // will do
    // vector<double> calculated_coordinates = multilateration();
    vector<double> calculated_coordinates = {1,2,3};

    compare_answer(calculated_coordinates, pinger_pos);

    return 0;
}