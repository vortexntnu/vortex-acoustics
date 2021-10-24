from dataclasses import dataclass

import numpy as np


### Initializing global constants ###

# HYDROPHONE DETAILS
@dataclass
class HydrophoneDetails:
    NUM_HYDROPHONES: int             # Number of hydrophones used on the AUV

    PORT_HYD_X: float               # x - position of port hydrophone [m]
    PORT_HYD_Y: float               # y - position of port hydrophone [m]
    PORT_HYD_Z: float               # z - position of port hydrophone [m]

    STARBOARD_HYD_X: float          # x - position of starboard hydrophone [m]
    STARBOARD_HYD_Y: float          # y - position of starboard hydrophone [m]
    STARBOARD_HYD_Z: float          # z - position of starboard hydrophone [m]

    STERN_HYD_X: float              # x - position of stern hydrophone [m]
    STERN_HYD_Y: float              # y - position of stern hydrophone [m]
    STERN_HYD_Z: float              # z - position of stern hydrophone [m]


# SYSTEM MARGINS
MARGIN_POS_ESTIMATE = 0.5





# snake_case : functions, variables
# PascalCase : classes, exceptions
# CAPITAL_SNAKE_CASE : global constants