#include <iostream>
#include <vector>
#include <cmath>
#include "tdoa.h"
#include <Eigen/Dense>
#include "arm_math.h"

using namespace std;
using namespace Eigen;

MatrixXd vectorToEigenMatrix(const vector<vector<double>>& A);
VectorXd vectorToEigenVector(const vector<double>& b);



std::vector<double> multilateration(std::vector<Pos> hydrophone_array, std::vector<double> TDOA){
    // Firs element in hydrophone is zero
    // The first element in TDOA is the timedifference between the main and first hydrophone in hydrophone array,
    // the second element for the main and second hydrophone and so on.


    //d == c*TDOA
    int const c = 1500;

    std::vector<double> distances = TDOA;
    for (int i = 0; i < distances.size(); i++){
        distances.at(i) *= c;
    };

    std::vector<std::vector<double>> A = { 
        {hydrophone_array.at(0).x, hydrophone_array.at(0).y, hydrophone_array.at(0).z, -distances.at(0)},
        {hydrophone_array.at(1).x, hydrophone_array.at(1).y, hydrophone_array.at(1).z, -distances.at(1)},
        {hydrophone_array.at(2).x, hydrophone_array.at(2).y, hydrophone_array.at(2).z, -distances.at(2)},
        {hydrophone_array.at(3).x, hydrophone_array.at(3).y, hydrophone_array.at(3).z, -distances.at(3)}
    };
    
    std::vector<double> b = {
        0.5 * (std::pow(hydrophone_array.at(0).x, 2) + std::pow(hydrophone_array.at(0).y, 2) + std::pow(hydrophone_array.at(0).z, 2) - std::pow(distances.at(0), 2)),
        0.5 * (std::pow(hydrophone_array.at(1).x, 2) + std::pow(hydrophone_array.at(1).y, 2) + std::pow(hydrophone_array.at(1).z, 2) - std::pow(distances.at(1), 2)),
        0.5 * (std::pow(hydrophone_array.at(2).x, 2) + std::pow(hydrophone_array.at(2).y, 2) + std::pow(hydrophone_array.at(2).z, 2) - std::pow(distances.at(2), 2)),
        0.5 * (std::pow(hydrophone_array.at(3).x, 2) + std::pow(hydrophone_array.at(3).y, 2) + std::pow(hydrophone_array.at(3).z, 2) - std::pow(distances.at(3), 2))
    };

    Eigen::MatrixXd A_matrix = vectorToEigenMatrix(A);
    Eigen::VectorXd b_vect = vectorToEigenVector(b);

    VectorXd x = A_matrix.colPivHouseholderQr().solve(b_vect); // Dersom du lurer på kva denne funksjonen gjer: ¯\_(ツ)_/¯. Den løysar likninga A*x = b

    return vector<double>(x.data(), x.data() + x.size()); 

}







// Convert std::vector to Eigen::MatrixXd
MatrixXd vectorToEigenMatrix(const vector<vector<double>>& A) {
    int rows = A.size();
    int cols = A[0].size();
    MatrixXd eigenMatrix(rows, cols);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            eigenMatrix(i, j) = A[i][j];

    return eigenMatrix;
}

// Convert std::vector to Eigen::VectorXd
VectorXd vectorToEigenVector(const vector<double>& b) {
    int rows = b.size();
    VectorXd eigenVector(rows);

    for (int i = 0; i < rows; i++)
        eigenVector(i) = b[i];

    return eigenVector;
}






Pos::Pos(std::vector<float32_t> pos) x(pos.at(0)), y(pos.at(1)), z(pos.at(2)){
    if (pos.size() >= 4){
        dist(pos[3])
        if (pos.size() > 4){
            stc::cout << pos.size()-4 << " values where not used" << std::endl;
        }
    }
    else{ arm_sqrt_f32(x*x+y*y+z*z, &dist); }

}