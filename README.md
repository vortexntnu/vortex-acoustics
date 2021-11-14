# Acoustics
Repository to analyze signals from three (3) different hydrophones and estimate the positioning of a pinger in XY-coordinates.

The estimation is calculated by using TDOA to trilaterate the position. TDOA (Time Difference Of Arrival) is the difference between the measured acoustic signals. The TDOA is calculated by the cross-correlation between the measured signals. Since the hydrophones' positions are known, the sound source is calculated through some linear algebra described at https://math.stackexchange.com/questions/1722021/trilateration-using-tdoa  

Dependencies:
  CMSIS-library: DSP-library developed by ARM. Downloadable at https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
  Eigen (v. 3.3.9 used): Library for linear algebra and matrix-operations. Downloadable at http://eigen.tuxfamily.org/index.php?title=Main_Page


Important notes:
  The code is compiled on STM32CubeIDE, and the MAKEFILE may not be up to date. The Makefile is kept for the future, in case the system is compiled and run on another platform

  OBS! To prevent the compiler used at STM32CubeIDE to go mental, some defines have been moved into the direct header-files. The compiler couldn't find the defines in the other files where they originally were defined. This can be a problem if the code is used for other projects or other MCUs, and should therefore be used with caution.

## Source files
The source files for 2020/2021, can be found in the folder "Source" with the header files under Include.


Files with a short description:
  PARAMETERS: Holds defines and global parameters, such as sampling frequency, hydrophone position, buffer sizes etc.

  TRILITERATION: Mantains functions to estimate the sound source's position

  ANALYZE_DATA: Defines a class hydrophone which wraps the data-analyzis for each hydrophone


## Resource files
Resource files are found in the folder "Resource". 
The folder includes the CMSIS-library, driver for STM32.


## Matlab
The Matlab code provided contains the calculations for the digital filter used in the C++ code.


## Python
The Python folder provides code for analyzing the multilateration algorithm.


## Future improvements/development

List of improvements:
  1. Check the correctness of the code. Specialized tests should be written to accomodate this requirement.
  2. Check the RAM-usage to prevent any memory-leaks. This should be done using Valgrind or any other program

