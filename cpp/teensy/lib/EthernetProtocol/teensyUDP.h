#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <vector>

#include "ethernetModule.h"

// To not overwhelm the client we need to limit each data pack sent in size
#define MAX_CLIENT_CAPACITY 100

namespace teensyUDP {
int32_t* frequency_data_from_client();

void send_data_16Bit(int16_t* data, int16_t lengthOfData);
void send_data_32Bit(int32_t* data, int32_t lengthOfData);

void send_hydrophone_data(int16_t* hydrophone, int16_t lengthOfData);
void send_samples_raw_data(int16_t* samplesRaw, int16_t lengthOfData);
void send_samples_filtered_data(q15_t* samplesFiltered, int16_t lengthOfData);
void send_FFT_data(q15_t* FFTdata, int16_t lengthOfData);
void send_peak_data(std::vector<std::vector<q31_t>> peakData, int16_t lengthOfPeakList);
} // namespace teensyUDP