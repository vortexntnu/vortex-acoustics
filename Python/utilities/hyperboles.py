import logging
from typing import Tuple

import numpy as np

from signal_generation.positioning import Position


def generate_hyperbole_points(
    distance_difference: float,
    receiver_positions: Tuple[Position, Position],
    max_range: float = 1000.0,
):
    """Generates the hyperboles for two given receiver positions and distance difference of the arrivals.

    Args:
        distance_difference: The difference in distance, usually calculated from the time difference of the arrivals
            and the speed of light / sound.
        receiver_positions: A tuple of two receiver positions.
        max_range: The range for which the points of the hyperboles are generated. E.g., if it's a hyperbole with
            vertically aligned foci, the range would be [-1000,1000] for the x-coordinates.

    Returns:
        Two arrays where one is the used x-coordinate values and the other are the corresponding y-coordinate values.
    """
    position_one = receiver_positions[0]
    position_two = receiver_positions[1]

    x_offset = (position_one.x + position_two.x) / 2
    y_offset = (position_one.y + position_two.y) / 2

    delta_x = position_two.x - position_one.x
    delta_y = position_two.y - position_one.y

    logging.debug(f"x_offset: {x_offset}")
    logging.debug(f"y_offset: {y_offset}")

    logging.debug(f"delta_x: {delta_x}")
    logging.debug(f"delta_y: {delta_y}")

    logging.debug(f"position one: {position_one}")
    logging.debug(f"position two: {position_two}")

    # Positions in specific coordinate system
    distance = abs(position_one - position_two)
    spec_position_one = Position(
        x=0,
        y=distance / 2,
    )
    spec_position_two = Position(
        x=0,
        y=(-1) * distance / 2,
    )

    logging.debug(f"specific position one: {spec_position_one}")
    logging.debug(f"specific position two: {spec_position_two}")

    # Foci are aligned vertically
    x = np.linspace(-max_range, max_range, 10000)

    a = distance_difference / 2  # why divided by 2
    c = abs(spec_position_one.y)

    y_pos = a * np.sqrt(x ** 2 / (c ** 2 - a ** 2) + 1)
    y_neg = -1.0 * y_pos

    # group_coordinates in tuples
    positive_coordinates = []
    negative_coordinates = []

    for index in range(0, len(y_pos)):
        positive_coordinates.append((x[index], y_pos[index]))
        negative_coordinates.append((x[index], y_neg[index]))

    # rotate coordinates
    if delta_y == 0:
        if position_two.x > position_one.x:
            rotation_angle = (-1) * np.pi / 2
        else:
            rotation_angle = np.pi / 2
    else:
        rotation_angle = np.arctan(delta_x / delta_y)

    logging.debug(f"rotational angle: {rotation_angle} rad")
    logging.debug(f"rotational angle: {360*rotation_angle/2/np.pi} degree")
    rotated_pos_coords = []
    rotated_neg_coords = []

    for index in range(0, len(y_pos)):
        rotated_pos_coords.append(
            rotate_coordinates(
                coordinate=positive_coordinates[index],
                theta=rotation_angle,
            )
        )
        rotated_neg_coords.append(
            rotate_coordinates(
                coordinate=negative_coordinates[index],
                theta=rotation_angle,
            )
        )

    # translate coordinates
    final_pos_coords = []
    final_neg_coords = []

    for index in range(0, len(y_pos)):
        final_pos_coords.append(
            translate_coordinate(
                coordinate=rotated_pos_coords[index],
                delta_x=x_offset,
                delta_y=y_offset,
            )
        )
        final_neg_coords.append(
            translate_coordinate(
                coordinate=rotated_neg_coords[index],
                delta_x=x_offset,
                delta_y=y_offset,
            )
        )

    # generate signals
    x_pos = np.zeros(len(y_pos))
    y_pos = np.zeros(len(y_pos))
    x_neg = np.zeros(len(y_pos))
    y_neg = np.zeros(len(y_pos))

    for index in range(0, len(y_pos)):
        x_pos[index], y_pos[index] = final_pos_coords[index]
        x_neg[index], y_neg[index] = final_neg_coords[index]

    return (x_pos, y_pos), (x_neg, y_neg)


def rotate_coordinates(
    coordinate,
    theta: float,  # [rad]
):
    x, y = coordinate
    x_new = x * np.cos(theta) + y * np.sin(theta)
    y_new = y * np.cos(theta) - x * np.sin(theta)

    return x_new, y_new


def translate_coordinate(
    coordinate,
    delta_x: float,
    delta_y: float,
):
    x, y = coordinate
    x_new = x + delta_x
    y_new = y + delta_y

    return x_new, y_new
