import ethernetProtocolTeensy
import time

# Variables ==================================================
frequencyOfInterest = 20_000 # 20 kHz
frequencyVariance = 2_000 # 2 kHz

# Setup ethernet protocol
teensy = ethernetProtocolTeensy.TeensyCommunicationUDP(
    TEENSY_IP = "10.0.0.111",
    TEENSY_PORT = 8888,
    MY_PORT = 9999,
    MAX_PACKAGE_SIZE_RECEIVED = 65536,
    TIMEOUT = 10,
)

while True:
    while not teensy.check_if_available():
        """
        "IMPORTANT! 
        DO NOT have "time.sleep(x)" value to SMALLER than 1 second!!!
        This will interrupt sampling by asking teensy if its available to many times
        If less than 1 second you risc crashing teensy to PC communication O_O
        """
        time.sleep(1)

    teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
    raw, filtered, fft, peaks = teensy.get_DSP_data()
    print(raw)
    print(peaks)
    teensy.send_SKIP()
    
    time.sleep(1)
