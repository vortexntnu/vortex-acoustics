# Acoustics
Repository to analyze signals from three (3) different hydrophones and estimate the positioning of a pinger in XY-coordinates.

Dependencies:
  CMSIS-library: DSP-library developed by ARM. Found at https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
  MANYEARS-library: Old library specialized for trilateration. Found at https://github.com/introlab/manyears 


Important notes:
  The ODAS-library is an updated version of the MANYEARS-library. It can be found at https://github.com/introlab/odas, however it may not work perfectly for trilateration

  The code is compiled on STM32CubeIDE, and the MAKEFILE may not be up to date. Mainly kept for the future, in case the system is compiled and run on another platform

# Source files
The source files for 2020/2021, can be found in the folder "Source" with the header files under Include. Older files are not considered relevant, and is therefore not in this repository.


Files with a short description:
  DSP_CONSTANTS: originally intended to hold the functions for the DSP (digital signal processing), however holds the DSP-constants at the moment. The only file that includes files from ARM

  TRILITERATION: Mantains the functions to calculate an estimate for heading and distance to the sound-source.

  HYDROPHONES: Wrapper for each hydrophone and the functions that maintain this. The functions could be moved to main.cpp, however that could reduce the readibility



# Resource files
Resource files are found in the folder "Resource". 
The folder includes the CMSIS- and MANYEARS-library, driver for STM32 and a test-script in MATLAB.


# Future improvements/development

List of improvements:
  1. Check the correctness of the code. Specialized tests should be written to accomodate this requirement.
  2. Check the RAM-usage to prevent any memory-leaks. This should be done using Valgrind or any other program
   

