#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace ethernetModule {
void UDP_init();
int16_t UDP_check_if_connected();
uint8_t* get_remoteIP();
uint16_t get_remotePort();
void UDP_send_ready_signal(uint8_t* remoteIPArray, uint16_t remotePort);
char* UDP_read_message();
void UDP_send_message(char* UDPReplyBuffer, int16_t sizeOfMessage, int16_t startIndexForMessage);
void UDP_clean_message_memory();
} // namespace ethernetModule