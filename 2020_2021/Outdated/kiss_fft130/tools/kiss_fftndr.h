#ifndef KISS_NDR_H
#define KISS_NDR_H

#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "kiss_fftnd.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct kiss_fftndr_state* kiss_fftndr_cfg;

/*
 dims[0] must be even

 If you don't care to allocate space, use mem = lenmem = NULL 
*/
kiss_fftndr_cfg  kiss_fftndr_alloc(
        const int* dims, 
        int ndims, 
        int inverse_fft, 
        void* mem, 
        size_t* lenmem);

/*
 input timedata has dims[0] X dims[1] X ... X  dims[ndims-1] scalar points
 output freqdata has dims[0] X dims[1] X ... X  dims[ndims-1]/2+1 complex points
*/
void kiss_fftndr(
        kiss_fftndr_cfg cfg,
        const kiss_fft_scalar* timedata,
        kiss_fft_cpx *freqdata);

/*
 input and output dimensions are the exact opposite of kiss_fftndr
*/
void kiss_fftndri(
        kiss_fftndr_cfg cfg,
        const kiss_fft_cpx *freqdata,
        kiss_fft_scalar *timedata);


#define kiss_fftr_free free

#ifdef __cplusplus
}
#endif

#endif
