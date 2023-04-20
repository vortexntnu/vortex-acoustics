#include "adc.h"
#include "GPT.h"
#include "PIT.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"
#include "DMAChannel.h"
#include "Arduino.h"

// used to link Quad timers to DMA channels
extern "C" void xbar_connect(unsigned int input, unsigned int output); // in pwm.c

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

namespace adc
{

    ADC_sample_mode ADC_mode;

#define DB_MASK 0xFFFF0000 // bit 16 to 31 of port 1 are the parallel interface
#define DB_REG_SHIFT 16    // values in GPIO reg must be shifted by 16
    volatile uint8_t channels_processed;
#ifdef TESTING
    uint16_t testing_value_par;
#endif

    RingBuffer_16bit ChannelA0 = RingBuffer_16bit();
    RingBuffer_16bit ChannelA1 = RingBuffer_16bit();
    RingBuffer_16bit ChannelB0 = RingBuffer_16bit();
    RingBuffer_16bit ChannelB1 = RingBuffer_16bit();
    RingBuffer_16bit ChannelC0 = RingBuffer_16bit();
    RingBuffer_32bit sampleTime = RingBuffer_32bit();

    RingBuffer_16bit *ringbuffer_channels_ptr[5];

    elapsedMicros stopwatch;

    // sets pins accordingly to value (no control signals)
    void
    write_ADC_par(uint16_t value);
    // returns value on par-bus to ADC (no control signals)
    uint16_t read_ADC_par();

    // should not be accessed from outside, get called when triggerconversion is called.
    // trigger conversion can be called from the outside to sample only once.
    void beginRead();
    void readData();
    void next_RD(); // pulling _RD down, starting timer for next read
    void stopRead();
    void transferData(); // transfer data to ringbuffers.
    void read_loop();

    void init()
    {
        gpio::set_normal_GPIO(1 << adc::_WR, _WR_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::_RD, _RD_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::_CS, _CS_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::BUSYINT, BUSYINT_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::HWSW, HWSW_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::PARSER, PARSER_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::XCLK, XCLK_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::STBY, STBY_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::CONVST, CONVST_GPIO_PORT_NORMAL);
        gpio::set_normal_GPIO(1 << adc::RESET, RESET_GPIO_PORT_NORMAL);

        gpio::set_normal_GPIO(0xFFFF0000, DB_GPIO_PORT_NORMAL);

        // BUSYINT as input
        gpio::configPin(BUSYINT, 0, BUSYINT_GPIO_PORT_NORMAL);
        // * no need, will be done in built-in interrupt function

        // _CS as output, high because interface is not enable on start-up
        gpio::configPin(_CS, 1, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        // HWSW as output, HIGH to select software mode
        gpio::configPin(HWSW, 1, HWSW_GPIO_PORT_NORMAL);
        gpio::write_pin(HWSW, 1, HWSW_GPIO_PORT_NORMAL);

        // PAR/SER as output, LOW to select parallel interface
        gpio::configPin(PARSER, 1, HWSW_GPIO_PORT_NORMAL);
        gpio::write_pin(PARSER, 0, HWSW_GPIO_PORT_NORMAL);

        // we are in software mode: XCLK as output, LOW (to ground) because NOT used
        gpio::configPin(XCLK, 1, XCLK_GPIO_PORT_NORMAL);
        gpio::write_pin(XCLK, 0, XCLK_GPIO_PORT_NORMAL);

        // _RD as output, HIGH, communication inactive
        gpio::configPin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);

        // _WR as output, HIGH, communication inactive
        gpio::configPin(_WR, 1, _WR_GPIO_PORT_NORMAL);
        gpio::write_pin(_WR, 1, _WR_GPIO_PORT_NORMAL);

        // _STBY as output, LOW because we use software mode (p8)
        gpio::configPin(STBY, 1, STBY_GPIO_PORT_NORMAL);
        gpio::write_pin(STBY, 0, STBY_GPIO_PORT_NORMAL);

        // CONVST as output, LOW
        gpio::configPin(CONVST, 1, CONVST_GPIO_PORT_NORMAL);
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);

        // RESET as output, active high
        gpio::configPin(RESET, 1, RESET_GPIO_PORT_NORMAL);
        gpio::write_pin(RESET, 0, RESET_GPIO_PORT_NORMAL);

#ifndef TESTING
        // configuring parallel interface, as input
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0x00000000, DB_MASK);
#endif
#ifdef TESTING
        // output, for testing pourpuse (LEDs)
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0xFFFF0000, DB_MASK);
#endif
    }

    // set up sampling
    void setup()
    {
        clock::setup(); /// the clockfrequency needs to be defined somewhere, does it need to be called also if adc is not init()
        PIT::setup();

        // setting pointers for the ringbuffers
        ringbuffer_channels_ptr[0] = &ChannelA0;
        ringbuffer_channels_ptr[1] = &ChannelA1;
        ringbuffer_channels_ptr[2] = &ChannelB0;
        ringbuffer_channels_ptr[3] = &ChannelB1;
        ringbuffer_channels_ptr[4] = &ChannelC0;

        // experimental found values
        PIT::setUpPeriodicISR(readData, clock::get_clockcycles_nano(T_RDL * 20), PIT::PIT_1);
        Serial.print("Clockcycles PIT1 : ");
        Serial.println(clock::get_clockcycles_nano(T_RDL * 20));
        PIT::setUpPeriodicISR(next_RD, clock::get_clockcycles_nano(T_RDH * 20), PIT::PIT_2);
        Serial.print("Clockcycles PIT2 : ");
        Serial.println(clock::get_clockcycles_nano(T_RDH * 20));
        // ! connect beginRead() to BUSY/INT interrupt -> is done in trigger_conversion()
    }

    // is starting the major loop timer, PIT0 that will trigger the conversion until stopped
    // @sample_period_us : a conversion will happen every "sample_period_us" microseconds
    void startConversion(uint32_t sample_period_us, ADC_sample_mode sample_mode)
    {
        Serial.println("Starting conversion");
        // ? do it in one call?
        // PIT::setUpPeriodicISR(triggerConversion, clock::get_clockcycles_micro(1000000), PIT::PIT_0);
        // value found by trial and error.

        ADC_mode = sample_mode;
        uint32_t bounded_period = sample_period_us;
        switch (sample_mode)
        {
        case BLOCKING:
            if (sample_period_us < MIN_SAMP_PERIOD_BLOCKING)
            {
                bounded_period = MIN_SAMP_PERIOD_BLOCKING;
            }
            break;
        case TIMER:
            if (sample_period_us < MIN_SAMP_PERIOD_TIMER)
            {
                bounded_period = MIN_SAMP_PERIOD_TIMER;
            }
            break;
        case DMA:
            if (sample_period_us < MIN_SAMP_PERIOD_DMA)
            {
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
        PIT::setUpPeriodicISR(triggerConversion, clock::get_clockcycles_micro(bounded_period), PIT::PIT_0);

        PIT::startPeriodic(PIT::PIT_0); // will call triggerConversion
    }

    void stopConversion()
    {
        // no new conversion
        PIT::stopPeriodic(PIT::PIT_0);
        // to finish the ongoing reading
        delayMicroseconds(100);
        for (uint8_t i = 1; i < 3; i++)
        {
            PIT::stopPeriodic(i);
        }
        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        // resetting ADC from actual conversion
        gpio::write_pin(RESET, 1, RESET_GPIO_PORT_NORMAL);
        delay(1);
        gpio::write_pin(RESET, 0, RESET_GPIO_PORT_NORMAL);
    }

    /// @brief function to start the conversion of data from ADC. once ADC is ready to output data, GpioISR will be triggered by the BUSY pin
    void triggerConversion()
    {
        // Serial.println("t");

        // ringbuffer with the timestamps
        stopwatch = elapsedMicros();
        sampleTime.insert(micros());

        // will pull the CONVST line high, that indicates to the adc to start conversion on all channels

        gpio::write_pin(CONVST, 1, CONVST_GPIO_PORT_NORMAL);

        // enable interrupt on BUSY/INT pin
        switch (ADC_mode)
        {
        case BLOCKING:
            attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), read_loop, FALLING);
            break;
        case TIMER:
            attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), beginRead, FALLING);
            break;
        default:
            break;
        }
        // * previous version
        // attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), beginRead, FALLING);
    }

    // resetting channels_processed and CONVST, continues with readData
    // updated version
    void beginRead()
    {
        // Serial.println("b");
        //  ! disable interrupts from BUSY/INT, will be enabled again by next triggerConvst()
        detachInterrupt(BUSYINT_ARDUINO_PIN);
        // PIT0 is continuing to run

        channels_processed = 0;
#ifdef TESTING
        testing_value_par = 1;
#endif
        // no need to have CONVST high now
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);

        gpio::write_pin(_CS, 0, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
        // should take at least 20ns to trigger timer
        PIT::startPeriodic(PIT::PIT_1); // will call readData()
    }

    // reads the data on the parallel bus, when beginRead() was called (updated)
    void readData()
    {
        // disabling timer, so far it is not a periodic timer
        PIT::stopPeriodic(PIT::PIT_1);
        // Serial.println("r");

#ifndef TESTING
        // sampleData[channels_processed] = read_ADC_par(); /// sampledata len = 8, maybe change if not needed (readloop would only need len = 5)
        ringbuffer_channels_ptr[channels_processed]->insert(read_ADC_par());
        // ringbuffer_channels_ptr[channels_processed]->insert(read_ADC_par());
#endif
#ifdef TESTING
        write_ADC_par(testing_value_par);
        testing_value_par *= 2;
#endif

        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        channels_processed++;

        // will automatically stop
        if (channels_processed == N_HYDROPHONES)
        {
            stopRead();
        }
        else
        {
            // ! testing of faster way
            //gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
            //PIT::startPeriodic(PIT::PIT_1);

            PIT::startPeriodic(PIT::PIT_2);
        }
    }

    // updated version
    void next_RD()
    {
        PIT::stopPeriodic(PIT::PIT_2);

        // Serial.println("n");

        gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
        // will call readData
        PIT::startPeriodic(PIT::PIT_1);
    }

    // turning of
    void stopRead()
    {
        // Serial.println("s");

        // timers are already off, no need to do anything
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);
        // unsigned long time_to_read = stopwatch;
        // Serial.print("time for 1 reading : ");
        // Serial.println(time_to_read);
        //  Serial.println("stop");
        // transferData();
    }

    void transferData()
    {
        ChannelA0.insert(sampleData[0]);
        ChannelA1.insert(sampleData[1]);
        ChannelB0.insert(sampleData[2]);
        ChannelB1.insert(sampleData[3]);
        ChannelC0.insert(sampleData[4]);
    }

    void read_loop()
    {
        detachInterrupt(BUSYINT_ARDUINO_PIN);

        // no need to have CONVST high now
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);

        gpio::write_pin(_CS, 0, _CS_GPIO_PORT_NORMAL);

        for (uint16_t i = 0; i < N_HYDROPHONES; i++)
        {
            gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
            // should take at least 20ns to trigger timer
            delayNanoseconds(T_RDL);

            ringbuffer_channels_ptr[i]->insert(read_ADC_par());
            gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);
            // this is already enough delay for 2ns (toggeling takes more than 2ns)
            // delayNanoseconds(20);
        }

        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);
        unsigned long time_to_read = stopwatch;
        // Serial.print("time for 1 reading : ");
        // Serial.println(time_to_read);
    }
    /**
      @brief configures the internal 32-bit config register of the ADC
      @param reg_val: value of the 32bit register
    */
    void config(uint32_t reg_val)
    {
        // pins as output
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0xFFFF0000, DB_MASK);
        //* see write access timing diagram on p.19 of ADC data sheet
        // check p.39 for info about config register
        // starting write access to ADC
        gpio::write_pin(_CS, 0, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_WR, 0, _WR_GPIO_PORT_NORMAL);

        // writing MSBs first
        write_ADC_par(reg_val >> 16);

        delayNanoseconds(15); // t_WRL; t_SUDI/t_HDI

        gpio::write_pin(_WR, 1, _WR_GPIO_PORT_NORMAL);

        delayNanoseconds(10); // t_WRH

        // then writing LSBs, timing of t_HDI is respected
        write_ADC_par(reg_val & 0xFFFF);
        gpio::write_pin(_WR, 0, _WR_GPIO_PORT_NORMAL);

        delayNanoseconds(15); // t_WRL

        gpio::write_pin(_WR, 1, _WR_GPIO_PORT_NORMAL); // stop 2nd write access
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        delayNanoseconds(5); // t_HDI

        // pins back as inputs
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0x00000000, DB_MASK);
    }

    void write_ADC_par(uint16_t value)
    {
        // The ADC pins are located at bits 16-31 of GPIO port (1)
        gpio::write_port(value << DB_REG_SHIFT, DB_GPIO_PORT_NORMAL, DB_MASK);
    }
    uint16_t read_ADC_par()
    {
        // we want the 16 highest bits
        return gpio::read_port(DB_GPIO_PORT_NORMAL) >> DB_REG_SHIFT;
    }

    void setting_up_timers_DMA()
    {
        // max value for counter: 0xFFFF
        // Set up 3 timers to create waveform timing events
        static uint16_t comp1load[3];
        // ! need to check if F_BUS is the right one
        // ! values only for testing
        Serial.print("FBUSACTUAL : ");
        Serial.println(F_BUS_ACTUAL);
        comp1load[0] = 0xFFFF;
        (uint16_t)((float)F_BUS_ACTUAL * (float)2);
        comp1load[1] = (uint16_t)((float)F_BUS_ACTUAL * (float)1);
        comp1load[2] = (uint16_t)((float)F_BUS_ACTUAL * (float)1);
        // in the Octo library:
        // 1: THTL total duraction for one pulse
        // 2: T0H the time after the GPIO needs to be LOW for a zero
        // 3: T1H, 1 code ,high voltage time

        TMR4_ENBL &= ~7; // disables the first 3 channels of timer 4
        // OEN: OFLAG is driven on external pin, FORCE: val is forced to OFLAG output,
        // MAster mode to reset other channels when this one is resetting
        TMR4_SCTRL0 = TMR_SCTRL_OEN | TMR_SCTRL_FORCE | TMR_SCTRL_MSTR;
        TMR4_CSCTRL0 = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_TCF1EN;
        TMR4_CNTR0 = 0;              // setting timer to 0
        TMR4_LOAD0 = 0;              // value to load when timer reaches compare (or overflow?)
        TMR4_COMP10 = comp1load[0];  // compare value when counting up
        TMR4_CMPLD10 = comp1load[0]; // preload for COMP10
        // CM (count mode): counting rising edges of primary source, PCS (Primary count source): IP bus clock with prescaler 1, LENGHT=1: counts until compare, OUTMODE: Toggle output on compare
        // ! for testing prescaler (it was 8 before)
        TMR4_CTRL0 = TMR_CTRL_CM(1) | TMR_CTRL_PCS(15) | TMR_CTRL_LENGTH | TMR_CTRL_OUTMODE(3);

        // channel 1 and 2 are configured the same
        TMR4_SCTRL1 = TMR_SCTRL_OEN | TMR_SCTRL_FORCE;
        TMR4_CNTR1 = 0;
        TMR4_LOAD1 = 0;
        TMR4_COMP11 = comp1load[1]; // T0H
        TMR4_CMPLD11 = comp1load[1];
        // CM (count mode): counting rising edges of primary source, PCS (Primary count source): IP bus clock with prescaler 1, LENGHT=0: counts until rollover, OUTMODE: Toggle output on compare
        // COINIT: reinitialization when channel 0 is resetting
        TMR4_CTRL1 = TMR_CTRL_CM(1) | TMR_CTRL_PCS(15) | TMR_CTRL_COINIT | TMR_CTRL_OUTMODE(3);

        TMR4_SCTRL2 = TMR_SCTRL_OEN | TMR_SCTRL_FORCE;
        TMR4_CNTR2 = 0;
        TMR4_LOAD2 = 0;
        TMR4_COMP12 = comp1load[2]; // T1H
        TMR4_CMPLD12 = comp1load[2];
        TMR4_CTRL2 = TMR_CTRL_CM(1) | TMR_CTRL_PCS(8) | TMR_CTRL_COINIT | TMR_CTRL_OUTMODE(3);

        // channel 3 as primary source for the other, to be able to go up to seconds
        TMR4_SCTRL3 = TMR_SCTRL_OEN | TMR_SCTRL_FORCE;
        TMR4_CNTR3 = 0;
        TMR4_LOAD3 = 0;
        TMR4_COMP13 = 0xFFFF; // T1H
        TMR4_CMPLD13 = 0xFFFF;
        TMR4_CTRL3 = TMR_CTRL_CM(1) | TMR_CTRL_PCS(8) | TMR_CTRL_OUTMODE(3);

        // route the timer outputs through XBAR to edge trigger DMA request

        CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON); // enabling clock (probably)
        xbar_connect(XBARA1_IN_QTIMER4_TIMER0, XBARA1_OUT_DMA_CH_MUX_REQ30);
        xbar_connect(XBARA1_IN_QTIMER4_TIMER1, XBARA1_OUT_DMA_CH_MUX_REQ31);
        xbar_connect(XBARA1_IN_QTIMER4_TIMER2, XBARA1_OUT_DMA_CH_MUX_REQ94);
        // don't know, p. 3271
        XBARA1_CTRL0 = XBARA_CTRL_STS1 | XBARA_CTRL_EDGE1(3) | XBARA_CTRL_DEN1 |
                       XBARA_CTRL_STS0 | XBARA_CTRL_EDGE0(3) | XBARA_CTRL_DEN0;
        XBARA1_CTRL1 = XBARA_CTRL_STS0 | XBARA_CTRL_EDGE0(3) | XBARA_CTRL_DEN0;

        // --------------------- DMA channels ----------------------------------
    }

    void setting_up_DMA_channels()
    {
        // configure DMA channels
        // dma1.begin();
        // dma1.sourceAddress();
        // dma1.destinationAddress();
        // dma1.TCD->SADDR = bitmask;
        // dma1.TCD->SOFF = 8;
        // dma1.TCD->ATTR = DMA_TCD_ATTR_SSIZE(3) | DMA_TCD_ATTR_SMOD(4) | DMA_TCD_ATTR_DSIZE(2);
        // dma1.TCD->NBYTES_MLOFFYES = DMA_TCD_NBYTES_DMLOE |
        //                             DMA_TCD_NBYTES_MLOFFYES_MLOFF(-65536) |
        //                             DMA_TCD_NBYTES_MLOFFYES_NBYTES(16);
        // dma1.TCD->SLAST = 0;
        // dma1.TCD->DADDR = &GPIO1_DR_SET;
        // dma1.TCD->DOFF = 16384;
        // dma1.TCD->CITER_ELINKNO = numbytes * 8;
        // dma1.TCD->DLASTSGA = -65536;
        // dma1.TCD->BITER_ELINKNO = numbytes * 8;
        // dma1.TCD->CSR = DMA_TCD_CSR_DREQ;
        // table p.54
        // dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_XBAR1_0);

        // dma2.TCD->SADDR = bitdata;
        // dma2.TCD->SOFF = 8;
        // dma2.TCD->ATTR = DMA_TCD_ATTR_SSIZE(3) | DMA_TCD_ATTR_DSIZE(2);
        // dma2.TCD->NBYTES_MLOFFYES = DMA_TCD_NBYTES_DMLOE |
        //                             DMA_TCD_NBYTES_MLOFFYES_MLOFF(-65536) |
        //                             DMA_TCD_NBYTES_MLOFFYES_NBYTES(16);
        // dma2.TCD->SLAST = 0;
        // dma2.TCD->DADDR = &GPIO1_DR_CLEAR;
        // dma2.TCD->DOFF = 16384;
        // dma2.TCD->CITER_ELINKNO = BYTES_PER_DMA * 8;
        // // dma2.TCD->DLASTSGA = (int32_t)(dma2next.TCD);
        // dma2.TCD->BITER_ELINKNO = BYTES_PER_DMA * 8;
        // dma2.TCD->CSR = 0;

        // dma2.begin();
        // dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_XBAR1_1);
        // dma2.attachInterrupt(isr);

        // dma3.begin();
        // dma3.TCD->SADDR = bitmask;
        // dma3.TCD->SOFF = 8;
        // dma3.TCD->ATTR = DMA_TCD_ATTR_SSIZE(3) | DMA_TCD_ATTR_SMOD(4) | DMA_TCD_ATTR_DSIZE(2);
        // dma3.TCD->NBYTES_MLOFFYES = DMA_TCD_NBYTES_DMLOE |
        //                             DMA_TCD_NBYTES_MLOFFYES_MLOFF(-65536) |
        //                             DMA_TCD_NBYTES_MLOFFYES_NBYTES(16);
        // dma3.TCD->SLAST = 0;
        // dma3.TCD->DADDR = &GPIO1_DR_CLEAR;
        // dma3.TCD->DOFF = 16384;
        // dma3.TCD->CITER_ELINKNO = numbytes * 8;
        // dma3.TCD->DLASTSGA = -65536;
        // dma3.TCD->BITER_ELINKNO = numbytes * 8;
        // dma3.TCD->CSR = DMA_TCD_CSR_DREQ | DMA_TCD_CSR_DONE;
        // // p.58
        // dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_XBAR1_2);
    }
}; // adc
