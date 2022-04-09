import logging

#import matplotlib.pyplot as plt
import numpy as np
import signal_generation.positioning as sg_pos
from multilateration import multilateration as ml
#from multilateration import parameters
from utilities import tdoa
from utilities.drone import Drone


def generate_estimate(
    source_position: sg_pos.Position,
    hydrophone_positions: np.array,
    sample_frequency: float = 100000,
):
    """
    Args:
        source_position: for useful plots make sure source positidebugon is in positive octant
        hydrophone_positions: give hydrophone positions relative to origo (0,0,0)
    """
    drone = Drone(
        hydrophone_positions=hydrophone_positions,
    )


    xs = np.linspace(0, source_position.x, 6)[:-1]
    ys = np.linspace(0, source_position.y, 6)[:-1]
    zs = np.linspace(0, source_position.z, 6)[:-1]

    for x in xs:
        for y in ys:
            for z in zs:
                drone.position = sg_pos.Position(
                    x=x,
                    y=y,
                    z=z,
                )
                lag_array = tdoa.generate_lag_array(
                    hydrophone_positions=drone.hydrophone_positions,
                    source_position=source_position,
                    sample_frequency=sample_frequency,
                )[:, 0][1:]

                res_x, res_y, res_z = ml.calculate_pinger_position(
                    tdoa_lag_array=lag_array,
                    hydrophone_positions=hydrophone_positions,
                    sample_frequency=sample_frequency,
                )

                estimate = sg_pos.Position(
                    x=res_x,
                    y=res_y,
                    z=res_z,
                )