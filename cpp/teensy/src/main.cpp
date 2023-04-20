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



#include "DSP.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>

/*
These are just to measure the time it takes to run the entire code.
The new calculations show 732 us (micro seconds) for the whole algorithm
*/
unsigned long timeDiff;
unsigned long startTime;
unsigned long endTime;

// A manual valiable to filter out small peaks that dont manage to get over the threshold
const q15_t peakThreshold = 300;

int16_t samplesRaw[SAMPLE_LENGTH] = {
-2256, -689, 913, 4660, 3775, 5389, 4846, 7176, 5475, 4341, 5075, 1562, 627, -220, -1760, -3239, -5872, -7415, -8056, -6393, -3898, -6002, -3980, -890, -839, 1969, 2250, 5372, 7845, 5764, 5414, 5910, 4125, 3048, 1536, 760, 292, -3626, -2775, -4869, -3689, -5528, -7081, -5363, -5590, -4276, -2587, -2271, 962, 982, 2498, 6255, 6286, 5564, 5928, 4472, 3079, 4209, 1943, 1260, -1439, -4785, -3712, -6172, -6181, -6649, -4853, -4321, -5631, 223, -1333, 1310, 1069, 2441, 3561, 6311, 7712, 6604, 5491, 4621, 3119, 517, 473, -669, -2843, -2965, -5775, -7163, -6788, -5239, -5285, -5506, -1498, 154, -678, 3325, 3394, 6205, 7214, 7903, 7982, 6538, 4814, 4243, 2724, 446, -1380, -1856, -5864, -6105, -6884, -5309, -6319, -4052, -4213, -3155, -1566, -774, 1642, 3151, 5331, 4558, 5973, 6663, 6567, 4120, 4517, 4000, 1880, -59, -3101, -3786, -3467, -6304, -6940, -5610, -4271, -5165, -3877, -1633, 892, 1883, 3162, 4153, 5372, 5831, 4397, 6199, 3954, 5387, 3220, 580, -842, -2002, -3615, -5342, -6797, -7494, -6973, -6272, -5682, -4890, -3953, -1990, -376, 1730, 3741, 3728, 7565, 6153, 6968, 5463, 4671, 3530, 2866, -1711, -1586, -3078, -4344, -4654, -5620, -7749, -8048, -6536, -3678, -2494, 817, -1529, 169, 3697, 5305, 8302, 6416, 6279, 4252, 5734, 1910, -125, -621, -2372, -3260, -4132, -4107, -5214, -7248, -5173, -6162, -3093, -2209, -2077, 1933, 2018, 4065, 3882, 7398, 7831, 7165, 6368, 5506, 3676, 2112, 590, 766, -3570, -4506, -6039, -5783, -5553, -7333, -6541, -4596, -2647, -3125, 2094, 1503, 2537, 4268, 5098, 6872, 6529, 5715, 7079, 3617, 1510, 285, 167, -2664, -3489, -4566, -6800, -7708, -6945, -4990, -4454, -1962, -2249, 305, 2, 4143, 4713, 5575, 5903, 6187, 4542, 6391, 3911, 2582, 2538, -7, -85, -6064, -5894, -7769, -8123, -5685, -4516, -3959, -2553, -2685, -141, 431, 1405, 3043, 6907, 7856, 6516, 7308, 8080, 5844, 3782, 300, -510, -194, -3385, -5848, -7833, -5413, -5843, -6877, -4767, -3538, -3263, 668, 415, 1951, 3906, 5913, 6429, 6198, 7429, 7294, 4032, 3768, 949, 322, 175, -1067, -4237, -6325, -5349, -5797, -6128, -6380, -2478, -2594, -1259, 299, 2666, 3955, 4052, 6648, 6669, 5928, 5132, 5152, 2718, 2277, 998, -2314, -3153, -2178, -5318, -6575, -6900, -7414, -4549, -5159, -1741, -2550, 651, 2608, 3741, 5393, 6389, 6299, 6753, 5993, 4005, 2873, 956, 451, 1194, -2081, -4770, -5225, -6301, -6496, -6212, -6122, -5966, -3803, -1713, -537, 1954, 2639, 4494, 7613, 6579, 6016, 3607, 7091, 3658, 4038, 1928, 426, -2169, -3679, -6580, -6199, -8378, -4366, -6195, -6934, -5594, -944, -1597, 732, 3479, 5179, 6941, 4778, 5202, 7227, 4741, 3998, 4009, 1341, 71, -1640, -4215, -5553, -6418, -5287, -5237, -4892, -5067, -3013, -2628, -531, 2409, 3154, 6245, 4510, 5629, 6274, 7213, 5383, 4321, 4053, 1077, -277, -2048, -2287, -5715, -6671, -8784, -6347, -6361, -3510, -5675, -3807, -128, 1103, 754, 3530, 6181, 6220, 5898, 8077, 7489, 4592, 2510, 3412, -147, -4346, -1603, -7506, -4210, -6335, -5903, -6816, -4758, -2523, -1989, -1381, 1085, 492, 5292, 4356, 4844, 7229, 4993, 5875, 3820, 5085, 690, 355, -2291, -2045, -4778, -5529, -5720, -5634, -4924, -4128, -3668, -2867, -3881, -840, 3520, 2657, 3302, 5565, 8144, 5759, 5923, 3773, 1975, 3393, 2295, -1333, -3310, -4244, -5651, -5518, -5773, -8365, -6079, -4334, -2531, -1029, -1292, 1115, 4242, 5769, 6609, 7047, 7604, 5043, 3019, 4751, 1796, 1166, 30, -1805, -5381, -5732, -7058, -6856, -5179, -5367, -4158, -4616, -1921, -1005, 3210, 3552, 4739, 5402, 5893, 6494, 5406, 6666, 4507, 2586, 2242, -2289, -2604, -5285, -2716, -6680, -7836, -7463, -6452, -5144, -5190, -2478, -1183, 713, 1510, 4618, 6519, 6334, 6455, 7197, 6765, 4933, 3806, 1596, 351, -412, -2407, -3389, -4155, -7240, -6499, -5820, -4986, -6121, -1063, -1650, -642, 446, 5958, 5515, 5787, 6376, 4945, 4655, 2704, 3367, 2829, -122, -2385, -5056, -3279, -3917, -5221, -6023, -6228, -6224, -3601, -3191, -1445, 2580, -175, 3304, 5547, 5480, 5332, 7363, 6849, 5010, 5142, 3169, 1946, -13, -2156, -5469, -5953, -5228, -5134, -7383, -5817, -5160, -1932, -2012, 1722, 337, 3127, 4965, 5411, 5404, 6807, 7935, 6172, 4004, 2420, 835, -833, 1087, -4706, -5778, -6887, -6151, -5608, -5428, -5447, -5066, -2443, 1010, 1271, 2942, 4977, 5126, 5705, 6030, 6785, 5625, 3873, 4264, -57, -260, -205, -3485, -4296, -5177, -6692, -7323, -7802, -5254, -4911, -1776, -2332, 2339, 3163, 3765, 4756, 4912, 6739, 7246, 6401, 3130, 3864, 997, 706, 543, -3995, -4500, -5970, -4606, -6490, -5923, -4686, -4867, -3606, -1270, 1222, 1830, 2802, 5820, 6485, 6213, 5996, 5637, 4727, 953, 1384, 1556, -338, -2547, -3389, -6889, -6025, -5779, -6871, -5530, -4955, -3601, -1510, 591, 584, 4053, 4497, 4449, 8785, 5272, 7277, 7005, 3397, 2411, 1344, -667, -3464, -4183, -5210, -5409, -5772, -5737, -5186, -4900, -3038, -2187, -1772, 2560, 3229, 3327, 4611, 6677, 6685, 6210, 3150, 2972, 3907, 1057, -969, -3416, -2720, -4285, -7906, -6344, -5373, -5244, -5107, -2871, -2777, 83, 1810, 4373, 4239, 5878, 8409, 7972, 5980, 4615, 3587, 3465, 1289, -1819, -4013, -4685, -4230, -7280, -4981, -5421, -4809, -4795, -3444, -3109, -382, 319, 1797, 6139, 5958, 7195, 6540, 7324, 7314, 5613, 3171, 2756, 180, -1423, -5531, -4495, -6125, -5335, -6589, -6197, -5074, -5454, -1509, 66, 2126, 2413, 4760, 5031, 6824, 5982, 8046, 4088, 5134, 2076, 2828, 931, -1952, -3158, -5299, -2869, -6083, -6614, -5494, -4621, -4137, -1974, -1403, 545, 3293, 4968, 4163, 5093, 5915, 5566, 6358, 5306, 3659, 1381, -440, -1867, -1597, -5297, -5912, -4798, -6888, -5765, -7232, -5869, -2041, -1606, 995, 3470, 2088, 5334, 7389, 6661, 7518, 5182, 4186, 4470, 1785, 1487, 344, -1979, -3952, -5748, -6519, -6207, -6735, -4483, -5556, -2336, -1084, -670, 2278, 3017, 5092, 4613, 6344, 7331, 6101, 6207, 3958, 3713
};

void setup() {
    Serial.begin(9600);
    delay(3000);
    Serial.println("Test");

    // Start timer to see time it takes for everything to run
    startTime = micros();


    // Digital Signal Processing (START) ==========
    q15_t* samplesFiltered = filter_butterwort_9th_order_50kHz(samplesRaw);  

    q15_t* FFTResultsRaw = FFT_raw(samplesFiltered);
    q15_t* FFTResults = FFT_mag(FFTResultsRaw);

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
    // Digital Signal Processing (STOP) ==========


    // End timer for testing speed of algorithm
    endTime = micros();

    // Print out how long it takes to run the whole algorithm
    timeDiff = endTime - startTime;
    Serial.print("StartTime: ");
    Serial.println(startTime);
    Serial.print("EndTime: ");
    Serial.println(endTime);
    Serial.print("Time: ");
    Serial.println(timeDiff); 

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
