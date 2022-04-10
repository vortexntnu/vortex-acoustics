import logging
from statistics import mean

#import matplotlib.pyplot as plt
import numpy as np
import signal_generation.positioning as sg_pos
from multilateration import multilateration as ml
#from multilateration import parameters
from utilities import tdoa
from utilities.drone import Drone

import accumulate_position_estimates as acc


def generate_estimates(
    source_position: sg_pos.Position,
    hydrophone_positions: np.array,
    sample_frequency: float = 100000,
):
    """
    Args:
        source_position: for useful plots make sure source positidebugon is in positive octant
        hydrophone_positions: give hydrophone positions relative to origo (0,0,0)
    """

    estimates = []
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

                estimate = acc.Position(
                    x=res_x,
                    y=res_y,
                    z=res_z,
                )

                estimates.append(estimate)

    return estimates

                

def test_integrate_new_measurement(): 

    source_position = sg_pos.Position(10, 10, 10)

    hydrophone_positions = np.array(
    [
        sg_pos.Position(
            x=-0.11,
            y=0.31,
            z=0.1,
        ),
        sg_pos.Position(
            x=0.11,
            y=0.31,
            z=0.1,
        ),
        sg_pos.Position(
            x=0.0,
            y=-0.24,
            z=0.0,
        ),
        sg_pos.Position(
            x=0.5,
            y=-0.1,
            z=0.4,
        ),
        sg_pos.Position(
            x=0.4,
            y=0.0,
            z=-0.4,
        ),
    ])
    sampling_frequency = 320

    estimates = generate_estimates(source_position, hydrophone_positions, sampling_frequency)

    computed_pinger_position = acc.PositionEstimate()

    for estimate in estimates:
        computed_pinger_position.integrate_new_measurement(estimate)

    tolerance = 0.5

    assert tolerance < abs(source_position.x - computed_pinger_position.mean.x)
    assert tolerance < abs(source_position.y - computed_pinger_position.mean.y)
    assert tolerance < abs(source_position.z - computed_pinger_position.mean.z)


