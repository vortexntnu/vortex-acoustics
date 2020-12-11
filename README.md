# Acoustics
Repository to analyze signals from three (3) different hydrophones and estimate the positioning of a pinger in XY-coordinates.\

Dependencies:\
  The DSP and the analysis depends on code developed by ARM. This includes both FFT/IFFT, autocorrelation and DF1-filtering. All of the required files can be found at CMSIS at https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html


Important notes:\
  Dependencies are NOT included in this repository! These must be downloaded by oneself. This is to prevent the github-page exceeding almost one million lines of code.\

  The code is compiled on STM32CubeIDE, however the IDE often has problems reading/understanding the types int/uint8_t/uint16_t/float32_t and so forth. No solution has been discovered as of 11.12.2020.\
  

# Development

The files for 2020/2021, can be found in the folder "2020_2021 / code_acoustics". Older files are not considered relevant, and is therefore not in this repository.\

Files with a short description:\
  DSP: originally intended to hold the functions for the DSP (digital signal processing), however holds the DSP-constants at the moment. The only file that includes files from ARM\

  TRILITERATION: Mantains the functions to calculate an estimate for heading and distance to the sound-source.\

  HYDROPHONES: Wrapper for each hydrophone and the functions that maintain this. The functions could be moved to main.cpp, however that could reduce the readibility\

# Future improvements/development

List of improvements:\
  1. Read the data from the ADC from each channel
  2. Check the correctness of the code. Specialized tests should be written to accomodate this requirement.
  3. Find a way to calculate the sound-intensity to estimate the distance from the sound-source
  4. Check the RAM-usage to prevent any memory-leaks. This should be done using Valgrind or any other program
   

