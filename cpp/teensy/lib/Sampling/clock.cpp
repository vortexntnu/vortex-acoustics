#include "clock.h"

namespace clock
{
    uint32_t PITclockFreq;

#ifdef SERIAL_DEBUG
    void dumpClockRegisters()
    {
        Serial.printf("CCM_CBCMR: 0x%X\r\n", CCM_CBCMR);
        Serial.printf("CCM_CBCDR: 0x%X\r\n", CCM_CBCDR);
        Serial.printf("CCM_CSCMR1: 0x%X\r\n", CCM_CSCMR1);
    }
#endif

    /// this was to slow down the systemclock to be able to analyse the data with the oscilloscope at the office
    /// might need to be checked again
    /// nice graphic p 1016

    void setup()
    {

#ifndef OSCILLOSCOPE_DEBUG
        // Setup Peripheral Clock
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(3); // Using PPL2 as source clock (~600MHz in theory) (~528MHz in practice)
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL; // Select clock from PRE_PERIPH_CLK_SEL
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0);     // divide /1 -> AHB Clock
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3);     // divide /4 (Unable to change -Christian) (~150Mhz at this point)
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1

        PITclockFreq = 150000000;
// Clock: 150 MHz -> Period: 6.667 ns (IN THEORY)
#endif

#ifdef OSCILLOSCOPE_DEBUG
        // Setup Peripheral Clock
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(3); // Using PPL2 as source clock (~600MHz IN THEORY) (~528MHz IN PRACTICE)
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(2); // divide /3 -> AHB Clock (~200Mhz at this point)
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4 (Unable to change -Christian) (~50Mhz at this point)
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 |= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
// Clock: 50 MHz -> Period: 20 ns
// PPL2 is totally divided by 12 instead of 4
// Also, this is 3 times slower than "max" possible frequency
#endif
    }

    uint32_t get_clockcycles_micro(float microseconds)
    {
        return microseconds * (PITclockFreq / 1000000.0);
    }

    uint32_t get_clockcycles_nano(float nanoseconds)
    {
        return nanoseconds * (PITclockFreq / 1000000000.0);
    }

}; // namespace clock
