from dataclasses import dataclass

import numpy as np


### Initializing global constants ###

# HYDROPHONE DETAILS
@dataclass
class HydrophoneDetails:

    NUM_HYDROPHONES: int = 3                # Number of hydrophones used on the AUV
    HYD_PREAMP_DB: int = 40                 # Number of dB the signal is preamplified
    HYD_FFVS: int = -173                    # Average FFVS for 20 - 40 kHz [db V/μPa]

    SIGNAL_GAIN: float = 1.0

    PORT_HYD_X: float = -0.11               # x - position of port hydrophone [m]
    PORT_HYD_Y: float = 0.31                # y - position of port hydrophone [m]
    PORT_HYD_Z: float = 0.15                # z - position of port hydrophone [m]

    STARBOARD_HYD_X: float = 0.11           # x - position of starboard hydrophone [m]
    STARBOARD_HYD_Y: float = 0.31           # y - position of starboard hydrophone [m]
    STARBOARD_HYD_Z: float = 0.15           # z - position of starboard hydrophone [m]

    STERN_HYD_X: float = 0.0                # x - position of stern hydrophone [m]
    STERN_HYD_Y: float = -0.24              # y - position of stern hydrophone [m]
    STERN_HYD_Z: float = 0.15               # z - position of stern hydrophone [m]



# SYSTEM MARGINS
@dataclass
class SystemMargins:
    MARGIN_POS_ESTIMATE: float = 0.5        #
    MARGIN_POS_SEARCH: float = 0.25

    MARGIN_INTENSITY: int = 20
    MARGIN_TIME_EPSILON: float = 0.1





# snake_case : functions, variables
# PascalCase : classes, exceptions
# CAPITAL_SNAKE_CASE : global constants