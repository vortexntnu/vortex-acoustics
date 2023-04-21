#include "gpio.h"

#include "Arduino.h"

namespace gpio
{

    /*
        Basic explanations:
        By including core_pins.h in the header file, we have access to register names of Teensy

    */

    /** @brief set pin to input or output mode. 1->output. 0->input.
     * @param pin: should be of type CORE_PINX_BIT, i.e. the pins number on the teensy.
     * @param GPIO_n: The GPIO port of the pin
     */
    void configPin(int pin, int mode, IMXRT_GPIO_t &GPIO_n)
    {
        // set to output mode.
        if (mode)
        {
            GPIO_n.GDIR |= (1 << pin);
        }
        // set to input mode.
        else
        {
            GPIO_n.GDIR &= !(1 << pin);
        }
    }

    void configPort(IMXRT_GPIO_t &GPIO_n, uint32_t reg_val, uint32_t mask = 0xFFFFFFFF)
    {
        // keeping the values we mask, and overwriting the other
        GPIO_n.GDIR = (GPIO_n.GDIR & ~mask) | (reg_val & mask);
    }

    /**
     * @brief By default, Arduino sets pin to fast mode. Pins are configured to normal speed.
     * Only GPIO ports 1-5 can then be used
     * See page 375 for register description
     * @param mask which pins are configured
     * @param GPIO_n The GPIO port of the pin, can be the normal or fast register
     */
    void set_normal_GPIO(uint32_t mask, IMXRT_GPIO_t &GPIO_n)
    {
        if (&GPIO_n == IMXRT_GPIO1_ADDRESS || &GPIO_n == IMXRT_GPIO6_ADDRESS)
        {
            IOMUXC_GPR_GPR26 &= ~mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO2_ADDRESS || &GPIO_n == IMXRT_GPIO7_ADDRESS)
        {
            IOMUXC_GPR_GPR27 &= ~mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO3_ADDRESS || &GPIO_n == IMXRT_GPIO8_ADDRESS)
        {
            IOMUXC_GPR_GPR28 &= ~mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO4_ADDRESS || &GPIO_n == IMXRT_GPIO9_ADDRESS)
        {
            IOMUXC_GPR_GPR29 &= ~mask; // 0xFFFFFFFF to use fast GPIO
        }
    }
    /**
     * @brief  Pins are configured to fast speed.
     * Only GPIO ports 6-9 can then be used
     * By default, Arduino sets pin to fast mode anyway.
     * See page 375 for register description
     * @param mask which pins are configured
     * @param GPIO_n The GPIO port of the pin, can be the normal or fast register
     */
    void set_fast_GPIO(uint32_t mask, IMXRT_GPIO_t &GPIO_n)
    {
        if (&GPIO_n == IMXRT_GPIO1_ADDRESS || &GPIO_n == IMXRT_GPIO6_ADDRESS)
        {
            IOMUXC_GPR_GPR26 |= mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO2_ADDRESS || &GPIO_n == IMXRT_GPIO7_ADDRESS)
        {
            IOMUXC_GPR_GPR27 |= mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO3_ADDRESS || &GPIO_n == IMXRT_GPIO8_ADDRESS)
        {
            IOMUXC_GPR_GPR28 |= mask; // 0xFFFFFFFF to use fast GPIO
        }
        else if (&GPIO_n == IMXRT_GPIO4_ADDRESS || &GPIO_n == IMXRT_GPIO9_ADDRESS)
        {
            IOMUXC_GPR_GPR29 |= mask; // 0xFFFFFFFF to use fast GPIO
        }
    }

    /** @brief reads the physical pin value
     * @param pin: should be of type CORE_PINX_BIT, i.e. the pins number on the teensy.
     * @param GPIO_n: The GPIO port of the pin
     * @return 0 if LOW and 1 if HiGH on the pin
     */
    uint8_t read_pin(int pin, IMXRT_GPIO_t &GPIO_n)
    {
        return (GPIO_n.PSR >> pin) & 0b1;
    }

    /** @brief sets the output of the pin as desired
     * @param pin should be of type CORE_PINX_BIT, i.e. the pins number on the teensy.
     * @param value the value to set to. 0 -> LOW and 1 -> HIGH
     * @param GPIO_n The GPIO port of the pin
     */
    void write_pin(int pin, uint8_t value, IMXRT_GPIO_t &GPIO_n)
    {
        if (value)
        {
            GPIO_n.DR_SET |= (1 << pin); // the MCU has registers to set or clear registers
        }
        else
        {
            GPIO_n.DR_CLEAR |= (1 << pin);
        }
    }

    void toggle_pin(int pin, IMXRT_GPIO_t &GPIO_n)
    {
        // same as above, there are registers (.DR_TOGGLE) that will toggle the bits where a 1 is written
        GPIO_n.DR_TOGGLE |= (1 << pin);
    }
    /** @brief writes values to a complete GPIO port
     * @param reg_value which pin should be set and which cleared, 32-bit
     * @param GPIO_n The GPIO port of the pin
     * @param mask mask if not all of the pins should be changed.
     * 1 means the pin will be set to value from @reg_value
     */
    void write_port(uint32_t reg_value, IMXRT_GPIO_t &GPIO_n, uint32_t mask)
    {
        GPIO_n.DR_SET = reg_value & mask;
        GPIO_n.DR_CLEAR = ~reg_value & mask; // flipping bits to set the bits that needs to be cleared
    }

    uint32_t read_port(IMXRT_GPIO_t &GPIO_n)
    {
        return GPIO_n.PSR;
    }

    // debug function
    void dump_GPIO_registers()
    {
        Serial.printf("GPIO6\nGDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO6.GDIR, IMXRT_GPIO6.DR);
        Serial.printf("GPIO7\nGDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO7.GDIR, IMXRT_GPIO7.DR);
    }

} // namespace gpio