#include <vector>
#include <cmath>
#include <numeric>
#include "cross.h"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;


// Normalize vector using the Euclidean norm. Not used at the moment
std::vector<double> normalize_vector(const std::vector<double>& v) {
    double magnitude = 0.0;
    // Compute the sum of squares.
    for (double val : v) {
        magnitude += val * val;
    }
    // Compute the Euclidean norm.
    magnitude = std::sqrt(magnitude);

    // Avoid division by zero.
    if (magnitude == 0.0) {
        return v;
    }

    // Normalize each component.
    std::vector<double> v_norm(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        v_norm[i] = v[i] / magnitude;
    }
    return v_norm;
}



// Compute the correlation between two vectors given their standard deviations.
// Both vectors are assumed to be mean-centered.
double correlation(const std::vector<double>& x, const std::vector<double>& y, double stdev_x, double stdev_y) {
    size_t size = x.size();
    double covariance = 0.0;

    // Compute the covariance.
    for (size_t i = 0; i < size; i++) {
        covariance += x[i] * y[i];
    }
    covariance /= size;

    

    return covariance / (stdev_x * stdev_y);
}

// Compute a cross-correlation lag vector between two signals using a brute-force approach. the peek index is how much x is lagging behind y
std::vector<double> still_brute_force_but_better_best_crosscorelation_lag(std::vector<double> x, std::vector<double> y) {
    size_t size_x = x.size();
    size_t size_y = y.size();

    if (size_x == 0 || size_y == 0) {
        return {};  // Return an empty vector if inputs are invalid.
    }

    // Compute the mean for each vector.
    double mean_x = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
    double mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();

    // Mean-center the vectors.
    for (double &num : x) {
        num -= mean_x;
    }
    for (double &num : y) {
        num -= mean_y;
    }


    // Compute the standard deviation for each vector.
    double sum_sq_x = 0.0;
    for (double val : x) {
        sum_sq_x += val * val;
    }
    double stdev_x = std::sqrt(sum_sq_x / x.size());

    double sum_sq_y = 0.0;
    for (double val : y) {
        sum_sq_y += val * val;
    }
    double stdev_y = std::sqrt(sum_sq_y / y.size());

    // Prepare for cross-correlation by zero-padding.
    int max_lag = 2 * static_cast<int>(size_x) - 1;

    // Extend x by appending zeros.
    x.resize(max_lag, 0.0);
    // Extend y by adding zeros to the front.
    int zeros_to_add = size_x;
    y.insert(y.begin(), zeros_to_add, 0.0);

    std::vector<double> correlation_vect;

    
    for (int i = 0; i < (max_lag); i++) {
        x.insert(x.begin(), 0.);
        x.pop_back();
        

        correlation_vect.push_back(correlation(x, y, stdev_x, stdev_y));
    }

    return correlation_vect;
}






// Finds index from the center and index to the right is positive, to the left is negative
int find_peak_index_centered(const std::vector<double>& signal) {
    // If the signal is empty, return 0 
    if (signal.empty()) {
        return 0;
    }

    int peakIndex = 0;
    double peakValue = signal[0];

    // Loop through the signal starting from the first element.
    for (int i = 1; i < signal.size(); ++i) {
        if (signal[i] > peakValue) {
            peakValue = signal[i];
            peakIndex = i;
        }
    }
    return peakIndex - signal.size()/2;
}