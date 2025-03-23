#include <iostream>
#include <fstream>
#include <vector>
#include "cross.h"
#include <random>
#include "multi.h"

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

    // main is allways zero
    std::vector<Pos> hydrophone_array = {
        {1.0, 1.0, 1.0},
        {1.2, 0.8, 1.1}, 
        {0.9, 1.3, 1.2},
        {1.1, 1.4, 0.9}
    };



    std::vector<double> fake_x = read_csv("signal_values/hydrophone_0.csv");
    std::vector<double> fake_y = add_noise(add_time_lag(fake_x, 2124), 20000);




    std::ofstream y_file("signal_values/hydrophone_1.csv");
    y_file << fake_y;


    
    std::vector<double>val = still_brute_force_but_better_best_crosscorelation_lag(fake_x, fake_y);




    std::vector<double> time;
    time.reserve(val.size());

    for (int i = 0; i < val.size(); i++){
        time.push_back(i);
        time.at(i) -= static_cast<int>(val.size()/2);
    }

    plt::plot(time, val);

    plt::show();
    
    
    return 0;
}