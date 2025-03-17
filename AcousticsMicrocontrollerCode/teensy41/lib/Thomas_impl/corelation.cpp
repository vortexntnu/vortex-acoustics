#include <vector>
#include <cmath>
#include <numeric>
#include "corelation.h"

std::vector<float32_t> normalize_vector(const std::vector<float32_t>& v) {
    float32_t magnitude = 0.0;
    
    // Compute the magnitude (Euclidean norm)
    for (float32_t val : v) {
        magnitude += val * val;
    }
    // magnitude = sqrt(magnitude);
    arm_sqrt_f32(magnitude, &magnitude);


    // Avoid division by zero
    if (magnitude == 0) {
        return v;  
    }

    // Normalize each component
    std::vector<float32_t> v_norm(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        v_norm[i] = v[i]/magnitude;
    }

    return v_norm;
}








// // Does not find the center (Have alredy done it in still_brute_force_but_better_best_crosscorelation_lag)
// float32_t std_deveation(std::vector<float32_t> x){
//     int size = x.size();
//     float32_t sum = 0;
//     for (int i = 0; i < size; i++){
//         sum += x.at(i)*x.at(i);
//     }
//     float32_t deviation;
//     arm_status status = arm_sqrt_f32(sum/size, &deviation); // if you want to do something if the sqrt fails, be my guest. I am to lazy
//     return deviation;
// }




float32_t correlation(std::vector <float32_t> x, std::vector <float32_t> y, float32_t stdev_x, float32_t stdev_y){
    int size = x.size();

    float32_t covariance = 0;

    for (int i = 0; i < size; i++){
        covariance += x.at(i)*y.at(i);
    }

    covariance /= size;

    return covariance/(stdev_x*stdev_y);
}



std::vector <float32_t> still_brute_force_but_better_best_crosscorelation_lag(std::vector <float32_t> x, std::vector <float32_t> y){
    
    size_t size_x = x.size();
    size_t size_y = y.size();

    if (size_x == 0 || size_y == 0) {
        return {};  // Return empty vector if input is invalid
    }
    float32_t mean_x, mean_y;
    arm_mean_f32(x.data(), size_x, &mean_x);
    arm_mean_f32(y.data(), size_y, &mean_y);

    for(float32_t &num: x) {num -= mean_x;};
    for(float32_t &num: y) {num -= mean_y;};

    float32_t stdev_x, stdev_y;
    arm_std_f32(x.data(), size_x, &stdev_x);
    arm_std_f32(y.data(), size_y, &stdev_y);


    // // finding mean center of the signals
    // float32_t mean_x = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
    // float32_t mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
    // for(float32_t &num: x) {num -= mean_x;};
    // for(float32_t &num: y) {num -= mean_y;};
    // float32_t stdev_x = std_deveation(x);
    // float32_t stdev_y = std_deveation(y);



    int size = x.size();

    int max_lag = 2*size;


    x.resize(max_lag); // add many zeros to the end
    int zeros_to_add = max_lag - size;
    y.insert(y.begin(), zeros_to_add, 0); // add many zeros to the front


    std::vector <float32_t> x_to_sum;
    std::vector <float32_t> y_to_sum;

    std::vector <float32_t> correlation_vect;


    for (int i = 0; i < (max_lag-1); i++){



        x.insert(x.begin(), x.back());
        x.pop_back();

        correlation_vect.push_back(correlation(x, y, mean_x, mean_y));
    }
    return correlation_vect;
    
}