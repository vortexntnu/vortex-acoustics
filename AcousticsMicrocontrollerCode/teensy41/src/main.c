


// Sampling Analog to Digital Converter (ADC) Libraries
#include "adc.h"
#include "clock.h"
#include "pit.h"
#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "MIMXRT1062_features.h"

// Digital Signal Processing (DSP) Libraries
#include "dsp.h"
#include "multilateration.h"

// Libraries for Ethernet
// #include "ethernet_module.h"
// #include "stack/fnet_stdlib.h"
// #include "teensy_udp.h"

int main(void) {
    // udp_init();
    // while (!udp_check_if_connected())
    //     ;
    // setup_communication();

    adc_init();

    filter_butterworth_4th_order_init();

    while (1) {
        uint8_t buffer_to_check = 0;
        // uint32_t samplingStartTime = millis();

        adc_start_conversion(SAMPLE_PERIOD, BLOCKING);

        while (1) {
            while (!(buffer_filled & (1 << buffer_to_check)))
                ;

            if (dsp_found_signal(buffer_to_check)) {
                break;
            }

            buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

            // if (millis() - samplingStartTime > SAMPLING_TIMEOUT) {
            //     break;
            // }
        }

        // filling buffers to ensure all hydrophones capture signal
        for (int i = 2; i < BUFFER_PER_CHANNEL; i++) {
            while (!(buffer_filled & (1 << buffer_to_check)))
                ;
            buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
        }

        adc_stop_conversion();

        if (find_pinger_position()) {
            // should add some error handling here
        }

        // transmit_data_udp();
    }
}

