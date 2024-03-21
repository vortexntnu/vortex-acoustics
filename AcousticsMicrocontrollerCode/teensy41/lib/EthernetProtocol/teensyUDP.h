#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <vector>
#include <string>
#include <sstream>

#include "ethernetModule.h"

// To not overwhelm the client we need to limit each data pack sent in size
#define MAX_CLIENT_CAPACITY 100
#define FREQUENCY_LIST_LENGTH 10

namespace teensyUDP {
void frequency_data_from_client(int32_t *frequenciesOfInterest, int32_t* frequencyVariances);

void send_data_16Bit(int16_t* data, int16_t lengthOfData);
void send_data_32Bit(int32_t* data, int32_t lengthOfData);
void send_data_64Bit(double* data, int32_t lengthOfData);
void send_type_message(char* typeMessage, uint8_t messageLength);

void send_hydrophone_data(int16_t* hydrophone, int16_t lengthOfData);
void send_samples_raw_data(int16_t* samplesRaw, int16_t lengthOfData);
void send_samples_filtered_data(q15_t* samplesFiltered, int16_t lengthOfData);
void send_FFT_data(q15_t* FFTdata, int16_t lengthOfData);
void send_peak_data(std::vector<std::vector<q31_t>> peakData, int16_t lengthOfPeakList);
void send_tdoa_data(double* tdoaData, int8_t lengthOfData = 5);
void send_location_data(double* locationData, int8_t lengthOfData = 3);

void setupTeensyCommunication(int32_t *frequenciesOfInterest, int32_t* frequencyVariances);
} // namespace teensyUDP