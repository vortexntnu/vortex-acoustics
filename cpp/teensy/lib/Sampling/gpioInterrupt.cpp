#include "gpioInterrupt.h"
#include "core_pins.h"

namespace gpioInterrupt {

volatile uint32_t triggeredPin; //// apparently not used
// uint32_t inputPinsMask = 0xC40000; //Activate GPIO8 pin 18, 22, 23
uint32_t inputPinsMask = ((0x1 << CORE_PIN28_BIT) | (0x1 << CORE_PIN30_BIT) | (0x1 << BUSY));
// uint32_t ICR2activeHighMask = 0xA020;
uint32_t configMask = (((2 << (2 * (BUSY % 16)))) | ((2 << (2 * (CORE_PIN28_BIT % 16)))) | ((2 << (2 * (CORE_PIN30_BIT % 16))))); //// wtf?
uint32_t ICR2clearMask = ~(0xF030);

// uint32_t intConfig = (2 << 12);//((2 << (2 * (BUSY % 16)))); // set interrupt pin to be rising edge sensitive.

void (*isr_convert_func)(void); /// function pointer, initialized in function setUpGpioISR. Why not using type void_function_ptr?

void ISR(void) /// this function is linked to interrupt IRQ_GPIO6789 (see below)
{

    NVIC_DISABLE_IRQ(IRQ_GPIO6789); /// the interrupt that trigerred this interrupt

    if ((0x1 << CORE_PIN28_BIT) & GPIO8_ISR) {
        GPIO8_ISR &= 0x1 << CORE_PIN28_BIT; /// for what purpose? maybe setting a 0 to clear interrupt?
        Serial.print("Pin 28 detected a signal!\n");
    }
    if ((0x1 << CORE_PIN30_BIT) & GPIO8_ISR) {
        GPIO8_ISR &= 0x1 << CORE_PIN30_BIT;
        Serial.print("Pin 30 detected a signal!\n");
    }

    if ((0x1 << BUSY) & GPIO8_ISR) /// if busy pins goes high again, conversion is finished and we can read the values.
    {
        GPIO8_ISR &= 0x1 << BUSY;
#ifdef SERIAL_DEBUG
        Serial.print("ADC finished converting\n");
#endif
        (*isr_convert_func)(); /// this is adc::readLoop() if adc::triggerConversion() has been called
        //// maybe need a check if null_ptr!
    }

    NVIC_ENABLE_IRQ(IRQ_GPIO6789);
}

void setUpGpioISR(void_function_ptr function)
{
    isr_convert_func = function;
}

void setup()
{
#ifdef SERIAL_DEBUG
    Serial.printf("Setting up GPIO interrupts\n");
#endif
    GPIO8_GDIR &= ~inputPinsMask; // Use inputPinsMask to mark pins as input

    GPIO8_IMR = inputPinsMask; // Use inputPinsMask to active interrups on chosen pins

    /// maybe need to specify the interrupt type

    // GPIO8_ICR2 &= ICR2clearMask; // Use ICR2clearMask to clear out currently existing values in chosen pins
    // GPIO8_ICR2 |= configMask; // Use ICR2activeHighMask to mark pins as rising edge interrupt trigger
    GPIO8_ICR2 = configMask; //// is this setting the interrupt mode?

    GPIO8_DR_CLEAR = 0xFFFFFFFF; // Clear current content in data register

#ifdef SERIAL_DEBUG
    dump_GPIO_interrupt_registers();
#endif

    attachInterruptVector(IRQ_GPIO6789, ISR); /// linking interrupt to fonction gpioInterrupt::ISR()
    /// what interrupt??

#ifdef SERIAL_DEBUG
    Serial.printf("Enabling interrupts..\n");
#endif
    NVIC_ENABLE_IRQ(IRQ_GPIO6789);

#ifdef SERIAL_DEBUG
    Serial.printf("Setup complete\n");
#endif
}

/// prints registers GDIR, IMR and ICR2 --> need to be checked what exactly is in there
void dump_GPIO_interrupt_registers()
{
    Serial.printf("GDIR: 0x%X\nIMR: 0x%X\nICR2: 0x%X\n", GPIO8_GDIR, GPIO8_IMR, GPIO8_ICR2);
}

}; // gpioInterrupt