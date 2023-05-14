#include "GPT.h"
#ifdef SERIAL_DEBUG
#include "gpio.h"
#endif

// GENERAL PURPOSE TIMER SETUP, READ ON PAGE 2945-2969

namespace gpt
{

    void (*isr_timer_func)(void);

    void ISR(void)
    {

        NVIC_DISABLE_IRQ(IRQ_GPT1);
        if (GPT1_SR & GPT_SR_OF1)
        {

            GPT1_CR &= ~GPT_CR_EN; // Turns off timer
            GPT1_SR |= GPT_SR_OF1;
            (*isr_timer_func)();
        }

        NVIC_ENABLE_IRQ(IRQ_GPT1);
    }

    void setup()
    {

        // Prescaler setup (We want to turn to divide by 1)
        uint32_t prescalerMask = 0xFFFF;
        GPT1_PR &= ~prescalerMask;

        // Control register setup
        uint32_t clearCLKSRCMask = 0x1C0;
        GPT1_CR &= ~GPT_CR_FRR; // Sets to restart mode

        GPT1_CR &= ~clearCLKSRCMask;
        GPT1_CR |= GPT_CR_CLKSRC(0x1);

        GPT1_CR |= GPT_CR_ENMOD;
        // Enables GPT1 module and resetting of counter register after resetting EN bit (0th-bit)

        GPT1_IR |= GPT_IR_OF1IE;

        attachInterruptVector(IRQ_GPT1, ISR); /// ISR() is the interupt routine specific to this namespace (--> gpd::ISR())
        NVIC_ENABLE_IRQ(IRQ_GPT1);            /// activating interupt management for this interrupt
    }

    void startTimer(int clockcycles)
    {
        GPT1_OCR1 = clockcycles; // Sets compare register value
#ifdef SERIAL_DEBUG
        gpio::toggle_pin(CORE_PIN32_BIT, IMXRT_GPIO7);
#endif
        GPT1_CR |= GPT_CR_EN; // Turns on timer
    }

    void setUpGptISR(void_function_ptr function)
    {
        isr_timer_func = function; /// sets function that will be called when timer finished
    }
};
