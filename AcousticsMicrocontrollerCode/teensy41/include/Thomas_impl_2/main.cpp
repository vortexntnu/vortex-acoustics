#include <iostream>
#include <fstream>
#include <vector>
#include "cross.h"
#include <random>
#include "multi.h"
#include "find_pinger.h"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;


std::vector<double> add_time_lag(std::vector<double> x, int n){
    for (int i = 0; i < n; i++){
        x.pop_back();
        x.insert(x.begin(), 1);
    }

    return x;
}


std::vector<double> add_noise(std::vector<double> x, int n){
    
    std::random_device r;
    std::default_random_engine e1(r());


    
    // Add noise to each element of the signal
    for (auto &value : x) {
        std::uniform_int_distribution<int> uniform_dist(-std::abs(n), std::abs(n));
        int mean = uniform_dist(e1);
        value += mean;
    }
    
    return x;
}


std::vector<double> read_csv(const std::string& path){
    std::vector<double> values;
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path << std::endl;
        return values;
    }
    
    std::string token;

    while (std::getline(file, token, ',')) {
        // Convert token to double. (Note: atof returns 0.0 if conversion fails)
        double value = std::atof(token.c_str());
        values.push_back(value);
    }
    return values;
}


std::ostream& operator<<(std::ostream& os, const std::vector<double>& vect){
    // Write each element, separated by commas.
    for (size_t i = 0; i < vect.size(); ++i) {
        os << vect[i];
        // Avoid appending a comma after the last element.
        if (i != vect.size() - 1) {
            os << ",";
        }
    }
    return os;
}


bool write_csv(const std::string &path, const std::vector<double>& vect) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << path << std::endl;
        return false;
    }
    
    // Write each element, separated by commas.
    file << vect;
    
    file.close();
    return true;
}


int main(){

    std::vector<double> TDOA = {0.00108333, 0.00116667, 0.00116667, 0.00121667};

    // main is allways zero
    std::vector<Pos> hydrophone_array = {
        {1.0, 1.0, 1.0},
        {1.2, 0.8, 1.1}, 
        {0.9, 1.3, 1.2},
        {1.1, 1.4, 0.9}
    };

    
    Pos pinger_pos = find_pinger("/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl_2/signal_values/hydrophone.csv", hydrophone_array);

    std::cout << "Pinger position: " << pinger_pos << std::endl;
    return 0;
}