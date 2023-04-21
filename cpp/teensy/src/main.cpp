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
#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

// Digital Signal Processing (DSP) Libraries
#include "DSP.h"





// Variables for Debugging ==================================================
/*
These are just to measure the time it takes to run the entire code.
The new calculations show 13300 us (micro seconds) (13.3 ms) for the whole algorithm
*/
unsigned long timeDiff;
unsigned long startTime;
unsigned long endTime;
int16_t samplesRaw[SAMPLE_LENGTH] = {
-22135, -4501, 9382, 20458, -6030, -3248, -7821, 11419, -14438, -13866, -18827, -1036, 10760, -6894, -6259, -27353, 9440, -4006, -2801, -32317, -15809, -1979, 10806, 17552, -290, 17190, -1592, 23304, 9443, 169, -8867, 9738, 1872, 3076, 9389, -14367, 7109, -1411, 13647, -13806, -4069, -8267, 4567, -2827, 3432, 11701, 11814, 32767, 9994, 12282, -1334, 12400, 1128, 18048, -405, -4371, 10657, 12448, 22853, -3562, 4996, -6319, 18882, -1837, 1016, -19476, 10405, 27604, 10764, 22535, -9892, 20376, -447, 15005, -8607, -12258, 2461, 15902, 16423, -13461, -1006, -17987, 19145, -7933, -14438, -26774, -10025, -2489, -4627, 4785, -3037, 20722, -156, 19682, -22830, -3706, -8205, 11856, -11348, -8478, -13229, -2765, 18378, -4148, -13792, -32767, -15054, -16175, 1718, -13721, -20646, -4501, 14130, 21017, -2864, -3248, -3725, 10665, -6815, -759, -11410, -7901, 10938, 6212, -6259, -21596, 9440, -5509, -4231, -19210, -13641, -5905, 2795, 20864, -7394, 10304, 8939, 22704, -3662, 209, -21267, 12672, 1872, 3076, -3716, -3321, 11654, 0, 11850, -11877, -7719, -6519, 4567, -2827, 7008, 10433, 17165, 32767, 20832, 20694, -3831, 21626, 1914, 14882, -6307, 1783, 13265, 18967, 27090, -15534, -2775, -8748, 6217, -2337, -8017, -8593, 4876, 19104, 19743, 22535, 1727, 16800, 8639, 18262, -7070, 549, -4638, 2795, 3316, -15974, 1251, -4881, 6038, -7933, -16854, -24555, -858, 139, 8479, 4785, -16143, 7615, 12644, 12097, -22769, -11564, -8205, 12837, -11348, -18564, -17774, -6740, 8093, -16139, -13792, -32767, -5611, -22559, 1718, -13721, -9106, 8605, 4451, 24845, -15971, -3248, -15946, 14022, -5461, -9661, -19196, -10405, 4935, 6212, 5301, -28403, 9440, -10642, -4231, -25996, -19001, -1979, 15902, 7757, -7394, 14873, -3096, 29793, -398, 13276, -8867, -434, 13140, 3076, -3716, -1260, 10966, 10780, 14215, -12152, -3059, -6519, 8724, 10279, -4790, -1405, 17165, 32767, 14769, 17696, -1334, 15259, 6178, 16456, 2647, -7915, 6175, 14864, 13983, -14700, 6581, -6319, 15789, -14944, 202, -10350, -2701, 25675, 6636, 22416, -11379, 13711, -447, 10659, -16505, 847, -7457, 3153, 9767, -2868, 849, -12524, 19145, -21040, -16854, -24555, -858, -8906, 8479, -6222, -16143, 12279, -185, 12406, -9723, -10228, -8205, -269, -11120, -5457, -17774, -2765, 12801, -9061, -13330, -27409, -15054, -18216, 1718, -23620, -10720, 6430, 1024, 25403, -2864, 7157, -7891, 16539, -1444, -759, -15340, 2098, 16825, 6212, 6846, -21340, 9440, -17113, -4231, -19210, -19003, -15086, 15902, 19637, -5856, 6892, 10009, 32767, 9443, 6278, -19443, 11399, 1872, 16183, 9389, -1260, 13613, 4430, 20166, -20958, 844, -15214, 17673, -2827, 7008, 11701, 4059, 32767, 20832, 20314, -14441, 20831, 319, 8543, -5941, 3437, 4019, 18967, 13983, -15617, -5403, -13263, 10847, -1837, -12089, -19912, 10405, 14497, 7787, 12395, -7878, 13711, -447, 14804, -7070, 847, -10645, 2795, 10175, -3030, -2984, -4881, 17523, -11613, -7806, -32767, -13965, 4200, -4627, -8320, -16143, 17257, -185, 12097, -20080, 1104, -21312, 12259, 903, -5457, -6880, -5449, 18378, -12257, -10765, -30539, -15054, -13119, 1718, -22747, -20487, 8605, 14130, 25403, -7244, 2406, -11385, 20899, -11735, -759, -11410, 2701, 16825, -1233, 5299, -15296, 1848, -9486, -4231, -26471, -7732, -15086, 15902, 20864, -7394, 4083, 2572, 27362, -3662, 13121, -18621, 4623, 11535, 16183, 3277, -2095, 20216, 7413, 17076, -11877, -6188, -19625, 4567, 10279, -5066, 10479, 17165, 32767, 20832, 25389, -8564, 23032, 10992, 8543, -339, -1359, 16161, 5860, 24945, -3562, -342, -6326, 16016, -11040, -2069, -19912, 7850, 23421, 6636, 15502, 1473, 26818, -447, 10659, -7070, 847, -3923, 2795, 16423, -14854, -8056, -13448, 19145, -21040, -7865, -25840, -858, -525, -3179, -6789, -3037, 20722, 10284, 25204, -22830, -9793, -21312, 12120, -3243, -5457, -5702, -2765, 18378, -17254, -13792, -32767, -9775, -16937, 1718, -26828, -22135, -2742, 6106, 25403, -2864, -3248, -16832, 7793, -14438, -13866, -21514, -4860, 5946, 6212, 6846, -15296, 7536, -17113, 2902, -26385, -20839, -15086, 2795, 11888, 5712, 17190, -1699, 22704, 9443, 169, -8867, 12672, 8233, 16183, -3716, -1260, 17817, 4533, 18863, -20850, -4615, -19625, 10334, -65, -1014, -1405, 9322, 32767, 20832, 12282, -1334, 23032, 669, 21650, -8828, -9669, 17126, 18967, 27090, -3562, -3377, -11460, 12106, -1837, -12089, -11071, 10405, 20304, 13646, 13559, -11379, 13711, 12659, 23765, -12090, 847, -2786, 10109, 9251, -15974, -4816, -6047, 15635, -20564, -7703, -29001, -13965, 4200, 8479, 559, -3037, 16251, -185, 18148, -15164, -11564, -8205, 6367, -5618, -9444, -4667, -15872, 5271, -10419, -685, -32767, -14116, -9452, -8916, -22908, -9028, 8605, 14130, 14785, -15971, 4449, -8477, 19721, -13009, -759, -20101, -5166, 9595, -6894, 4344, -25473, 9440, -17113, -4231, -30126, -12541, -1979, 10646, 20239, -5117, 4083, 1035, 32767, 4398, 5842, -10487, -434, 1872, 3076, -3254, -1260, 7109, 3280, 20986, -24984, -12262, -19286, 17673, -2827, -662, -1405, 8476, 32767, 20832, 25063, -14441, 13663, 3187, 8543, -8632, -2444, 4019, 18967, 27090, -16669, -5403, -6319, 13416, -1837, -12089, -16022, -2701, 24262, 13039, 20013, 1727, 24958, 12659, 18040, -7070, -128, 2461, 2795, 3316, -11740, 632, -12456, 16006, -21040, -16854, -25497, -13965, -8906, -4627, 4785, -3037, 16648, -185, 16337, -20008, -11564, -21312, 12837, 1758, -17373, -17774, -4193, 5271, -8549, -8150, -31585, -2281, -22559, -11387, -13721, -9028, 685, 1024, 25403, -2864, -793, -3725, 7793, -14438, -3910, -24516, -7688, 16825, 6212, -3134, -28403, 1387, -4006, 4763, -20138, -8823, -1979, 2846, 10392, 5712, 4083, 5709, 32767, -3662, 9580, -12104, 12672, 1872, 3076, 9389, -1260, 20216, -2201, 22610, -14295, -3517, -6519, 17673, 8460
};

// Variables for Sampling ==================================================
// to be safe should be a bit under 1500. If it sampled more than 1500 for some reason,
// the data gathered will be inconsistent.
uint16_t number_samples = SAMPLE_LENGTH;
uint32_t sample_period = 11; // >= MIN_SAMP_PERIOD_TIMER
int16_t samplesRawHydrophone1[SAMPLE_LENGTH];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH];

// Variables for Digital Signal Processing ==================================================
// A manual variable to filter out small peaks that dont manage to get over the threshold
const q15_t peakThreshold = 200;



void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial connected\r\n");

    // Start timer to see time it takes for everything to run
    startTime = micros();


    // Sampling (START) ==================================================
    // (1) - First part: initializing ADC before being able to use it
    Serial.println("Initialize ADC");
    // clock of teensy is 600MHz after normal boot
    clock::setup();
    adc::init();
    // defining value of register
    uint32_t ADC_reg_config;
    // WRITE_EN needs to be set to update REG, internal clock, BUSY mode active high,
    // powering off channel D because we don't need it, internal ref because nothing external connected, reference voltage to 2.5V
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC);
    // value of channel A doubles by dividing range by 2 (works as expected)
    // ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_RANGE_A);
    adc::config(ADC_reg_config);
    adc::setup();
    // adc is now ready to use

    // (2) - Second Part: Sampling the data that is then put into the ringbuffers
    // to not overfill ringbuffer, just set maximum hard set limit of number of samples to 1500
    if (number_samples > 1500) {
        number_samples = 1500;
    }
    // Start sampling
    adc::startConversion(sample_period, adc::TIMER);
    // will sample "number_samples" samples
    delayMicroseconds(sample_period * number_samples);
    // Stop Sampling
    adc::stopConversion();

    // (3) - Third Part:  The data is now in the ringbuffers. First the timestamps are processed and the each of the channels
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Getting data");
    // Saving data into array we will use further down the line
    for (uint16_t i = 0; i < number_samples; i++) {samplesRawHydrophone1[i] = (int16_t)adc::ChannelA0.get();}
    for (uint16_t i = 0; i < number_samples; i++) {samplesRawHydrophone2[i] = (int16_t)adc::ChannelA1.get();}
    for (uint16_t i = 0; i < number_samples; i++) {samplesRawHydrophone3[i] = (int16_t)adc::ChannelB0.get();}
    for (uint16_t i = 0; i < number_samples; i++) {samplesRawHydrophone4[i] = (int16_t)adc::ChannelB1.get();}
    for (uint16_t i = 0; i < number_samples; i++) {samplesRawHydrophone5[i] = (int16_t)adc::ChannelC0.get();}
    /*
        Do sample and process repeatedly:
            1: reset all the ringbuffer so that the new values are written from the start
            2: Sample again for the wanted number of samples
            3: process the new values
            4: go back to 1 and reset the ringbuffers
        Following can be implemented to sample "continuously":
            - setup adc
            - define sample number and frequency
            loop:
                - sample
                - process the data in the ringbuffer
                - reset ringbuffer for new sampling
            end of loop
    */
    // Sampling (STOP) ==================================================


    // Digital Signal Processing (START) ==================================================
    // Filter raw samples
    q15_t* samplesFiltered = filter_butterwort_9th_order_50kHz(samplesRawHydrophone1);  

    // Preform FFT calculations on filtered samples
    q15_t* FFTResultsRaw = FFT_raw(samplesFiltered);
    q15_t* FFTResults = FFT_mag(FFTResultsRaw);

    // Get peaks of frequencies that might be of interest and their useful information like amplitude, frequency and phase
    q31_t** peaks = peak_detection(FFTResultsRaw, FFTResults, peakThreshold);
    
    /*
    Since we are storing the length of the array in the first index, we do
    not start from 0 in the array when printing out. Find out how to get
    length of a 2D array of a q31_t datatype. For now we return the length of
    the array in the first index of 2D array, This must be solved, this is
    not a good solution.
    */
    int lengthOfPeakArray = peaks[0][0];

    // TIPS: For getting phase of the peak FFTs from q31 format that we dont understand to radians in floats, use this:
    // phaseQ31_to_radianFloat32(peaks[x][2]);
    // Digital Signal Processing (STOP) ==================================================


    // End timer for testing speed of algorithm
    endTime = micros();

    // Print out how long it takes to run the whole algorithm
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Timer");
    timeDiff = endTime - startTime;
    Serial.print("StartTime: ");
    Serial.println(startTime);
    Serial.print("EndTime: ");
    Serial.println(endTime);
    Serial.print("Time: ");
    Serial.println(timeDiff); 

    // Print raw sampled signal
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Raw data from hydrophone 1");
    for (uint16_t i = 0; i < number_samples; i++) {
        Serial.print(samplesRawHydrophone1[i]);
        Serial.print(", ");
    }

    // Print Filtered signal response
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Filtered samples");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(samplesFiltered[i]);
        Serial.print(", ");
    }

    // Print FFT
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("FFT");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(FFTResults[i]);
        Serial.print(", ");
    }

    // Print peaks of FFT
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Peaks");
    Serial.println(lengthOfPeakArray);
    Serial.println("[Amplitude, Frequency, Phase in radians]");
    for (int i = 1; i < lengthOfPeakArray; i++) {
        Serial.print("[");
        Serial.print(peaks[i][0]);
        Serial.print(", ");
        Serial.print(peaks[i][1]);
        Serial.print(", ");

        // Calculate phase in comprehensible manner
        // peaks[i][2] is q31_t type but in the taylor expansion it is actually q15_t, the rest of the 16 MSB are just 0,
        // The reason it is q31_t is just because we send it with all the frequencies, witch are really big and require q31_t to be stored properly
        float32_t phase_in_radians = phaseQ31_to_radianFloat32(peaks[i][2]);

        Serial.print(phase_in_radians);

        Serial.println("],");
    }
}

void loop() {
  //Serial.println("Test");
}
