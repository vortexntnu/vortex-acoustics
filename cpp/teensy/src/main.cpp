#include <Arduino.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#define SAMPLE_RATE 500000
#define SAMPLE_LENGTH 1024
#define BITSHiFT 9
#define SCALE_FACTOR 1000.0
#define NUM_TAPS 10

int16_t samples_raw[SAMPLE_LENGTH] = {
 -593   496  1372  2218  2673  2492  3245  3134  3091  3571  1865  2749
  1768   731   265  -327  -846 -2058 -2463 -2444 -3388 -3576 -3268 -2789
 -2043 -1620 -1413 -1023  -404   746  1770  1678  2284  2735  3307  3731
  3187  3024  2981  2652  1912   877   841  -727  -920 -1701 -2128 -2935
 -2858 -2806 -3562 -3280 -3482 -2574 -1211 -1220  -389   259  1051  2070
  1919  3295  3032  3614  3224  3296  2484  2634  1792   828   239    59
  -472 -1482 -2109 -2296 -2387 -3316 -3046 -3404 -2460 -2869 -1807  -626
  -739  -236   956  1310  2062  2448  2852  3121  3058  3538  2665  2909
  2685  1668   502  -656  -461 -1439 -1961 -2150 -2921 -2970 -3550 -3589
 -3077 -2530 -2608 -1621  -632    22   516  1242  1717  2442  2931  2581
  3248  3331  3249  3253  2454  1556   773   279  -477 -1381 -1665 -1798
 -2617 -3454 -3557 -3208 -3322 -3153 -2547 -2568 -1139  -736   180   674
  1330  2523  3095  3662  3126  3507  2557  3565  2194  2232   768   893
  -755  -896 -2223 -2534 -2980 -3367 -2879 -3342 -2860 -2606 -2472 -2489
 -1260  -342   218   430  1198  1874  2615  3209  3007  2809  3297  2410
  2422  2220  1605  1046   150   106 -1269 -1414 -2008 -2539 -3034 -2986
 -2862 -2683 -3424 -2069 -1418 -1811   116  1132  1049  2100  2610  2430
  3045  3688  2685  3319  3036  2729  1168  1027   160  -220 -2158 -1544
 -2085 -2965 -3531 -3130 -2910 -3507 -3125 -2169 -1848 -1763  -709   681
   871  1644  2806  2962  3134  3097  2964  3006  2700  2568  2174   978
   -88   -89  -708  -813 -2081 -2370 -3202 -2628 -3899 -2952 -2705 -2669
 -1364 -1072  -687   263   929  2171  2343  2997  3132  2744  2747  3422
  2896  1956  2430  1398   164    -5  -728  -876 -2272 -2663 -2635 -3904
 -3566 -2947 -2852 -3085 -2288 -2022  -730   343   258  1399  1787  2392
  2603  3275  3534  3036  3610  2940  2139  1241   882   631  -514 -1827
 -1038 -1784 -2814 -3503 -2842 -3238 -3320 -3236 -2325 -1694 -1897  -278
   466  1700  2216  2180  2179  2690  3065  2653  3382  2907  2371  1498
  1178   498  -460 -1257 -1528 -1259 -2520 -3279 -3700 -3309 -3070 -2714
 -3201 -2108 -1423  -616   292   719  1710  2099  2547  2482  3195  3675
  2625  3061  2452  2320  1012  1182   -58  -774 -1587 -2533 -2345 -2595
 -2968 -3093 -3359 -3363 -2729 -3259  -708  -508 -1003   463  1139  1536
  1532  3231  3226  3684  3121  3711  2349  2525  1347  1073   116  -638
 -1368 -1583 -2249 -2531 -2731 -3361 -3679 -3357 -3031 -2659 -1141 -1440
  -553   377   952  1748  2071  3240  2768  3291  3041  3531  2792  2515
  1693   870   349  -189 -1074 -1524 -2090 -2905 -3152 -3164 -3590 -3089
 -2900 -2563 -2573  -724  -813  -169  1060  1868  2206  2620  2784  2867
  3227  3018  3217  3198  2488  1587  1075   213 -1087 -1290 -1939 -2383
 -2662 -2818 -2924 -3282 -3183 -2315 -1641 -1895  -863  -564   366  1219
  1288  2904  2468  3057  3080  3077  3402  2874  2258   940   625   173
  -555  -820 -1831 -2812 -3020 -2762 -3378 -2932 -2981 -2932 -2016 -1995
 -1634  -381   300  1198  1561  2206  2775  2806  3552  2816  2575  2256
  2640  2354  1596   -61  -173  -861 -1733 -2274 -2794 -3340 -3734 -3371
 -3520 -2860 -2558 -1726 -1446 -1390  -409   312  1076  1828  2439  2847
  3753  3061  3106  3854  2565  2347   874   657  -222  -706 -1133 -1585
 -2919 -2632 -3301 -3442 -3281 -3007 -2654 -2085 -1490 -1384  -690    69
  1486  3289  2222  2916  2989  3132  3037  3495  2986  2348  1747   655
   620  -958 -1714 -2097 -2449 -2618 -3036 -3354]};

int16_t samples[SAMPLE_LENGTH];
/*
q15_t coefficients[NUM_TAPS];
q15_t coeffBuffer[NUM_TAPS + SAMPLE_LENGTH + 1];

arm_iir_lattice_instance_q15 iir_instance;
*/

//int16_t samples[SAMPLE_LENGTH];
int16_t samplesScaled[SAMPLE_LENGTH];
int16_t results[2*SAMPLE_LENGTH]; //To store the results of fft with complex numbers, need to have double the size of the sample length z = a + bi, {a1, b1, a2, b2, a3, b3 ... }
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_instance_q15 fft_instance;
int16_t results_mag[SAMPLE_LENGTH]; //


void setup() {
Serial.begin(9600); // Initialize the FFT
Serial.println("Serial output begun");

int16_t xn1 = 0;
int16_t yn1 = 0;
//int16_t k = 0; 

for (int i = 0; i < SAMPLE_LENGTH; i++)
{
  samples[i] = (int16_t)10000*(0.969*yn1 + 0.0155*samples_raw[i] + 0.0155*xn1);

  xn1 = samples_raw[i];
  yn1 = samples[i];
  Serial.print(samples[i]); Serial.print(", ");
}
Serial.println("Filtered");

/*
for (int i = 0; i < SAMPLE_LENGTH; i++) {
samples[i] = (q15_t)i;
}
Serial.println("Iterated through samples");

// Initialize the coefficients of the filter
for (int i = 0; i < NUM_TAPS; i++) {
coefficients[i] = (q15_t)(i + 1);
}
Serial.println("Iterated through coefficients");

// Initialize the instance structure of the IIR filter
arm_iir_lattice_init_q15(&iir_instance, NUM_TAPS, coefficients, NULL, NULL, SAMPLE_LENGTH);
Serial.println("Initialized instance structure");

// Apply the IIR filter to the input signal
arm_iir_lattice_q15(&iir_instance, (q15_t*)samples, (q15_t*)samples, SAMPLE_LENGTH);
Serial.println("Applied IIR filter");
*/

/*arm_rfft_init_q15(&fft_instance, SAMPLE_LENGTH, ifftFlag, doBitReverse); // Generate imperfect sinusoidal signal at 30kHz
Serial.println("rfft initialized");
arm_scale_q15(samples, SCALE_FACTOR, BITSHiFT, samplesScaled, SAMPLE_LENGTH); //Scale samples

Serial.println("Made samples"); //For troubleshooting
arm_rfft_q15(&fft_instance, samplesScaled, results); // The result of the FFT is stored in the results array
Serial.println("Results created");
// Use the magnitude function to compute the magnitude of the complex result
arm_cmplx_mag_q15(results, results_mag, SAMPLE_LENGTH); // Convert the magnitude to dB and store in the samples array

Serial.println("fft output"); //Printing the fft output for a better understanding of what is going on
for(int i = 0; i < SAMPLE_LENGTH; i++){
Serial.print(results_mag[i]);
if(i < (SAMPLE_LENGTH - 1)){
Serial.print(", ");
} else{
Serial.println("");
}
}

Serial.print("Frequencies: ");
for (int i = 0; i < SAMPLE_LENGTH/2; i++)
{
if(results_mag[i] > 0){
float total_amp = results_mag[i] + results_mag[i+1]; //Calculate total amplitude of neighboring non-zero indexes.
float prop1 = results_mag[i]/total_amp; //Calculate proportion of index 1 amplitude to total amplitude
float prop2 = results_mag[i+1]/total_amp; //Ditto for index 2
float temp_freq = (float)(prop1*i * SAMPLE_RATE / SAMPLE_LENGTH) + (float)(prop2*(i+1) * SAMPLE_RATE / SAMPLE_LENGTH); //We use the proportions instead of the straight average of 2s
Serial.print(temp_freq); 
Serial.print(", ");
i++;
}
}
Serial.println("");
*/
}

void loop() {}
