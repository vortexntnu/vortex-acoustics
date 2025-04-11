#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>

#include "multi.h"
#include "cross.h"
#include "find_pinger.h"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;


std::vector<std::vector<double>> read_csv_array(const std::string& path);



Pos find_pinger(const std::string& hydrophone_path, std::vector<Pos> hydrophone_array, int sampling_frequency){
    double speed_of_sound = 1500;

    if (hydrophone_array.size() != 4){
        std::cout << "We need 5 hydrophones to use this approach you dummy. (This can works in testing)" << std::endl
                  << "The hydrophone array is " << hydrophone_array.size() << " long and should be 4 (The main one is always {0, 0, 0}, so no need for that in the array)" << std::endl;
    }

    std::vector<std::vector<double>> hydrophone_value_array = read_csv_array(hydrophone_path);

    std::vector<double> TDOA;

    

    for (int i = 1; i < (hydrophone_value_array.size()); i++){

        std::vector<double> correlation = still_brute_force_but_better_best_crosscorelation_lag(hydrophone_value_array.at(0), hydrophone_value_array.at(i));
        int peek_index = find_peak_index_centered(correlation);


        double time_difference = static_cast<double>(peek_index)/sampling_frequency;
        TDOA.push_back(static_cast<double>(time_difference));
    }

    std::cout << "TDOA: ";
    for (double num: TDOA){
        std::cout << num << ", ";
    }
    std::cout << "\n";
    
    Pos pinger_pos = multi(hydrophone_array, TDOA);

    return pinger_pos;


}







std::vector<std::vector<double>> read_csv_array(const std::string& path) {
    std::ifstream file(path);
    std::vector<std::vector<double>> data;

    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return data;
    }

    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) continue;  // Skip empty lines

        std::stringstream ss(line);
        std::string token;
        std::vector<double> row;

        // Split the line by commas and convert tokens to double.
        while (std::getline(ss, token, ',')) {
            try {
                double value = std::stod(token);
                row.push_back(value);
            } catch (const std::exception& e) {
                // If conversion fails, push a default value (e.g., 0.0).
                row.push_back(0.0);
            }
        }

        // On the first line, initialize the outer vector to have as many inner vectors as there are columns.
        if (firstLine) {
            data.resize(row.size());
            firstLine = false;
        }

        // Append each value to its corresponding hydrophone vector.
        for (size_t i = 0; i < row.size(); ++i) {
            data[i].push_back(row[i]);
        }
    }

    return data;
}




