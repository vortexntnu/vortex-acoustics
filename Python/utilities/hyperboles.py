import logging
from typing import Tuple

import numpy as np

from signal_generation.positioning import Position


def generate_hyperbole_points(
    distance_difference: float,
    receiver_positions: Tuple[Position, Position],
    max_range: float = 20.0,
):
    """Generates the hyperboles for two given receiver positions and distance difference of the arrivals.

    Args:
        distance_difference: The difference in distance, usually calculated from the time difference of the arrivals
            and the sound speed.
        receiver_positions: A tuple of two receiver positions. They must lie on the either the same x- or y-coordinates.
        max_range: The range for which the points of the hyperboles are generated. E.g., if it's a hyperbole with
            vertically aligned foci, the range would be [-20,20] for the x-coordinates.

    Returns:
        Two arrays where one is the used x-coordinate values and the other are the corresponding y-coordinate values.
    """
    position_one = receiver_positions[0]
    position_two = receiver_positions[1]

    x_offset = (position_one.x + position_two.x) / 2
    y_offset = (position_one.y + position_two.y) / 2

    x_diff = position_one.x - position_two.x
    y_diff = position_one.y - position_two.y

    logging.debug(f"x_offset: {x_offset}")
    logging.debug(f"y_offset: {y_offset}")

    logging.debug(f"x_diff: {x_diff}")
    logging.debug(f"y_diff: {y_diff}")

    logging.debug(f"position one: {position_one}")
    logging.debug(f"position two: {position_two}")

    if y_diff == 0 and x_diff != 0:
        # Foci are aligned horizontally
        y = np.linspace(-max_range, max_range, 10000)

        a = distance_difference / 2
        c = max([position_one.x, position_two.x]) - x_offset

        x_pos = a * np.sqrt(y ** 2 / (c ** 2 - a ** 2) + 1)
        x_neg = -1.0 * x_pos

        return (x_neg + x_offset, x_pos + x_offset), (y + y_offset, y + y_offset)
    elif x_diff == 0 and y_diff != 0:
        # Foci are aligned vertically
        x = np.linspace(-max_range, max_range, 10000)

        a = distance_difference / 2
        c = max([position_one.y, position_two.y]) - y_offset

        y_pos = a * np.sqrt(x ** 2 / (c ** 2 - a ** 2) + 1)
        y_neg = -1.0 * y_pos

        return (x + x_offset, x + x_offset), (y_neg + y_offset, y_pos + y_offset)
    else:
        raise ValueError("None horizontal/vertical hyperbole are not supported")
