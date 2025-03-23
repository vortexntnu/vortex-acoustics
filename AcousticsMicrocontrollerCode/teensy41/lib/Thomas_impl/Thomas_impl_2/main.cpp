#include <iostream>
#include <fstream>
#include <vector>
#include "cross.h"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;


std::vector<double> add_time_lag(std::vector<double> x, int n){
    for (int i = 0; i < n; i++){
        x.pop_back();
        x.insert(x.begin(), 1);
    }

    return x;
}


int random(int upper, int lower){
    // Seed with a real random value, if available
    std::random_device r;
 
    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, 6);
    return uniform_dist(e1);
}

std::vector<double> add_noice(std::vector<double>& x, int n){

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



    std::vector<double> fake_x = read_csv("signal_values/hydrophone_0.csv");
    std::vector<double> fake_y = add_time_lag(fake_x, 2124);




    std::ofstream y_file("signal_values/hydrophone_1.csv");
    y_file << fake_y;


    
    std::vector<double>val = still_brute_force_but_better_best_crosscorelation_lag(fake_x, fake_y);

    std::ofstream file("signal_values/corr_values.csv");



    std::vector<double> time;
    time.reserve(val.size());

    for (int i = 0; i < val.size(); i++){
        time.push_back(i);
        time.at(i) -= static_cast<int>(val.size()/2);
    }

    plt::plot(time, val);

    plt::show();

    file << val;
    
    
    return 0;
}