#ifndef CLOCK_H
#define CLOCK_H

#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "PERI_CCM.h"
#include <stdint.h>

#define CPU_CLOCK 150000000

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus


/**
 *@brief Initializes clock
 */
void clock_init();

/**
*@brief Converts microseconds to clockcycles
*@param microseconds time in microseconds
*@return time in clockcycles
*/
uint32_t get_clockcycles_micro(float microseconds);

/**
*@brief Converts nanoseconds to clockcycles
*@param nanoseconds time in microseconds
*@return time in clockcycles
*/
uint32_t get_clockcycles_nano(float nanoseconds);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
