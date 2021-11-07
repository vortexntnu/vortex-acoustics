"""Contains parameters and constants used for the multilateration algorithm.
"""

from dataclasses import dataclass

import numpy as np


@dataclass
class DSPConstants:
    """Constants for digital signal processing.
    
    Attributes:
        SAMPLE_FREQUENCY: Float for the ADC sampling frequency [Hz]
        SAMPLE_TIME: Float for the ADC sample time [s]
        IN_BUFFER_LENGTH: uintc for total number of (real + imaginary) measurements per hydrophone
    """

    SAMPLE_FREQUENCY: float = 112500.0
    SAMPLE_TIME: float = 1 / SAMPLE_FREQUENCY
    IN_BUFFER_LENGTH: np.uintc = 4096


@dataclass
class HydrophoneDetails:
    """Parameters that indicate the setup of the hydrophones.
    
    Variables must be changed according to the hydrophones used.
    Datasheet for currently used hydrophones:
    https://www.benthowave.com/products/Specs/BII-7014FGPGDatasheet.pdf

    
    It is thought that the hydrophones are
    placed in a form of a triangle. Therefore, we
    have one on front starboard, one on front port
    and one on the stern in the middle. Could be changed
    in the future.
                    BOW
        Port                       Starboard
        HYD ''''''''''''''''''''' HYD
            '''''''''''''''''''''
            ''''''''' A '''''''''
            ''''''''' U '''''''''
            ''''''''' V '''''''''
            '''''''''''''''''''''
            '''''''''''''''''''''
            '''''''''HYD'''''''''
                    STERN
        y
        ^
        |
        |
        |
        |
    ----------------------> x
        |
        |
    The position of the hydrophones are relative
    to the center of the AUV
    The axis in the xy-plane are rotated compared
    to the axis that mechanical works with. This implies
    that n_HYD_X equals mechanical's y-axis

    Attributes:
        NUM_HYDROPHONES: Int for the number of hydrophones used on the AUV

        PORT_HYD_X: x - position of port hydrophone [m]
        PORT_HYD_Y: y - position of port hydrophone [m]
        PORT_HYD_Z: z - position of port hydrophone [m]

        STARBOARD_HYD_X: x - position of starboard hydrophone [m]
        STARBOARD_HYD_Y: y - position of starboard hydrophone [m]
        STARBOARD_HYD_Z: z - position of starboard hydrophone [m]

        STERN_HYD_X: x - position of stern hydrophone [m]
        STERN_HYD_Y: y - position of stern hydrophone [m]
        STERN_HYD_Z: z - position of stern hydrophone [m]
    """

    NUM_HYDROPHONES: int = 3

    PORT_HYD_X: float = -0.11
    PORT_HYD_Y: float = 0.31
    PORT_HYD_Z: float = 0.0

    STARBOARD_HYD_X: float = 0.11
    STARBOARD_HYD_Y: float = 0.31
    STARBOARD_HYD_Z: float = 0.0

    STERN_HYD_X: float = 0.0
    STERN_HYD_Y: float = -0.24
    STERN_HYD_Z: float = 0.0


@dataclass
class TestParameters:
    SOURCE_POS_X: float = 10.0
    SOURCE_POS_Y: float = 2.0 
    SOURCE_POS_Z: float = 0.0


@dataclass
class PhysicalConstants:
    SOUND_SPEED: float = 1480.0