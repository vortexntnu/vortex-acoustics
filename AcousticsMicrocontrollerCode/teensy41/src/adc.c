#include "adc.h"
#include "clock.h"
#include "pit.h"


/*
*---- from ADC datasheet -----
*    init():
        -configuring the pins: maybe need to reconfigure from write to read --> test
            -CS(13), RD(12), WR(11), PAR/SER(8), HW/SW(37) CONVST as output, BUSY/INT(35) as input
            -floating pins in software: XCLK/RANGE, REFEN
        -settings pins to enable software and parallel mode
        Config ADC:
        -finding the right register value
        -write value to register
*    config_adc():
        -PAR-pins as output (16 pins)
        -CS first and then WR low (for at least 15ns)
        -to avoid any timing problem, output register value before pulling low
         data (16 MSB) must be on the bus at least 5ns to be read correctly
        -WR high, CS stays low. High pulse for at least 10ns
        -writing second part (16 LSB) to the pins
        -WR to low for at least 15ns
        -First WR high, then CS, data must be valid for 5ns more!
        IF bit 30, the register values can be read (maybe on parallel interface) on SDO_A
        -PAR-pins as input again
*    start_convertion():
        -pull CONVST high to start conversion (all channels at the same time)
        -CONVST can stay high or pulled low after, best is when BUSY/INT happens
        -if using BUSY: max time to BUSY high: 25ns. Then stays high for max 1,7 micro sec
            depending on ADC model (12-14-16 bit), don't care about time, using BUSY pin
*    beginRead():
        -wait for at least 86ns (model dependant, we have 12-bit resolution)
        -CONVST to LOW
        -CS low, then RD low(no delay needed between)
        -CS can stay low
        -resetting channels_processed
        -starting timer to call readData()
       ! - RD has to be low for at least 20ns
Repeat:
*    readData():
        - stopping timer that called this function
        TODO: use non-periodic timers
        -value on PAR-bus is ready after max 15ns after RD low
        - RD to HIGH, must be high for at least 2ns for next data.
        INFO: Data is valid for 5ns after rising edge
        * if all channels read: go to stopRead()
        - timer to call next_RD()
*    next_RD():
        -pull read down again if other data expected
        - starting timer to call readData() again
*    stopRead();
        -CS to high (RD is already high from readData())
        -BUS goes to tree-state after max 10ns.
        -On ADS8528: 0 ns between CS rising edge to next CONVST rising edge
        ! min time between 2 CONVST rising edges: 240ns
*/



ADC_sample_mode ADC_mode;

#define DB_MASK 0xFFFF0000 // bit 16 to 31 of port 1 are the parallel interface
#define DB_REG_SHIFT 16    // values in GPIO reg must be shifted by 16
volatile uint8_t channels_processed;
#ifdef TESTING
uint16_t testing_value_par;
#endif


int16_t samples_raw_hydro_1[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydro_2[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydro_3[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydro_4[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydro_5[RAW_HYDROPHONE_SIZE];

time_buff_3_1024 timestamps;


int16_t* samples_raw_hydrophones[5] = {samples_raw_hydro_1, samples_raw_hydro_2, samples_raw_hydro_3, samples_raw_hydro_4, samples_raw_hydro_5};

volatile uint8_t stop_sampling;

volatile uint8_t active_buffer = 0; // to know which one is being filled, [0, BUFFER_PER_CHANNEL-1]
volatile size_t sample_index = 0;
volatile uint16_t buffer_filled = 0;
volatile uint32_t overall_buffer_count = 0;

uint32_t clk_cyc = 0;

// * DMA ----------------------
uint8_t _RD_reg_value = 1 << _RD;
uint8_t vec_RD_values[5];


static void adc_config(uint32_t reg_val) {
    // pins as output
    // configPort(DB_GPIO_PORT_NORMAL-> 0xFFFF0000, DB_MASK);
    DB_GPIO_PORT_NORMAL->>GDIR = (DB_GPIO_PORT_NORMAL->GDIR & ~DB_MASK) | (DB_MASK);
    //* see write access timing diagram on p.19 of ADC data sheet
    // check p.39 for info about config register
    // starting write access to ADC
    _CS_GPIO_PORT_NORMAL->>DR_CLEAR = (1 << _CS);
    _WR_GPIO_PORT_NORMAL->>DR_CLEAR = (1 << _WR);

    // writing MSBs first
    DB_GPIO_PORT_NORMAL->>DR_SET = ((reg_val >> 16) & DB_MASK);
    DB_GPIO_PORT_NORMAL->>DR_CLEAR = (~(reg_val >> 16)) & DB_MASK;

    // delayNanoseconds(15); // t_WRL; t_SUDI/t_HDI

    _WR_GPIO_PORT_NORMAL->>DR_SET = (1 << _WR);

    // delayNanoseconds(10); // t_WRH

    // then writing LSBs, timing of t_HDI is respected
    DB_GPIO_PORT_NORMAL->>DR_SET = ((reg_val & 0xFFFF) & DB_MASK);
    DB_GPIO_PORT_NORMAL->>DR_CLEAR = (~(reg_val & 0xFFFF)) & DB_MASK;

    _WR_GPIO_PORT_NORMAL->>DR_CLEAR = (1 << _WR);

    // delayNanoseconds(15); // t_WRL

    _WR_GPIO_PORT_NORMAL->>DR_CLEAR = (1 << _WR);
    _CS_GPIO_PORT_NORMAL->>DR_SET = (1 << _CS);

    // delayNanoseconds(5); // t_HDI

    // pins back as inputs
    DB_GPIO_PORT_NORMAL->>GDIR = (DB_GPIO_PORT_NORMAL.GDIR & ~DB_MASK) | (0);

    DB_GPIO_PORT_NORMAL->>GDIR &= ~DB_MASK;
}



void read_loop() {
    // timestamps[active_buffer][sample_index] = ARM_DWT_CYCCNT - clk_cyc;

    if (stop_sampling) {
        return;
    }
    NVIC_DISABLE_IRQ(IRQ_PIT);
    // detachInterrupt(BUSYINT_ARDUINO_PIN);

    // no need to have CONVST high now
    // * write both at the same time to go faster
    IMXRT_GPIO7.DR_CLEAR = 1 << CONVST | 1 << _CS;

    for (uint16_t hydrophone = 0; hydrophone < N_HYDROPHONES; hydrophone++) {
        // write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL->;
        IMXRT_GPIO9.DR_CLEAR |= (1 << _RD);
        // delayNanoseconds(T_RDL);

        size_t index = sample_index + active_buffer * SAMPLE_LENGTH_ADC;
        samples_raw_hydrophones[hydrophone][index] = DB_GPIO_PORT_NORMAL->PSR >> DB_REG_SHIFT;
        IMXRT_GPIO9.DR_SET |= (1 << _RD);
        // write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL->;
        //  this is already enough delay for 2ns (toggeling takes more than 2ns)
        //  delayNanoseconds(20);
    }

    _CS_GPIO_PORT_NORMAL->DR_SET |= (1 << _CS);

    // timestamps[active_buffer][sample_index] = stopwatch;
    // stopwatch = elapsedMicros();
    sample_index++;

    if (sample_index >= SAMPLE_LENGTH_ADC) {
        // updating global variables
        buffer_filled |= (1 << active_buffer);
        sample_index = sample_index % SAMPLE_LENGTH_ADC; // or maybe to 0
        active_buffer = (active_buffer + 1) % BUFFER_PER_CHANNEL;
        buffer_filled &= ~(1 << active_buffer);

        overall_buffer_count++;
    }
    // timestamps[active_buffer][sample_index] = ARM_DWT_CYCCNT - clk_cyc;
    // unsigned long time_to_read = stopwatch;
    // Serial.print("time 1 read: ");
    // Serial.println(time_to_read);
    NVIC_ENABLE_IRQ(IRQ_PIT);
}



void adc_init() {
    // ! commented because we test with using the fast pins
    // set_normal_GPIO(1 << adc::_WR, _WR_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::_RD, _RD_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::_CS, _CS_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::BUSYINT, BUSYINT_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::HWSW, HWSW_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::PARSER, PARSER_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::XCLK, XCLK_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::STBY, STBY_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::CONVST, CONVST_GPIO_PORT_NORMAL->;
    // set_normal_GPIO(1 << adc::RESET, RESET_GPIO_PORT_NORMAL->;

    // set_normal_GPIO(0xFFFF0000, DB_GPIO_PORT_NORMAL->;

    // BUSYINT as input
    BUSYINT_GPIO_PORT_NORMAL->>GDIR &= (1 << BUSYINT);
    // * no need, will be done in built-in interrupt function

    // _CS as output, high because interface is not enable on start-up
    _CS_GPIO_PORT_NORMAL->GDIR |= (1 << _CS);
    _CS_GPIO_PORT_NORMAL->DR_SET = (1 << _CS);

    // HWSW as output, HIGH to select software mode
    HWSW_GPIO_PORT_NORMAL->GDIR |= (1 << HWSW);
    HWSW_GPIO_PORT_NORMAL->DR_SET = (1 << HWSW);

    // PAR/SER as output, LOW to select parallel interface
    HWSW_GPIO_PORT_NORMAL->GDIR |= (1 << PARSER);
    HWSW_GPIO_PORT_NORMAL->DR_CLEAR = (1 << PARSER);

    // we are in software mode: XCLK as output, LOW (to ground) because NOT used
    XCLK_GPIO_PORT_NORMAL->GDIR |= (1 << XCLK);
    XCLK_GPIO_PORT_NORMAL->DR_CLEAR = (1 << XCLK);

    // _RD as output, HIGH, communication inactive
    _RD_GPIO_PORT_NORMAL->GDIR |= (1 << _RD);
    _RD_GPIO_PORT_NORMAL->DR_SET = (1 << _RD);

    // _WR as output, HIGH, communication inactive
    _WR_GPIO_PORT_NORMAL->GDIR |= (1 << _WR);
    _WR_GPIO_PORT_NORMAL->DR_SET = (1 << _WR);

    // _STBY as output, LOW because we use software mode (p8)
    STBY_GPIO_PORT_NORMAL->GDIR |= (1 << STBY);
    STBY_GPIO_PORT_NORMAL->DR_CLEAR = (1 << STBY);

    // CONVST as output, LOW
    CONVST_GPIO_PORT_NORMAL->GDIR = (1 << CONVST);
    CONVST_GPIO_PORT_NORMAL->DR_CLEAR = (1 << CONVST);

    // RESET as output, active high
    RESET_GPIO_PORT_NORMAL->GDIR |= (1 << RESET);
    RESET_GPIO_PORT_NORMAL->DR_CLEAR = (1 << RESET);

#ifndef TESTING
    // configuring parallel interface, as input
    DB_GPIO_PORT_NORMAL->GDIR = (DB_GPIO_PORT_NORMAL.GDIR & ~DB_MASK) | (0);
#endif
#ifdef TESTING
    // output, for testing pourpuse (LEDs)
    configPort(DB_GPIO_PORT_NORMAL-> 0xFFFF0000, DB_MASK);
#endif


    
    adc_config(ADC_reg_config);
    
    clock_setup(); /// the clockfrequency needs to be defined somewhere, does it need to be called also if adc is not init()
    pit_setup();



    // ! connect beginRead() to BUSY/INT interrupt -> is done in trigger_conversion()
}


/**
   @brief is starting the major loop timer, PIT0 that will trigger the conversion until stopped
   @param sample_period_us : a conversion will happen every "sample_period_us" microseconds
   @param sample_mode : ONLY use BLOCKING
*/
void adc_start_conversion(float sample_period_us, ADC_sample_mode sample_mode) {
    // Serial.println("Starting conversion");
    // ? do it in one call?
    // setUpPeriodicISR(triggerConversion, clock_ADC::get_clockcycles_micro(1000000), PIT_0);
    // value found by trial and error.

    sample_index = 0;
    stop_sampling = 0;
    // * to check if buffer needs to be set to 0.
    active_buffer = 0;
    overall_buffer_count = 0;
    buffer_filled = 0;

    RESET_GPIO_PORT_NORMAL->DR_SET = (1 << RESET);
    delay(1);
    RESET_GPIO_PORT_NORMAL->DR_CLEAR = (1 << RESET);

    ADC_mode = sample_mode;
    float bounded_period = sample_period_us;
    switch (sample_mode) {
    case BLOCKING:
        if (sample_period_us < MIN_SAMP_PERIOD_BLOCKING) {
            bounded_period = MIN_SAMP_PERIOD_BLOCKING;
        }
        break;
    case TIMER:
        if (sample_period_us < MIN_SAMP_PERIOD_TIMER) {
            bounded_period = MIN_SAMP_PERIOD_TIMER;
        }
        break;
    case DMA:
        if (sample_period_us < MIN_SAMP_PERIOD_DMA) {
            bounded_period = MIN_SAMP_PERIOD_DMA;
        }
        break;
    default:
        break;
    }

    // if (sample_period_us < MIN_SAMPLING_PERIOD)
    // {
    //     sample_period_us = MIN_SAMPLING_PERIOD;
    // }

    setUpPeriodicISR(adc_trigger_conversion, get_clockcycles_micro(bounded_period), PIT_0);
    startPeriodic(PIT_0); // will call triggerConversion
}

void adc_stop_conversion() {
    stop_sampling = 1;
    // no new conversion
    stopPeriodic(PIT_0);
    // to finish the ongoing reading
    // delayMicroseconds(100);
    for (uint8_t i = 1; i < 3; i++) {
        stopPeriodic(i);
    }
    // detachInterrupt(BUSYINT_ARDUINO_PIN);

    _RD_GPIO_PORT_NORMAL->DR_SET = (1 << _RD);
    CONVST_GPIO_PORT_NORMAL->DR_CLEAR = (1 << CONVST);
    _CS_GPIO_PORT_NORMAL->DR_SET = (1 << _CS);

    // resetting ADC from actual conversion
    RESET_GPIO_PORT_NORMAL->DR_SET = (1 << RESET);
    // delay(1);
    RESET_GPIO_PORT_NORMAL->DR_CLEAR = (1 << RESET);
}

/// @brief function to start the conversion of data from ADC. once ADC is ready to output data, GpioISR will be triggered by the BUSY pin
void adc_trigger_conversion() {
    // Serial.println("t");

    // will pull the CONVST line high, that indicates to the adc to start conversion on all channels
    // stopwatch = elapsedMicros();
    // clk_cyc = ARM_DWT_CYCCNT;
    CONVST_GPIO_PORT_NORMAL->DR_SET = (1 << CONVST);
    // adding timestamp
    // timestamps[active_buffer][sample_index] = micros();
    if (!stop_sampling) {
        timestamps[active_buffer][sample_index] = ARM_DWT_CYCCNT;
    }

    // changing indexing to put data at right place is done at end of read loop
    // ! not yet implemented with timers

    // enable interrupt on BUSY/INT pin
    switch (ADC_mode) {
    case BLOCKING:
        // attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), read_loop, FALLING);
        break;
    case TIMER:
        // attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), beginRead, FALLING);
        break;
    default:
        break;
    }
    // clk_cyc = ARM_DWT_CYCCNT;
}


