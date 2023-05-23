#include <Arduino.h>
#include <vector>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "ethernetModule.h"

namespace teensyUDP {
    int32_t* frequency_data_from_client();
    void send_a_single_hydrophone_data(int16_t* hydrophone, int16_t lengthOfData);
    void send_hydrophone_data(int16_t* hydrophone1, int16_t* hydrophone2, int16_t* hydrophone3, int16_t* hydrophone4, int16_t* hydrophone5, int16_t lengthOfData);
}