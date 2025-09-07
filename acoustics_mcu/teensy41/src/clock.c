#include "clock.h"



void clock_init() {

// #ifndef OSCILLOSCOPE_DEBUG
    // Setup Peripheral Clock
    CCM->CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(3);
    // derive Peripheral from Pre-Peripheral Clock
    CCM->CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL(3); // Select clock from PRE_PERIPH_CLK_SEL
    CCM->CBCDR |= CCM_CBCDR_AHB_PODF(0);     // divide /1 -> AHB Clock
    CCM->CBCDR |= CCM_CBCDR_IPG_PODF(3);     // divide /4 (Unable to change -Christian) (~150Mhz at this point)
    CCM->CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL(3);
    CCM->CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1

// Clock: 150 MHz -> Period: 6.667 ns (IN THEORY)
// #endif
}

uint32_t get_clockcycles_micro(float microseconds) { return microseconds * (CPU_CLOCK / 1000000.0); }

uint32_t get_clockcycles_nano(float nanoseconds) { return nanoseconds * (CPU_CLOCK / 1000000000.0); }

