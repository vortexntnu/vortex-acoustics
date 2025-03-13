#include <iostream>
using namespace std;

#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

// Speed of sound underwater: 1500 m/s
const double v = 1500.0;

// Hydrophone positions. 20cm between the hydrophones is optimal.
//You may change these coordinates
vector<double> hydrophone_0 = {0.0, 0.0, 0.0};
vector<double> hydrophone_1 = {0.2, 0.2, 0.0};
vector<double> hydrophone_2 = {0.0, 0.2, 0.2};
vector<double> hydrophone_3 = {0.2, 0.0, 0.2};
vector<double> hydrophone_4 = {0.0, 0.0, 0.2};

// Positive TDOA if the reference hydrophone is closest to the pinger
//Calculates the time difference of arrival between the reference hydrophone and another hydrophone
double calculate_tdoa(vector<double> ref, vector<double> pos, vector<double> pinger_pos) {
    double ref_distance = sqrt(pow(ref[0] - pinger_pos[0], 2) +
                               pow(ref[1] - pinger_pos[1], 2) +
                               pow(ref[2] - pinger_pos[2], 2));

    double pos_distance = sqrt(pow(pos[0] - pinger_pos[0], 2) +
                               pow(pos[1] - pinger_pos[1], 2) +
                               pow(pos[2] - pinger_pos[2], 2));

    return (1 / v) * (pos_distance - ref_distance);
}

//Prints time difference between hydrophone 0 and hydrophone number i
vector<double> calculate_t_differences(vector<double> pinger_pos) {
    double tdoa_01 = calculate_tdoa(hydrophone_0, hydrophone_1, pinger_pos);
    double tdoa_02 = calculate_tdoa(hydrophone_0, hydrophone_2, pinger_pos);
    double tdoa_03 = calculate_tdoa(hydrophone_0, hydrophone_3, pinger_pos);
    double tdoa_04 = calculate_tdoa(hydrophone_0, hydrophone_4, pinger_pos);

    cout << "TDOA 01: " << tdoa_01 << endl;
    cout << "TDOA 02: " << tdoa_02 << endl;
    cout << "TDOA 03: " << tdoa_03 << endl;
    cout << "TDOA 04: " << tdoa_04 << endl;

    return {tdoa_01, tdoa_02, tdoa_03, tdoa_04};
}



// Compare calculated coordinates with actual pinger position
void compare_answer(vector<double> calculated_coordinates, vector<double> pinger_pos) {
    cout << "Actual pinger position: [" 
         << pinger_pos[0] << ", " 
         << pinger_pos[1] << ", " 
         << pinger_pos[2] << "]" << endl;

    cout << "Calculated pinger position: [" 
         << calculated_coordinates[0] << ", " 
         << calculated_coordinates[1] << ", " 
         << calculated_coordinates[2] << "]" << endl;

    cout << "Your error was: ["
         << pinger_pos[0] - calculated_coordinates[0] << ", "
         << pinger_pos[1] - calculated_coordinates[1] << ", "
         << pinger_pos[2] - calculated_coordinates[2] << "]" << endl;
}

// int main() {
//     // Pinger position
//     vector<double> pinger_pos = {1.23, 2.64, 3.03};
//     vector<double> calculated_coordinates = {1.20, 2.60, 3.00};
//     calculate_t_differences(pinger_pos);
//     compare_answer(calculated_coordinates, pinger_pos);

//     return 0;
// }
