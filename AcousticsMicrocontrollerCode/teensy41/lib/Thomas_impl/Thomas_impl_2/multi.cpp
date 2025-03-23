
#include <iostream>
#include <vector>
#include "multi.h"
#include <cmath>

#include <Eigen/Dense>



std::vector<double> multilateration(std::vector<Pos> hydrophone_array, std::vector<double> TDOA) {
    // The first element in hydrophone_array is the main hydrophone.
    // The first element in TDOA is the time difference between the main and the first hydrophone,
    // the second element between the main and the second hydrophone, and so on.
    if (TDOA.size() < 4 || hydrophone_array.size() < 4) {
        std::cout << "Error: Not enough hydrophones or TDOA values for multilateration.\n";
        return std::vector<double>(4, 0.0);  // Return a vector of four zeros.
    }

    // Convert time differences to distances: d = c * TDOA, where c is the speed of sound in water.
    const double c = 1500.0;
    std::vector<double> distances = TDOA;
    for (size_t i = 0; i < distances.size(); i++){
        distances.at(i) *= c;
    }

    // Build the A matrix (4x4) using the hydrophone positions and distances.
    double A_data[4 * 4] = {
        hydrophone_array.at(0).x, hydrophone_array.at(0).y, hydrophone_array.at(0).z, -distances.at(0),
        hydrophone_array.at(1).x, hydrophone_array.at(1).y, hydrophone_array.at(1).z, -distances.at(1),
        hydrophone_array.at(2).x, hydrophone_array.at(2).y, hydrophone_array.at(2).z, -distances.at(2),
        hydrophone_array.at(3).x, hydrophone_array.at(3).y, hydrophone_array.at(3).z, -distances.at(3)
    };

    // Helper lambda for squaring a value.
    auto square = [](double x) -> double { return x * x; };

    // Build the b vector (4x1) according to:
    // 0.5 * (x^2 + y^2 + z^2 - distance^2)
    double b_data[4] = {
        0.5 * (square(hydrophone_array.at(0).x) + square(hydrophone_array.at(0).y) + square(hydrophone_array.at(0).z) - square(distances.at(0))),
        0.5 * (square(hydrophone_array.at(1).x) + square(hydrophone_array.at(1).y) + square(hydrophone_array.at(1).z) - square(distances.at(1))),
        0.5 * (square(hydrophone_array.at(2).x) + square(hydrophone_array.at(2).y) + square(hydrophone_array.at(2).z) - square(distances.at(2))),
        0.5 * (square(hydrophone_array.at(3).x) + square(hydrophone_array.at(3).y) + square(hydrophone_array.at(3).z) - square(distances.at(3)))
    };

    // Map the raw arrays to Eigen objects.
    // Note: A_data is stored in row-major order.
    Eigen::Map<Eigen::Matrix<double, 4, 4, Eigen::RowMajor>> A(A_data);
    Eigen::Map<Eigen::Vector4d> b(b_data);

    // Solve the system A * x = b using QR decomposition.
    Eigen::Vector4d x = A.colPivHouseholderQr().solve(b);

    // Convert the Eigen vector to std::vector<double> for the result.
    std::vector<double> result_vect_format(4);
    for (int i = 0; i < 4; ++i) {
        result_vect_format[i] = x[i];
    }

    return result_vect_format;
}





Pos::Pos(std::vector<double> pos): x(pos.at(0)), y(pos.at(1)), z(pos.at(2)){
    if (pos.size() >= 4){
        dist = pos[3];
        if (pos.size() > 4){
            std::cout << pos.size()-4 << " values where not used" << std::endl;
        }
    }
    else{ dist = std::sqrt(x*x+y*y+z*z); }

}


std::ostream& operator <<(std::ostream& os, const Pos& pos){
    os << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ", distance: " << pos.dist;
    return os;
}