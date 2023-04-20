#include "imxrt.h"
#include "stdlib.h"
#include <Arduino.h>

/// Configuring and handling interrupts on pins 28, 30 and the one from BUSY of the adc

namespace gpioInterrupt {

#define BUSY CORE_PIN31_BIT // GPIO 9

typedef void (*void_function_ptr)(void);

extern volatile uint32_t triggeredPin;

extern uint32_t inputPinsMask;
extern uint32_t ICR2activeHighMask;
extern uint32_t ICR2clearMask;

void setup();

////void ISR(void);     /// should not be called from outside!!

void setUpGpioISR(void_function_ptr function);

void configPin(int pin, int mode); /// not implemented

void dump_GPIO_interrupt_registers();
};