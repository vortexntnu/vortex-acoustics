#pragma once
#include "arm_math.h"


namespace Multilateration{

int find_peak_index(int32_t* signal, int size);

void crosscorelation(const int* signal1, const int* signal2, int n, int* result); // working example, but slower then the arm one

void crosscorelation_2(const int* x, const int* y, const int size, int32_t* result); // for testing

void arm_correlation(const int *pSrcA_int, uint32_t srcALen, const int *pSrcB, uint32_t srcBLen, int32_t *pDst_int);

}