#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

/// @brief to activate the clock
namespace clock
{
    extern uint32_t PITclockFreq; //// init in the .cpp but no values given !!!
    void dumpClockRegisters();
    void setup();

    // clockcycles for microseconds to use in the PITs
    uint32_t get_clockcycles_micro(uint32_t microseconds);
    // clockcycles for nanoseconds to use in the PITs
    uint32_t get_clockcycles_nano(uint32_t nanoseconds);
};
