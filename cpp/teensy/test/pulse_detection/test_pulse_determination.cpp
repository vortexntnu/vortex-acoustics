#include "Arduino.h"
#include "inputPulseDetermination.h"
#include "pulse_determination.h"
#include "unity.h"

void test_short_time_fourier_transform() {
    uint32_t pulseLength = 4; // [ms]

    float32_t* pulse = new float32_t[pulseLength * SAMPLING_FREQUENCY];
    for (uint32_t i = 0; i < pulseLength * SAMPLING_FREQUENCY; i++) {
        pulse[i] = COSINE_WAVE[i];
    }

    float32_t* fft = new float32_t[FFT_SIZE];
    arm_status status = shortTimeFourierTransform(pulse, pulseLength, fft);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);

    float32_t computedCarrierFrequency = computeCarrierFrequency(fft);

    float32_t tolerance = static_cast<float32_t>(SAMPLING_FREQUENCY) / FFT_SIZE;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, EXPECTED_CARRIER_FREQ,
                             computedCarrierFrequency);

    delete[] pulse;
    delete[] fft;
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_short_time_fourier_transform);
    UNITY_END();
}
