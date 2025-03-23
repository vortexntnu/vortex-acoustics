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
    covariance /= x.size();

    

    return covariance / (stdev_x * stdev_y);
}

// Compute a cross-correlation lag vector between two signals using a brute-force approach.
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
    size_t size = x.size();
    int max_lag = 2 * static_cast<int>(size);

    // Extend x by appending zeros.
    x.resize(max_lag, 0.0);
    // Extend y by prepending zeros.
    int zeros_to_add = max_lag - static_cast<int>(size);
    y.insert(y.begin(), zeros_to_add, 0.0);

    std::vector<double> correlation_vect;


    auto overlap = [&](int shift) {
        // Original data for x, before shifting, was at indices [0:size-1].
        // After we append zeros, the length is max_lag, but x's "real" data
        // (before shift) is conceptually in [0, size-1].
        // Once we shift x by `shift`, that "real" data moves to [shift, shift + size - 1].

        // Original data for y is in [zeros_to_add, zeros_to_add + size - 1],
        // because we prepended zeros_to_add elements.

        int startX = shift;
        int endX   = shift + static_cast<int>(size) - 1;

        int startY = zeros_to_add;
        int endY   = zeros_to_add + static_cast<int>(size) - 1;

        // Intersection of the two ranges:
        int overlapStart = std::max(startX, startY);
        int overlapEnd   = std::min(endX, endY);

        int overlapLength = overlapEnd - overlapStart + 1;
        return (overlapLength > 0) ? overlapLength : 0;
    };

    for (int i = 0; i < (max_lag - 1); i++) {
        // Perform a circular shift: move the last element to the front.
        x.insert(x.begin(), 0.);
        x.pop_back();


        // Use the standard deviations in the correlation computation.
        correlation_vect.push_back(correlation(x, y, stdev_x, stdev_y));
    }
    return correlation_vect;
}
//taper