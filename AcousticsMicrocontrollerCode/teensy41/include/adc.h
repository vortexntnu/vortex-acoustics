#ifndef ADC_H
#define ADC_H


#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "PERI_GPIO.h"
#include <stddef.h>

#define N_CHANNELS 8 // no. of channels on the ADC
#define N_HYDROPHONES 5
#define SAMPLE_SIZE 12 // bits in one sample

typedef enum { BLOCKING, TIMER, DMA } ADC_sample_mode;

/*
        *Pin definition of Control signals
        The CORE_PINX_BIT define is the offset in the GPIO register. For example CORE_PIN39_BIT
        and CORE_PIN34_BIT are 29, but they are on a different GPIO port (1 and 2, respectively)
        To access the right control and output registers we need the corresponding port.
        GPIO1 and GPIO6 are the same port, GPIO6 operates the pins in fast mode //!(not compatible with DMA)
        Same with GPIO2 and GPIO7 and so on.
        *see chapter about IO, chapter 10, 11 and especially 12 from datasheet
    */
// GPIO port+bit is commented
//! if pin is reallocated, change here the port. Never use directly the port
#define STBY 4 // GPIO 4.4
#define STBY_GPIO_PORT_NORMAL GPIO9
#define RESET  5 // GPIO 4.5
#define RESET_GPIO_PORT_NORMAL GPIO9
#define _RD  6 // GPIO 4.6
#define _RD_GPIO_PORT_NORMAL GPIO9
#define  _CS  2 // GPIO 2.2
#define _CS_GPIO_PORT_NORMAL GPIO7
#define PARSER 1 // GPIO 2.1
#define PARSER_GPIO_PORT_NORMAL GPIO7
#define REFEN  8 // GPIO 4.8
#define _WR  8   // same pin as REFEN, depends if PAR or SER
#define REFEN_GPIO_PORT_NORMAL GPIO9
#define _WR_GPIO_PORT_NORMAL GPIO9
#define HWSW  7 // GPIO 4.07
#define HWSW_GPIO_PORT_NORMAL GPIO9
#define CONVST  29 // GPIO 2.29
#define CONVST_GPIO_PORT_NORMAL GPIO7
#define ASLEEP  28 // GPIO 2.28
#define ASLEEP_GPIO_PORT_NORMAL GPIO7
#define BUSYINT  18 // GPIO 2.18
#define BUSYINT_GPIO_PORT_NORMAL GPIO7
#define BUSYINT_ARDUINO_PIN 36    // pin number to use for built-in arduino libraries
#define RANGE 19 // GPIO 2.19
#define XCLK 19  // same pin as RANGE, depending if HW or SW
#define RANGE_GPIO_PORT_NORMAL GPIO2
#define XCLK_GPIO_PORT_NORMAL GPIO2

// the port of the DB pins is GPIO port 1 (DMA needs the slower ports)
#define DB_GPIO_PORT_NORMAL GPIO1
// #define DB_GPIO_PORT_NORMAL GPIO6

// Timing definitions

// Configuration of the ADC control register
#define CONFIG_WRITE_EN 31
#define CONFIG_READ_EN 30
#define CONFIG_CLKSEL 29
#define CONFIG_CLKOUT 28
#define CONFIG_BUSYINT 27
#define CONFIG_BUSY_POL 26
#define CONFIG_STBY 25
#define CONFIG_RANGE_A 24
#define CONFIG_RANGE_B 23
#define CONFIG_PD_B 22
#define CONFIG_RANGE_C 21
#define CONFIG_PD_C 20
#define CONFIG_RANGE_D 19
#define CONFIG_PD_D 18
#define CONFIG_REFEN 15
#define CONFIG_REFBUF 14
#define CONFIG_VREF 13
#define CONFIG_REFDAC 0

// * TIMINGS -----------
// in nanoseconds !!
#define T_RDL 20
#define T_RDH 2
#define T_PDDO 15 // max
#define T_WRL 15
#define T_WRH 10

#define MIN_SAMP_PERIOD_BLOCKING 2.3
#define MIN_SAMP_PERIOD_TIMER 11
#define MIN_SAMP_PERIOD_DMA 10

// * new way
#define SAMPLE_LENGTH_ADC 1024
#define BUFFER_PER_CHANNEL 6 // 6 is max. (space = BUFFER_PER_CHANNEL * SAMPLE_LENGTH_ADC * hydrophones * (datatype_size in bytes) = 6*1024*5*16/8 = 61_440. This must be less than 64_000 = 64 kilo bytes)
#define RAW_HYDROPHONE_SIZE (SAMPLE_LENGTH_ADC * BUFFER_PER_CHANNEL)
typedef int32_t time_buff_3_1024[BUFFER_PER_CHANNEL][SAMPLE_LENGTH_ADC];

#define SAMPLING_TIMEOUT 10000 // [ms]
#define SAMPLE_PERIOD 2.4

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus


extern time_buff_3_1024 timestamps;

extern int16_t* samples_raw_hydrophones[5];

volatile extern uint8_t active_buffer; // to know which one is being filled, [0, BUFFER_PER_CHANNEL-1]
volatile extern size_t sample_index;   // To know what ring buffer memory we are in [0, SAMPLE_LENGTH_ADC-1]
volatile extern uint16_t buffer_filled;
volatile extern uint32_t overall_buffer_count;

void adc_init(); // inits pins

void adc_start_conversion(float sample_period_us, ADC_sample_mode sample_mode); // setup periodic timer interrupts.
void adc_stop_conversion();                                                     // stop periodic timer interrupts

void adc_trigger_conversion(); // tell ADC to start converting.

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // !ADC_H
