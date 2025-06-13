/*
    This is the header file for the Periodic Interrupt Timers (PIT)
    It is usefull if periodic interrupts are needed, and it can be linked to the first 4 DMA channels
    Datasheet Chapter 53, from page 2971
    Each timer can be controlled independantly, the timer starts at a value LDVAL and counts down to 0
*/

#ifndef PIT_H
#define PIT_H

#include <stddef.h>
#include <stdint.h>
#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "PERI_PIT.h"


#ifdef __cplusplus

extern "C"{



#endif // __cplusplus


typedef void (*void_function_ptr)(void);
typedef enum PIT_channels { PIT_0, PIT_1, PIT_2, PIT_3 } PIT_channels;

void pit_setup();

// general
void setUpPeriodicISR(void_function_ptr function, uint32_t clockcycles, uint8_t PIT_number); 
void startPeriodic(uint8_t PIT_number, uint8_t chained); 
void stopPeriodic(uint8_t PIT_number); 


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !PIT_H
