/*
    This is the header file for the Periodic Interrupt Timers (PIT)
    It is usefull if periodic interrupts are needed, and it can be linked to the first 4 DMA channels
    Datasheet Chapter 53, from page 2971
    Each timer can be controlled independantly, the timer starts at a value LDVAL and counts down to 0
*/

#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

namespace PIT
{

    typedef void (*void_function_ptr)(void);
    typedef enum PIT_channels
    {
        PIT_0,
        PIT_1,
        PIT_2,
        PIT_3
    } PIT_channels;

    void setup();

    // general
    void setUpPeriodicISR(void_function_ptr function, uint32_t clockcycles, PIT_channels PIT_number);
    void setUpPeriodicISR(void_function_ptr function, PIT_channels PIT_number);
    void startPeriodic(void_function_ptr ISR_func, uint32_t clockcycles, PIT_channels PIT_number, uint8_t chained = 0);
    void startPeriodic(uint32_t clockcycles, PIT_channels PIT_number, uint8_t chained = 0);
    void startPeriodic(PIT_channels PIT_number, uint8_t chained = 0);
    void stopPeriodic(PIT_channels PIT_number);
} // PIT