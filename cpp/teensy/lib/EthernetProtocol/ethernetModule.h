#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace ethernetModule {
    void UDP_init();
    int16_t UDP_check_if_connected();
    char* UDP_read_message();
    void UDP_send_message(char* UDPReplyBuffer);
    void UDP_clean_message_memory();
}