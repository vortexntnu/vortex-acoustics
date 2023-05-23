#include <Arduino.h>
#include <vector>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "ethernetModule.h"

namespace teensyUDP {
    int32_t* frequency_data_from_client();
    void send_hydrophone_data(int16_t* hydrophone, int16_t lengthOfData);
}