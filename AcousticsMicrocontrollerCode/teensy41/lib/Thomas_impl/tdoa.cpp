
#include <vector>
#include "tdoa.h"
#include "corelation.h"
#include <Arduino.h>


namespace Multilateration{

// for testing
float32_t calculate_tdoa(Pos pos, Pos pinger_pos, float32_t v){
    Pos relative_pos(pos.x-pinger_pos.x,pos.y-pinger_pos.y,pos.z-pinger_pos.z);
    return (1/v)*(relative_pos.dist-pinger_pos.dist);
}




// All this just to solve A*x=b like this x = A⁻¹b
Pos tdoa_multilateration(const Pos hydrophone_pos_array[NUM_HYDROPHONES], const double TDOA[TDOA_DATA_LENGHT]){
    // Firs element in hydrophone is zero
    // The first element in TDOA is the timedifference between the main and first hydrophone in hydrophone array,
    // the second element for the main and second hydrophone and so on.


    //d == c*TDOA
    float32_t const c = 1500;

    double distances[TDOA_DATA_LENGHT];
    for (size_t i = 0; i < TDOA_DATA_LENGHT; i++){
        distances[i] = c*TDOA[i];
    };


    float32_t A_data[4 * 4] = {
        hydrophone_pos_array[1].x, hydrophone_pos_array[1].y, hydrophone_pos_array[1].z, -distances[0],
        hydrophone_pos_array[2].x, hydrophone_pos_array[2].y, hydrophone_pos_array[2].z, -distances[1],
        hydrophone_pos_array[3].x, hydrophone_pos_array[3].y, hydrophone_pos_array[3].z, -distances[2],
        hydrophone_pos_array[4].x, hydrophone_pos_array[4].y, hydrophone_pos_array[4].z, -distances[3]
    };
    
    auto square = [](float32_t x) -> float32_t { return x * x; };

    float32_t b_data[4] = {
        0.5f * (square(hydrophone_pos_array[1].x) + square(hydrophone_pos_array[1].y) + square(hydrophone_pos_array[1].z) - square(distances[0])),
        0.5f * (square(hydrophone_pos_array[2].x) + square(hydrophone_pos_array[2].y) + square(hydrophone_pos_array[2].z) - square(distances[1])),
        0.5f * (square(hydrophone_pos_array[3].x) + square(hydrophone_pos_array[3].y) + square(hydrophone_pos_array[3].z) - square(distances[2])),
        0.5f * (square(hydrophone_pos_array[4].x) + square(hydrophone_pos_array[4].y) + square(hydrophone_pos_array[4].z) - square(distances[3]))
    };


    arm_matrix_instance_f32 A;
    arm_mat_init_f32(&A, 4, 4, A_data);

    arm_matrix_instance_f32 b;
    arm_mat_init_f32(&b, 4, 1, b_data);

    float32_t A_inverse_data[4 * 4];
    arm_matrix_instance_f32 A_inverse;
    arm_mat_init_f32(&A_inverse, 4, 4, A_inverse_data);

    float32_t resulting_data[4];
    arm_matrix_instance_f32 result_vect;
    arm_mat_init_f32(&result_vect, 4, 1, resulting_data);
    std::vector<float32_t> result_vect_format(4, 0.0f); // for returning
    


    arm_status status =  arm_mat_inverse_f32(&A, &A_inverse);

    if (status == ARM_MATH_SUCCESS){
        status = arm_mat_mult_f32(&A_inverse, &b, &result_vect);

        
        if (status == ARM_MATH_SUCCESS){
            // Store the result in a std::vector
            for (int i = 0; i < 4; i++) {

                result_vect_format[i] = resulting_data[i];
            }
        }
    }

    return Pos(result_vect_format); // this will return all zeros if some calculationes failed
}








// TODO: Solve for x = (A^T A)^{-1} A^T b, which gives a more presise and reliable estimated position.
Pos multilateration_least_square(const std::vector<Pos>& hydrophones,
                                 const std::vector<float32_t>& tdoa,
                                 float32_t c)
{
    return Pos(0, 0, 0);
}


#include <math.h>

// Define related constants.
#define NUM_EQUATIONS (NUM_HYDROPHONES - 1)
#define MAX_ITER 20     // maximum iterations for Gauss-Newton
#define TOL 1e-8f      // convergence tolerance

//------------------------------------------------------------------------------
// Linear Least-Squares Estimate
//------------------------------------------------------------------------------
void linearLeastSquaresEstimate(const float32_t hydrophones[NUM_HYDROPHONES][3],
                                  const float32_t tdoas[TDOA_DATA_LENGHT],
                                  float32_t x_est[3], float32_t c)
{
    // Build matrix A (size: (NUM_EQUATIONS x 3)) and vector b (NUM_EQUATIONS x 1)
    float32_t A_data[NUM_EQUATIONS * 3];
    float32_t b_data[NUM_EQUATIONS];
    
    // Compute squared norm of reference hydrophone (hydrophone 0)
    float32_t p0_sq = hydrophones[0][0]*hydrophones[0][0] +
                      hydrophones[0][1]*hydrophones[0][1] +
                      hydrophones[0][2]*hydrophones[0][2];
    
    // Fill A and b. For i = 1,2,...,NUM_hydrophones-1:
    //   A[i-1,:] = hydrophones[i] - hydrophones[0]
    //   b[i-1] = 0.5 * [ (||p_i||^2 - ||p_0||^2) - c^2 * (tdoa[i-1])^2 ]
    for (int i = 0; i < NUM_EQUATIONS; i++)
    {
        A_data[i*3 + 0] = hydrophones[i+1][0] - hydrophones[0][0];
        A_data[i*3 + 1] = hydrophones[i+1][1] - hydrophones[0][1];
        A_data[i*3 + 2] = hydrophones[i+1][2] - hydrophones[0][2];

        float32_t pi_sq = hydrophones[i+1][0]*hydrophones[i+1][0] +
                          hydrophones[i+1][1]*hydrophones[i+1][1] +
                          hydrophones[i+1][2]*hydrophones[i+1][2];
        b_data[i] = 0.5f * (pi_sq - p0_sq - c * c * (tdoas[i] * tdoas[i]));
    }
    
    // Create CMSIS matrix instances
    arm_matrix_instance_f32 A, AT, ATA, ATA_inv, ATb, x_ls;
    float32_t AT_data[3 * NUM_EQUATIONS];
    float32_t ATA_data[9];          // 3x3 matrix
    float32_t ATA_inv_data[9];
    float32_t ATb_data[3];
    float32_t x_ls_data[3];

    arm_mat_init_f32(&A, NUM_EQUATIONS, 3, A_data);
    arm_mat_init_f32(&AT, 3, NUM_EQUATIONS, AT_data);
    arm_mat_init_f32(&ATA, 3, 3, ATA_data);
    arm_mat_init_f32(&ATA_inv, 3, 3, ATA_inv_data);
    arm_mat_init_f32(&ATb, 3, 1, ATb_data);
    arm_mat_init_f32(&x_ls, 3, 1, x_ls_data);
    
    // Compute transpose: AT = A^T
    arm_mat_trans_f32(&A, &AT);
    
    // Compute ATA = A^T * A (3x3 matrix)
    arm_mat_mult_f32(&AT, &A, &ATA);
    
    // Invert ATA
    arm_status status = arm_mat_inverse_f32(&ATA, &ATA_inv);
    if (status != ARM_MATH_SUCCESS) {
        Serial.println("Matrix inversion failed!\n");
        return;
    }
    
    // Compute ATb = A^T * b. We first treat b as a matrix.
    arm_matrix_instance_f32 b_mat;
    arm_mat_init_f32(&b_mat, NUM_EQUATIONS, 1, b_data);
    arm_mat_mult_f32(&AT, &b_mat, &ATb);
    
    // Compute x_ls = ATA_inv * ATb (initial estimate)
    arm_mat_mult_f32(&ATA_inv, &ATb, &x_ls);
    
    // Copy result into x_est array
    for (int i = 0; i < 3; i++) {
        x_est[i] = x_ls_data[i];
    }
}


float32_t distance3D(const float32_t x[3], const float32_t p[3])
{
    float32_t dx = x[0] - p[0];
    float32_t dy = x[1] - p[1];
    float32_t dz = x[2] - p[2];
    return sqrtf(dx*dx + dy*dy + dz*dz);
}


// Nonlinear Refinement using Gauss–Newton
//
// We want to solve for x that minimizes:
//    F(x) = sum_{i=1}^{NUM_hydrophoneS-1} ( d_i - d_0 - c*tdoa_i )^2,
// where d_i = || x - p_i || and p_0 is the reference hydrophone.

void nonlinearRefinement(const float32_t hydrophones[NUM_HYDROPHONES][3],
                         const float32_t tdoas[TDOA_DATA_LENGHT],
                         float32_t x_est[3], float32_t c)
{
    for (int iter = 0; iter < MAX_ITER; iter++) {
        // Residual vector (size: NUM_EQUATIONS)
        float32_t r[NUM_EQUATIONS];
        // Jacobian matrix (size: NUM_EQUATIONS x 3)
        float32_t J[NUM_EQUATIONS * 3];

        // Compute distance from the current estimate to the reference hydrophone (hydrophone 0)
        float32_t d0 = distance3D(x_est, hydrophones[0]);
        
        // For each hydrophone i = 1,...,NUM_hydrophoneS-1, compute residual and Jacobian row.
        for (int i = 0; i < NUM_EQUATIONS; i++) {
            float32_t di = distance3D(x_est, hydrophones[i+1]);
            // Residual: (di - d0) - c * tdoas[i]
            r[i] = (di - d0) - (c * tdoas[i]);
            
            // Compute the partial derivatives:
            // For hydrophone i: d(di)/dx = (x_est - p_i)/di, and similarly for d0 at hydrophone 0.
            for (int j = 0; j < 3; j++) {
                float32_t term1 = (x_est[j] - hydrophones[i+1][j]) / (di + 1e-6f);
                float32_t term2 = (x_est[j] - hydrophones[0][j]) / (d0 + 1e-6f);
                J[i*3 + j] = term1 - term2;
            }
        }
        
        // Now we want to compute the Gauss–Newton update:
        //   delta_x = -(J^T J)^{-1} J^T r.
        // First compute J^T (3 x NUM_EQUATIONS).
        float32_t JT[3 * NUM_EQUATIONS];
        for (int i = 0; i < NUM_EQUATIONS; i++) {
            for (int j = 0; j < 3; j++) {
                JT[j*NUM_EQUATIONS + i] = J[i*3 + j];
            }
        }
        
        // Compute J^T J: a 3x3 matrix.
        float32_t JTJ[9] = {0};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                float32_t sum = 0.0f;
                for (int k = 0; k < NUM_EQUATIONS; k++) {
                    sum += JT[i*NUM_EQUATIONS + k] * J[k*3 + j];
                }
                JTJ[i*3 + j] = sum;
            }
        }
        
        // Create CMSIS matrix structures for JTJ and its inverse.
        arm_matrix_instance_f32 matJTJ, invJTJ;
        float32_t invJTJ_data[9];
        arm_mat_init_f32(&matJTJ, 3, 3, JTJ);
        arm_mat_init_f32(&invJTJ, 3, 3, invJTJ_data);
        arm_status status = arm_mat_inverse_f32(&matJTJ, &invJTJ);
        if (status != ARM_MATH_SUCCESS) {
            Serial.println(invJTJ_data[0], 10);
            Serial.println("Gauss-Newton: Inversion of JTJ failed!\n");
            break;
        }
        
        // Compute J^T * r: result is 3x1 vector.
        float32_t JTr[3] = {0};
        for (int i = 0; i < 3; i++) {
            float32_t sum = 0.0f;
            for (int k = 0; k < NUM_EQUATIONS; k++) {
                sum += JT[i*NUM_EQUATIONS + k] * r[k];
            }
            JTr[i] = sum;
        }
        
        // Compute update delta = - inv(J^TJ) * (J^T * r)
        float32_t delta[3] = {0};
        for (int i = 0; i < 3; i++) {
            float32_t sum = 0.0f;
            for (int j = 0; j < 3; j++) {
                sum += invJTJ_data[i*3 + j] * JTr[j];
            }
            delta[i] = -sum;
        }
        
        // Update the current estimate.
        float32_t norm_delta = sqrtf(delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2]);
        for (int i = 0; i < 3; i++) {
            x_est[i] += delta[i];
        }
        
        // Check for convergence
        if (norm_delta < TOL) {
            break;
        }
    }
}



























Pos find_pinger(int hydrophone_value_array[NUM_HYDROPHONES][HYDROPHONE_DATA_SIZE], double TDOA[TDOA_DATA_LENGHT], const int sampling_frequency){
    for (int i = 0; i < TDOA_DATA_LENGHT; i++) {

        // Compute cross-correlation (convolution) for all possible lags
        int max_lag = 2 * HYDROPHONE_DATA_SIZE - 1;  // Total number of possible shifts
        int32_t correlation_array[max_lag];
        crosscorelation(hydrophone_value_array[0], hydrophone_value_array[i+1], HYDROPHONE_DATA_SIZE, correlation_array);

        // Find peak of the cross corelation, thats where TDOA is located
        int peek_index = find_peak_index(correlation_array, max_lag);
        

        // Calculate the TDOA in seconds
        // Explain here in TDOA why subtract by HYDROPHONE_DATA_SIZE_TESTING (Because cross corelation produces lag in positive and negative +- HYDROPHONE_DATA_SIZE_TESTING), we want it in time format to be corret thus we subtract
        double tdoa = ((float)(peek_index - HYDROPHONE_DATA_SIZE)/sampling_frequency);


        TDOA[i] = tdoa;
        Serial.print(tdoa, 10);Serial.print(", ");
    }

    // float32_t x_est[3] = {0, 0, 0};
    // float32_t f_tdoa[TDOA_DATA_LENGHT];
    // for (int i = 0; i < TDOA_DATA_LENGHT; i++){
    //     f_tdoa[i] = static_cast<float32_t>(TDOA[i]);
    // }


    // linearLeastSquaresEstimate(test_hydrophones, f_tdoa, x_est, 1500);

    // nonlinearRefinement(test_hydrophones, f_tdoa, x_est, 1500);

    // return Pos(x_est[0], x_est[1], x_est[2]);

    Pos pinger = tdoa_multilateration(hydrophonePositions, TDOA);
    Serial.println("Calculated: ");
    pinger.display();
    return pinger;
}





Pos::Pos(std::vector<float32_t> pos): x(pos.at(0)), y(pos.at(1)), z(pos.at(2)){
    arm_sqrt_f32(x*x+y*y+z*z, &dist);
}


float32_t Pos::dist_to(const Pos& other){
    float32_t result;
    arm_sqrt_f32((other.x - x)*(other.x - x) + (other.y - y)*(other.y - y) + (other.z - z)*(other.z - z), &result);
    return result;
}



void Pos::display(){
    Serial.println();
    Serial.println("Position info: ");
    Serial.print("x: ");Serial.print(x, 5);Serial.print(", y: "); Serial.print(y, 5);Serial.print(", z: "); Serial.print(z, 5); Serial.print(", distance: "); Serial.print(dist, 5);
    Serial.println();
    Pos dir = (*this).direction();
    Serial.println("Direction: ");
    Serial.print("x: ");Serial.print(dir.x, 5);Serial.print(", y: "); Serial.print(dir.y, 5);Serial.print(", z: "); Serial.print(dir.z, 5); Serial.print(", distance: "); Serial.println(dir.dist);
    Serial.println();
}}