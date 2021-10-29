from dataclasses import dataclass

import numpy as np

 
# @file
# @brief Parameters and constants used for the system. 
# Included parameters:
#    DSP_CONSTANTS: 
#        Sampling-frequency
#        Buffer-sizes for DMA, ADC, FFT and filter
# 
#    HYDROPHONE_DETAILS:
#        Number hydrophones
#        Hydrophone amplification
#        Hydrophone position
#        
#    TEST_PARAMETERS:
#        Position of fictional sound-source
# 
#    PHYSICAL_CONSTANTS:
#        Speed of sound in water


# DSP_CONSTANTS
# @brief
# Sampling frequency
#D Different buffer-length used
@dataclass
class DSPConstants:
    SAMPLE_FREQUENCY: float = 112500.0              # Sample frequency [Hz]
    SAMPLE_TIME: float = 1/SAMPLE_FREQUENCY         # Sample time [s]

    IN_BUFFER_LENGTH: np.uintc = 4096               # Total number (real + imaginary) measurements
                                                    # per hydrophone
    DMA_BUFFER_LENGTH: int = IN_BUFFER_LENGTH / 2   # Number real measurements transferred with DMA
    IIR_SIZE: int = IN_BUFFER_LENGTH                # Number of data-points to filter



# HYDROPHONE DETAILS
# @brief Defines that indicate the setup of the hydrophones
# These variables must be changed according to the hydrophones used.
# As of 03.01.21, three hydrophones of the type Benthowave BII-7014FG 
# is used. See the datasheet at 
# https://www.benthowave.com/products/Specs/BII-7014FGPGDatasheet.pdf
# for more information
# 
# @note It is thought that the hydrophones are
# placed in a form of a triangle. Therefore, we
# have one on front starboard, one on front port
# and one on the stern in the middle. Could be changed
# in the future
# 
#                    BOW
#                      
#      Port                       Starboard
#                      
#       HYD ''''''''''''''''''''' HYD
#           '''''''''''''''''''''
#           ''''''''' A '''''''''
#           ''''''''' U '''''''''
#           ''''''''' V '''''''''
#           '''''''''''''''''''''
#           '''''''''''''''''''''
#           '''''''''HYD'''''''''
# 
#                   STERN
# 
#    y
#    ^
#    |
#    |
#    |
#    |
# ----------------------> x
#    |
#    |
# 
# 
# @note The position of the hydrophones are relative
# to the center of the AUV
# 
# @warning The axis in the xy-plane are rotated compared 
# to the axis that mechanical works with. This implies
# that n_HYD_X equals mechanical's y-axis
@dataclass
class HydrophoneDetails:

    NUM_HYDROPHONES: int = 3                        # Number of hydrophones used on the AUV
    HYD_PREAMP_DB: int = 40                         # Number of dB the signal is preamplified
    HYD_FFVS: int = -173                            # Average FFVS for 20 - 40 kHz [db V/μPa]

    SIGNAL_GAIN: float = 1.0                        # Gain of signal (unknown as og 06.01)

    PORT_HYD_X: float = -0.11                       # x - position of port hydrophone [m]
    PORT_HYD_Y: float = 0.31                        # y - position of port hydrophone [m]
    PORT_HYD_Z: float = 0.15                        # z - position of port hydrophone [m]

    STARBOARD_HYD_X: float = 0.11                   # x - position of starboard hydrophone [m]
    STARBOARD_HYD_Y: float = 0.31                   # y - position of starboard hydrophone [m]
    STARBOARD_HYD_Z: float = 0.15                   # z - position of starboard hydrophone [m]

    STERN_HYD_X: float = 0.0                        # x - position of stern hydrophone [m]
    STERN_HYD_Y: float = -0.24                      # y - position of stern hydrophone [m]
    STERN_HYD_Z: float = 0.15                       # z - position of stern hydrophone [m]



# SYSTEM MARGINS
# @brief Defines that indicate the errors that we can allow and still 
# get an acceptable result 
@dataclass
class SystemMargins:
    MARGIN_POS_ESTIMATE: float = 0.5                # Error tolerable in estimating the position [m]
    MARGIN_POS_SEARCH: float = 0.25                 # How much the search will deviate in x      [m]

    MARGIN_INTENSITY: int = 20                      # Difference between the intensity-
    MARGIN_TIME_EPSILON: float = 0.1                # Determines the difference allowed between the
                                                    # detected lags. With LAG_DIFF_EPSILON = 0.1 we
                                                    # allow signals to arrive 1.1 * max_time and
                                                    # still count. max_time is the maximum possible
                                                    # time for sound to pass from one to another
                                                    # hydrophone.



# TEST PARAMETERS
# @brief Defines that indicate which parameters are to be tested 
@dataclass
class TestParameters:
    CURR_TESTING_BOOL: bool = 0                     # Bool to indicate if the code is being tested
                                                    # Not testing: 0
                                                    # Testing:     1
    
    SOURCE_POS_X: float = 10.0                      # x - position of sound-source
    SOURCE_POS_y: float = 2.0                       # y - position of sound-source
    SOURCE_POS_Z: float = 0.0                        # z - position of sound-source


# PHYSICAL CONSTANTS
# @brief Defines that indicate physical characteristics/constants 
# of the system
@dataclass
class PhysicalConstants:
    SOUND_SPEED: float = 1480.0                    # Speed of sound in water [m/s]



# FILTER SETUP
# @brief A fourth-order IIR-filter to filter the data before 
# processing it. By using this filter, we should (hopefully)
# eliminate unwanted frequencies. 
# 
# @warning Must have abs(filter) < 1 to prevent overflow
#
# @warning The filter is designed with the filterdesigner 
# in MATLAB, using
#      Fs    = 112500        Sampling frequency
#      Fc1   = 15000         Lower cut-off frequency
#      Fc2   = 45000         Upper cut-off frequency
#      
#      Order = 4             Filter order
#      Type  = Butterworth   Filter type
#      
# 
# The filter's transferfunction is given as 
#      H(z) = B(z) * 1 / A(z)
# where
#      B(z) = b0 + b1 * z^(-1) + b2 * z^(-2) + b3 * z^(-3) + ...
#      A(z) = 1 + a1 * z^(-1) + a2 * z^(-2) + a3 * z^(-3) + ...
# 
# NOTE: Since the function uses a biquad-filter, every filter with order > 2 is split 
# into multiple serial second-order filters, leaving at most one filter with order = 1 
# 
# Example:
#      4th order filter with the original filter function given as
#      H(z) = B(z)/A(z) with
#
#          B(z) = b0 + b1 * z^(-1) + b2 * z^(-2) + b3 * z^(-3) + b4 * z^(-4)
#          A(z) = 1 + a1 * z^(-1) + a2 * z^(-2) + a3 * z^(-3) + a4 * z^(-4)
#    
#      is instead given as H(z) = B_1(z)/A_1(z) * B_2(z)/A_2(z), where
# 
#          B_1(z) = b10 + b11 * z^(-1) + b12 * z^(-2)
#          A_1(z) = 1 + a11 * z^(-1) + a12 * z^(-2)
# 
#      and equivalent for B_2(z) and A_2(z)
# 
# NOTE: For more information, see 
# https://arm-software.github.io/CMSIS_5/DSP/html/group__BiquadCascadeDF1__32x64.html
# 
# NOTE: The MATLAB-script used to test the filter can be found under Resource/MATLAB
# 
# @param num_stages            Number of second order cascade-filters. Determines the
#                              filter order. Order = 2 * num_stages
# 
# @param state_coefficients    Initial values for x[n-1], x[n-2], ..., x[n-(2*num_stages)]
#                              y[n-1], y[n-2], ..., y[n-(2*num_stages)]
# 
# @param filter_coefficients   Filter coefficients given as {b10, b11, b12, a11, a12
#                              b20, b21, b22, a21, a22, ...}
# 
# @param IIR_FILTER            A struct describing a biquad DF1 IIR filter
# FILTER SETUP
@dataclass
class FilterSetup:
    NUM_STAGES: int = 2;                            # Number of second order cascade-filters. Determinse the
                                                    # filter order. Order = 2 * num_stages