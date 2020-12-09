#ifndef KISS_FTR_H
#define KISS_FTR_H

#include "kiss_fft.h"
#ifdef __cplusplus
extern "C" {
#endif
    
/* 
    Real optimized version can save about 45% cpu time vs. complex fft of a real seq. 
*/
typedef struct kiss_fftr_state* kiss_fftr_cfg;

/*
    nfft must be even

    If you don't care to allocate space, use mem = lenmem = NULL 
*/
kiss_fftr_cfg kiss_fftr_alloc(
    int nfft,
    int inverse_fft,
    void * mem, 
    size_t * lenmem);

/*
    Input timedata has nfft scalar points
    Output freqdata has nfft/2+1 complex points
*/
void kiss_fftr(
    kiss_fftr_cfg cfg,
    const kiss_fft_scalar* timedata,
    kiss_fft_cpx* freqdata);

/*
    Input freqdata has  nfft/2+1 complex points
    Output timedata has nfft scalar points
*/
void kiss_fftri(
    kiss_fftr_cfg cfg,
    const kiss_fft_cpx* freqdata,
    kiss_fft_scalar* timedata);

#define kiss_fftr_free free

#ifdef __cplusplus
}
#endif
#endif
