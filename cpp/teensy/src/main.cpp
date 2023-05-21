/*
IMPORTANT INFORMATION!

Some libraries are not imported properly when downloading from GitHub
This is because they are to big and so they get auto deleted
For now the libraries that need to be manually installed are
- CMSIS
To import libraries properly do this:
Copy file from ".\vortex-acoustics\Resource\CMSIS" to ".\vortex-acoustics\cpp\teensy\.pio\libdeps\teensy41\CMSIS"

Code written by: Vortex NTNU
*/

// CMSIS Libraries
#include "arm_const_structs.h"
#include "arm_math.h"

// Arduino Libraries
#include <Arduino.h>

// Sampling Analog to Digital Converter (ADC) Libraries
#include "GPT.h"
#include "PIT.h"
#include "adc.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"

// Digital Signal Processing (DSP) Libraries
#include "DSP.h"





// Libraries for ethernet
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

// Variables that should be in .h file ==========
// MAC address for every device must be different in every network
byte macAddressTeensy[] = {0xDE, 0xED, 0xBE, 0xEE, 0xFE, 0xED};
// Necessary networking variables for Teensy to be part of the network
IPAddress ipAddressTeensy(10, 0, 0, 111);
unsigned int localPort = 8888;

// Variables that should be in .cpp file ==========
// buffers for receiving and sending data
char receiveBuffer[UDP_TX_PACKET_MAX_SIZE];
char replyBuffer[] = "acknowledged";
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial connected\r\n");

    // Initialize Ethernet pins
    //Ethernet.init(20);

    // start the Ethernet
    Serial.println("Connecting to network");
    Ethernet.begin(macAddressTeensy, ipAddressTeensy);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
        }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    // Check that the port is available
    if (Udp.begin(localPort)) {
        Serial.println("SUCCESS! Connected to network");
    }
    else {
        Serial.println("FAILED could not connect to network");
        Serial.println(Udp.begin(localPort));
    }
}

void loop() {
   // if there's data available, read a packet
  int packetSize = Udp.parsePacket();

  if (packetSize) {
    // Get packet size
    Serial.print("Received packet of size ");Serial.print(packetSize);
    Serial.println();

    // Get senders IP
    IPAddress remoteIP = Udp.remoteIP();
    Serial.print("From: ");
    for (int i=0; i < 4; i++) {
      Serial.print(remoteIP[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.println();

    // Get senders port
    IPAddress remotePort = Udp.remotePort();
    Serial.print("Port: ");Serial.print(remotePort);
    Serial.println();

    // read the message into buffer
    Udp.read(receiveBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(receiveBuffer);

    // send a message back to the IP address and port that sent us the message
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyBuffer);
    Udp.endPacket();
  }

  // Wait for 0.1 seconds
  delay(100);
}
