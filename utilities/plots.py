import logging

import matplotlib.pyplot as plt
import numpy as np
import signal_generation.positioning as sg_pos
from multilateration import multilateration as ml
from multilateration import parameters
from utilities import hyperboles, tdoa
from utilities.drone import Drone

XKCD_COLORS = [
    "xkcd:purple",
    "xkcd:green",
    "xkcd:blue",
    "xkcd:pink",
    "xkcd:brown",
    "xkcd:light blue",
    "xkcd:teal",
    "xkcd:orange",
    "xkcd:light green",
    "xkcd:magenta",
    "xkcd:yellow",
    "xkcd:sky blue",
    "xkcd:grey",
    "xkcd:lime green",
    "xkcd:light purple",
    "xkcd:violet",
    "xkcd:dark green",
]


def plot_positions(
    hydrophone_positions,
    source_position,
    result_positions,
    result_labels=None,
):
    if (result_labels is not None) and (len(result_positions) != len(result_labels)):
        raise ValueError(
            "When labels are given must be equal to amount of given results"
        )

    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1, projection="3d")

    # plot hydrophone positions
    for index, position in enumerate(hydrophone_positions):
        ax.plot(
            [position.x],
            [position.y],
            [position.z],
            "bx",
            label=f"receiver {index+1}, pos=({position.x},{position.y},{position.z})",
        )

    # plot source position
    ax.plot(
        [source_position.x],
        [source_position.y],
        [source_position.z],
        "gx",
        label=f"source, pos=({source_position.x},{source_position.y},{source_position.z})",
        markersize=16,
    )

    # plot result positions
    for index, position in enumerate(result_positions):
        if result_labels is not None:
            ax.plot(
                [position.x],
                [position.y],
                [position.z],
                "o",
                label=result_labels[index],
            )
        else:
            ax.plot([position.x], [position.y], [position.z], "o", label="")

    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.legend()
    plt.set_loglevel("info")
    plt.show()


def plot_estimates_from_grid(
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
    logging.debug(f"hydrophone positions: {drone.hydrophone_positions}")

    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1, projection="3d")

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
                logging.debug(f"drone position: {drone.position}")
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
                logging.debug(f"estimate: {estimate}")

                # plot position
                ax.plot([x], [y], [z], "bx")

                # plot estimate
                ax.plot(
                    [estimate.x],
                    [estimate.y],
                    [estimate.z],
                    "gx",
                )

    ax.plot(
        [source_position.x],
        [source_position.y],
        [source_position.z],
        "rx",
    )

    # plot settings
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    plt.set_loglevel("info")
    plt.show()


def plot_estimates_along_path(
    source_position: sg_pos.Position,
    hydrophone_positions: np.array,
    path_positions: np.array,
    sample_frequency: float = 100000,
):
    drone = Drone(
        hydrophone_positions=hydrophone_positions,
    )

    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1, projection="3d")

    # plot source
    ax.plot(
        [source_position.x],
        [source_position.y],
        [source_position.z],
        "rx",
        label="Source",
    )

    for index, position in enumerate(path_positions):
        drone.position = sg_pos.Position(
            x=position.x,
            y=position.y,
            z=position.z,
        )
        logging.debug(f"drone position: {drone.position}")

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
        logging.debug(f"estimate: {estimate}")

        # plot position
        ax.plot(
            [position.x],
            [position.y],
            [position.z],
            color=XKCD_COLORS[index],
            label=f"Position {index}",
            marker="o",
        )

        # plot estimate
        ax.plot(
            [estimate.x],
            [estimate.y],
            [estimate.z],
            color=XKCD_COLORS[index],
            label=f"Estimate {index}",
            marker="1",
        )

    # plot settings
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.legend()
    plt.set_loglevel("info")
    plt.show()


def plot_hyperboles(
    source_positions,
    hydrophone_positions,
    max_range: float = 20,
):
    colors = ["b", "c", "m", "y", "k"]

    sample_frequency = 1000000

    for index, coordinate in enumerate(source_positions):
        source_position = sg_pos.Position(
            x=coordinate[0],
            y=coordinate[1],
            z=0,
        )
        logging.debug(f"source position: {source_position}")

        # calculate hyperboles
        tdoa_lag_array = tdoa.generate_lag_array(
            hydrophone_positions=hydrophone_positions,
            source_position=source_position,
            sample_frequency=sample_frequency,
        )[:, 0][1:]

        distance_of_arrival = (
            tdoa_lag_array / sample_frequency * parameters.PhysicalConstants.SOUND_SPEED
        )

        pos_hyperbole_one, neg_hyperbole_one = hyperboles.generate_hyperbole_points(
            distance_difference=distance_of_arrival[0],
            receiver_positions=(
                hydrophone_positions[0],
                hydrophone_positions[1],
            ),
            max_range=max_range,
        )
        pos_hyperbole_two, neg_hyperbole_two = hyperboles.generate_hyperbole_points(
            distance_difference=distance_of_arrival[1],
            receiver_positions=(
                hydrophone_positions[0],
                hydrophone_positions[2],
            ),
            max_range=max_range,
        )

        # plot hyperboles
        color = colors[index % len(colors)]
        logging.debug(f"hyperbole color: {color}")

        plt.plot(pos_hyperbole_one[0], pos_hyperbole_one[1], f"{color}-")
        plt.plot(neg_hyperbole_one[0], neg_hyperbole_one[1], f"{color}-")
        plt.plot(pos_hyperbole_two[0], pos_hyperbole_two[1], f"{color}-")
        plt.plot(neg_hyperbole_two[0], neg_hyperbole_two[1], f"{color}-")

        # plot receivers and source
        for position in hydrophone_positions:
            plt.plot(position.x, position.y, "gx")
        plt.plot(source_position.x, source_position.y, "rx")

    # plot settings
    plt.grid()
    plt.xlim((-1) * max_range, max_range)
    plt.ylim((-1) * max_range, max_range)
    plt.set_loglevel("info")
    plt.show()
