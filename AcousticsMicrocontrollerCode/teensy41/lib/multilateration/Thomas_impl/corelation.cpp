#include <vector>
#include <cmath>
#include <numeric>






std::vector<double> normalize_vector(const std::vector<double>& v) {
    double magnitude = 0.0;
    
    // Compute the magnitude (Euclidean norm)
    for (double val : v) {
        magnitude += val * val;
    }
    magnitude = std::sqrt(magnitude);

    // Avoid division by zero
    if (magnitude == 0) {
        return v;  
    }

    // Normalize each component
    std::vector<double> v_norm;
    for (double val : v) {
        v_norm.push_back(val / magnitude);
    }

    return v_norm;
}








// Does not find the center (Have alredy done it in still_brute_force_but_better_best_crosscorelation_lag)
double std_deveation(std::vector<double> x){
    int size = x.size();

    double sum = 0;

    for (int i = 0; i < size; i++){
        sum += x.at(i)*x.at(i);
    }

    return std::sqrt(sum/size);
}




double correlation(std::vector <double> x, std::vector <double> y, double stdev_x, double stdev_y){
    int size = x.size();

    double covariance = 0;

    for (int i = 0; i < size; i++){
        covariance += x.at(i)*y.at(i);
    }

    covariance /= size;

    return covariance/(stdev_x*stdev_y);
}



std::vector <double> still_brute_force_but_better_best_crosscorelation_lag(std::vector <double> x, std::vector <double> y){
    

    // finding mean center of the signals
    double mean_x = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
    double mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
    for(double &num: x) {num -= mean_x;};
    for(double &num: y) {num -= mean_y;};

    double stdev_x = std_deveation(x);
    double stdev_y = std_deveation(y);

    int size = x.size();

    int max_lag = 2*size;


    x.resize(max_lag); // add many zeros to the end
    int zeros_to_add = max_lag - size;
    y.insert(y.begin(), zeros_to_add, 0); // add many zeros to the front


    std::vector <double> x_to_sum;
    std::vector <double> y_to_sum;

    std::vector <double> correlation_vect;


    for (int i = 0; i < (max_lag-1); i++){



        x.insert(x.begin(), x.back());
        x.pop_back();

        correlation_vect.push_back(correlation(x, y, mean_x, mean_y));
    }
    return correlation_vect;
    
}