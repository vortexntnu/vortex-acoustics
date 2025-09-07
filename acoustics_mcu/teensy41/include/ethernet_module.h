#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <cstdint>

void udp_init();

int16_t udp_check_if_connected();

uint8_t* get_remoteIP();
uint16_t get_remotePort();

void udp_send_ready_signal(uint8_t* remoteIPArray, uint16_t remotePort);

char* udp_read_message();

void udp_send_message(char* UDPReplyBuffer, int16_t sizeOfMessage, int16_t startIndexForMessage);

void udp_send_message_raw(void* UDPReplyBuffer, uint32_t sizeOfMessage);

void udp_clean_message_memory();
