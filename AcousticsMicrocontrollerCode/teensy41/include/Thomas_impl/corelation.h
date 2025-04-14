
#include <vector>
#include "arm_math.h"




std::vector<float32_t> still_brute_force_but_better_best_crosscorelation_lag(std::vector <float32_t> x, std::vector <float32_t> y);
int find_peak_index(int32_t* signal, int size);

void crosscorelation(const int* signal1, const int* signal2, int n, int* result);


void crosscorelation_2(const int* x, const int* y, const int size, int32_t* result);

void arm_correlation(const int *pSrcA_int, uint32_t srcALen, const int *pSrcB, uint32_t srcBLen, int32_t *pDst_int);
