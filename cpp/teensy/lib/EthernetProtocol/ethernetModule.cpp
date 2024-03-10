#include "ethernetModule.h"

// Networking variables
byte macAddressTeensy[] = {0x00, 0x00, 0xBE, 0xEE, 0xFE, 0xED};
IPAddress ipAddressTeensy(10, 0, 0, 111);
IPAddress dnsTeensy(10, 0, 0, 1);
IPAddress defautGatewayTeensy(10, 0, 0, 1);
IPAddress subnetMaskTeensy(255, 255, 255, 0);
unsigned int localPort = 8888;

// buffers for receiving and sending data
char UDPReceiveBuffer[UDP_TX_PACKET_MAX_SIZE];

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

namespace ethernetModule {
void UDP_init() {
    // Configure pins for Teensy
    // Ethernet.init(20);

    // Start the ethernet connection
    Ethernet.begin(macAddressTeensy, ipAddressTeensy);

    // Ethernet.setLocalIP(ipAddressTeensy);
    // Ethernet.setMACAddress(macAddressTeensy);
    // Ethernet.setDnsServerIP(dnsTeensy);
    // Ethernet.setGatewayIP(defautGatewayTeensy);
    // Ethernet.setSubnetMask(subnetMaskTeensy);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    // Check that the port is available
    if (Udp.begin(localPort)) {
        Serial.println("SUCCESS! Connected to network");
    } else {
        Serial.println("FAILED could not connect to network");
        Serial.println(Udp.begin(localPort));
    }
}

int16_t UDP_check_if_connected() {
    int16_t packetSize = Udp.parsePacket();
    return packetSize;
}

uint8_t* get_remoteIP() {
    static uint8_t tempIP[4];
    IPAddress remoteIP = Udp.remoteIP();
    tempIP[0] = (uint8_t)remoteIP[0];
    tempIP[1] = (uint8_t)remoteIP[1];
    tempIP[2] = (uint8_t)remoteIP[2];
    tempIP[3] = (uint8_t)remoteIP[3];
    return tempIP;
}

uint16_t get_remotePort() {
    static uint16_t tempPort;
    IPAddress remotePort = Udp.remotePort();
    tempPort = (uint16_t)remotePort;
    return tempPort;
}

void UDP_send_ready_signal(uint8_t* remoteIPArray, uint16_t remotePort) {
    // Variables
    char UDPReplyBuffer[] = "READY";
    byte tempByte;

    // Set up IP and Port of sender
    IPAddress remoteIP(remoteIPArray[0], remoteIPArray[1], remoteIPArray[2], remoteIPArray[3]);

    // Start sending data
    Udp.beginPacket(remoteIP, remotePort);
    for (int16_t i = 0; i < 5; i++) {
        tempByte = (byte)UDPReplyBuffer[i];
        Udp.write(tempByte);
    }
    Udp.endPacket();
}

char* UDP_read_message() {
    // read the message into buffer
    Udp.read(UDPReceiveBuffer, UDP_TX_PACKET_MAX_SIZE);

    return UDPReceiveBuffer;
}

void UDP_send_message(char* UDPReplyBuffer, int16_t sizeOfMessage, int16_t startIndexForMessage) {
    // Temporary variable for later use
    byte tempByte;

    // Get IP and Port of sender
    IPAddress remoteIP = Udp.remoteIP();
    IPAddress remotePort = Udp.remotePort();

    // Start sending data
    Udp.beginPacket(remoteIP, remotePort);
    for (int16_t i = startIndexForMessage; i < (startIndexForMessage + sizeOfMessage); i++) {
        tempByte = (byte)UDPReplyBuffer[i];
        Udp.write(tempByte);
    }
    Udp.endPacket();
}

void UDP_clean_message_memory() {
    memset(UDPReceiveBuffer, 0, UDP_TX_PACKET_MAX_SIZE); //clear out the packetBuffer array
}
}