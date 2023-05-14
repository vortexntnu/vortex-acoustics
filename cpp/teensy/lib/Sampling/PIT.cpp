#include "PIT.h"

#include <Arduino.h>

// ! It is possible to give priorities to interrupts in case the timer interrrupts are critical
// see in intervalTimer.cpp (library for PIT)
// #include <IntervalTimer.h>
namespace PIT
{
    /// will be changed later, this keeps it generic
    static void dummyISR(void)
    {
        Serial.println("Ahhh");
    }

    // inspired from <IntervalTimer.h>, makes it possible to have one general function for 4 timers
    IMXRT_PIT_CHANNEL_t *PIT[4] = {IMXRT_PIT_CHANNELS, IMXRT_PIT_CHANNELS + 1, IMXRT_PIT_CHANNELS + 2, IMXRT_PIT_CHANNELS + 3};

    static void (*isr_funct_table[4])(void) __attribute((aligned(32))) = {dummyISR, dummyISR, dummyISR, dummyISR};

#ifdef SERIAL_DEBUG
    void
    dumpPeriodicRegisters()
    {
        Serial.printf("PIT.MCR: 0x%X\r\n", PIT_MCR);
        Serial.printf("PIT.LDVAL0: 0x%X\r\n", PIT_LDVAL0);
        Serial.printf("PIT.LDVAL1: 0x%X\r\n", PIT_LDVAL1);
        Serial.printf("PIT.LDVAL2: 0x%X\r\n", PIT_LDVAL2);
        Serial.printf("PIT.LDVAL3: 0x%X\r\n", PIT_LDVAL3);
        Serial.printf("PIT.TCTRL0: 0x%X\r\n", PIT_TCTRL0);
        Serial.printf("PIT.TCTRL1: 0x%X\r\n", PIT_TCTRL1);
        Serial.printf("PIT.TCTRL2: 0x%X\r\n", PIT_TCTRL2);
        Serial.printf("PIT.TCTRL3: 0x%X\r\n", PIT_TCTRL3);
    }
#endif

    void ISR()
    {
        NVIC_DISABLE_IRQ(IRQ_PIT);
        if (PIT_TFLG0 && isr_funct_table[0] != nullptr)
        { // bit 0 of this register: interupt pending
            PIT_TFLG0 = 0x1;
            // Serial.println("0");
            //(*isr_periodic_func3)();
            (*isr_funct_table[0])();
        }
        // making sure it is not an empty pointer that would crash the programm
        else if (PIT_TFLG3 && isr_funct_table[3] != nullptr)
        { // bit 0 of this register: interupt pending
            PIT_TFLG3 = 0x1;
            //(*isr_periodic_func3)();
            (*isr_funct_table[3])();
        }

        else if (PIT_TFLG2 && isr_funct_table[2] != nullptr)
        { // bit 0 of this register: interupt pending
            PIT_TFLG2 = 0x1;
            //(*isr_periodic_func3)();
            (*isr_funct_table[2])();
        }

        else if (PIT_TFLG1 && isr_funct_table[1] != nullptr)
        { // bit 0 of this register: interupt pending
            PIT_TFLG1 = 0x1;
            //(*isr_periodic_func3)();
            // Serial.println("1");
            (*isr_funct_table[1])();
        }

        NVIC_ENABLE_IRQ(IRQ_PIT);
    }

    void setup()
    {
        PIT_MCR &= ~PIT_MCR_MDIS; // activates clock for periodic timers

        // PIT can only be used through this interface, because the ISR is defined here
        // it is anyway very unlikely that some library uses PIT (exept for DMA, be careful)
        PIT_TCTRL0 = 0x0; // no chainmode, interrupt and timer disabled (p 2985)
        PIT_TCTRL1 = 0x0;
        PIT_TCTRL2 = 0x0;
        PIT_TCTRL3 = 0x0;

        // all PIT interrupts are grouped into one IRQ
        attachInterruptVector(IRQ_PIT, ISR);
        NVIC_ENABLE_IRQ(IRQ_PIT); /// is activating the interrupt management for IRQ_PIT
#ifdef SERIAL_DEBUGs
        dumpPeriodicRegisters();
#endif
    }

    // info: usefull info from datasheet: MCR[FRZ] can freeze the timers to debug. Datasheet page 2975
    //  these timers run until stopped

    //* ----------- general function, generelized for all 4 timers ---------------

    // setting up timer, so that a call to startPeriodic is enought (chained has to be set there if needed)
    void setUpPeriodicISR(void_function_ptr function, uint32_t clockcycles, PIT_channels PIT_number)
    {
        if (PIT_number > 3)
            return;
        isr_funct_table[PIT_number] = function;
        PIT[PIT_number]->LDVAL = clockcycles;
    }

    void setUpPeriodicISR(void_function_ptr function, PIT_channels PIT_number)
    {
        if (PIT_number > 3)
            return;
        isr_funct_table[PIT_number] = function;
    }

    void startPeriodic(void_function_ptr ISR_func, uint32_t clockcycles, PIT_channels PIT_number, uint8_t chained = 0)
    {
        if (PIT_number > 3)
            return;
        isr_funct_table[PIT_number] = ISR_func;
        startPeriodic(clockcycles, PIT_number, chained);
    }

    void startPeriodic(uint32_t clockcycles, PIT_channels PIT_number, uint8_t chained = 0)
    {
        if (PIT_number > 3)
            return;

        PIT[PIT_number]->LDVAL = clockcycles;
        startPeriodic(PIT_number, chained);
        // PIT[PIT_number]->TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
    }

    void startPeriodic(PIT_channels PIT_number, uint8_t chained = 0)
    {
        PIT[PIT_number]->TCTRL = (chained ? PIT_TCTRL_CHN | PIT_TCTRL_TEN | PIT_TCTRL_TIE : PIT_TCTRL_TEN | PIT_TCTRL_TIE);
    }

    void stopPeriodic(PIT_channels PIT_number)
    {
        if (PIT_number > 3)
            return;
        PIT[PIT_number]->TCTRL &= ~(PIT_TCTRL_TIE | PIT_TCTRL_TEN);
    }

} // namespace PIT