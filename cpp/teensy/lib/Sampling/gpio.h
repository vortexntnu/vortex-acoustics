#pragma once
#include "core_pins.h"

namespace gpio
{
    void configPin(int pin, int mode, IMXRT_GPIO_t &GPIO_n);
    void configPort(IMXRT_GPIO_t &GPIO_n, uint32_t reg_val, uint32_t mask = 0xFFFFFFFF);

    void set_normal_GPIO(uint32_t mask, IMXRT_GPIO_t &GPIO_n);
    void set_fast_GPIO(uint32_t mask, IMXRT_GPIO_t &GPIO_n);

    uint8_t read_pin(int pin, IMXRT_GPIO_t &GPIO_n);
    void write_pin(int pin, uint8_t value, IMXRT_GPIO_t &GPIO_n);
    void toggle_pin(int pin, IMXRT_GPIO_t &GPIO_n);

    void write_port(uint32_t reg_value, IMXRT_GPIO_t &GPIO_n, uint32_t mask = 0xFFFFFFFF);
    uint32_t read_port(IMXRT_GPIO_t &GPIO_n);

    /// prints values of registers IMXRT_GPIO6.GDIR, IMXRT_GPIO6.DR and IMXRT_GPIO7.GDIR, IMXRT_GPIO7.DR
    void dump_GPIO_registers();

} // namespace gpio