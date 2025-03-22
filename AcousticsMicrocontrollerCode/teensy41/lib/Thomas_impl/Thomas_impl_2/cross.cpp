#include <vector>
#include <cmath>
#include <Eigen/Dense>

// Normalize a vector using Eigen and double.
std::vector<double> normalize_vector(const std::vector<double>& v) {
    // Map the input std::vector to an Eigen vector
    Eigen::Map<const Eigen::VectorXd> vec(v.data(), v.size());
    double magnitude = vec.norm();  // Euclidean norm
    if (magnitude == 0) {
        return v;  // Avoid division by zero
    }
    Eigen::VectorXd normalized = vec / magnitude;
    // Convert back to std::vector<double>
    return std::vector<double>(normalized.data(), normalized.data() + normalized.size());
}

// Compute the correlation between two vectors, given their standard deviations.
double correlation(const std::vector<double>& x, const std::vector<double>& y, double stdev_x, double stdev_y) {
    Eigen::Map<const Eigen::VectorXd> vec_x(x.data(), x.size());
    Eigen::Map<const Eigen::VectorXd> vec_y(y.data(), y.size());
    double covariance = vec_x.dot(vec_y) / static_cast<double>(x.size());
    return covariance / (stdev_x * stdev_y);
}


std::vector<double> still_brute_force_but_better_best_crosscorelation_lag(std::vector<double> x, std::vector<double> y) {
    size_t size_x = x.size();
    size_t size_y = y.size();
    if (size_x == 0 || size_y == 0) {
        return {};  // Return empty vector if input is invalid
    }
    
    // Compute means using Eigen
    Eigen::Map<const Eigen::VectorXd> vec_x(x.data(), size_x);
    Eigen::Map<const Eigen::VectorXd> vec_y(y.data(), size_y);
    double mean_x = vec_x.mean();
    double mean_y = vec_y.mean();
    
    // Center the vectors
    for (double &num : x) { num -= mean_x; }
    for (double &num : y) { num -= mean_y; }
    
    // Compute standard deviations (population version)
    double stdev_x = std::sqrt(Eigen::Map<const Eigen::VectorXd>(x.data(), size_x).array().square().sum() / size_x);
    double stdev_y = std::sqrt(Eigen::Map<const Eigen::VectorXd>(y.data(), size_y).array().square().sum() / size_y);
    
    int size = static_cast<int>(x.size());
    int max_lag = 2 * size;
    
    // Extend x by appending zeros and y by prepending zeros
    x.resize(max_lag, 0.0);
    int zeros_to_add = max_lag - size;
    y.insert(y.begin(), zeros_to_add, 0.0);
    
    std::vector<double> correlation_vect;
    
    // For each lag, circularly shift x and compute the correlation
    for (int i = 0; i < (max_lag - 1); i++) {

        x.insert(x.begin(), 0);
        x.pop_back();
        
        correlation_vect.push_back(correlation(x, y, stdev_x, stdev_y));
    }
    
    return correlation_vect;
}
