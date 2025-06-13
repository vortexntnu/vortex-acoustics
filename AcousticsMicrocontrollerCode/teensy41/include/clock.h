#ifndef CLOCK_H
#define CLOCK_H


#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "PERI_CCM.h"
#include <stdint.h>
#ifdef SERIAL_DEBUG
#endif

/// @brief to activate the clock_ADC
extern uint32_t PITclockFreq; //// init in the .cpp but no values given !!!
void dumpClockRegisters();
void clock_setup();

// clockcycles for microseconds to use in the PITs
uint32_t get_clockcycles_micro(float microseconds);
// clockcycles for nanoseconds to use in the PITs
uint32_t get_clockcycles_nano(float nanoseconds);

#endif
