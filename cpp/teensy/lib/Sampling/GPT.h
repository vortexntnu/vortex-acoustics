/*
    This is the header file to control the General Purpuse Timer (GPT)
    There is only one timer of this kind on the Teensy 4.1
    Datasheet: Chapter 52 (registers: p2957)

    TODO: (when time to do it or needed)
    - function to enable all interrupts independantly (there are 6)
    - Input capture
    - using compare register 2 and 3
    - the datasheet is unclear about the number of GPTs, the text says 1, the register definitions 2
*/

#include "Arduino.h"
#include "clock.h"
#include "imxrt.h"

/// this was created to be used for delays in the nanoseconds range, but did't work reliably. Probably because the time of execution of the different lines where not compensated. ---->
/// USE the inbuilt function delaynanoseconds INSTEAD !!!!!!!!!!!!!!!!!!!!!!!!!! -----------------------------

/// @brief GENERAL PURPOSE TIMER
namespace gpt
{
    extern int waiting;

    typedef void (*void_function_ptr)(void);

    void setup();
    void startTimer(int clockcycles);
    ////void ISR(void); /// should not be called from outside!!
    void setUpGptISR(void_function_ptr function);
};
