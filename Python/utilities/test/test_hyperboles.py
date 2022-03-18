import matplotlib.pyplot as plt
import pytest

from signal_generation.positioning import Position
from utilities import hyperboles


@pytest.mark.plot
def test_generate_hyperbole():
    position_one = Position(
        x=100.0,
        y=550.0,
    )

    position_two = Position(
        x=250.0,
        y=100.0,
    )

    position_three = Position(
        x=-250.0,
        y=-250.0,
    )

    time_difference_12 = -500e-9
    time_difference_13 = -400e-9

    speed_of_light = 3e8

    distance_difference_12 = abs(time_difference_12 * speed_of_light)
    distance_difference_13 = abs(time_difference_13 * speed_of_light)

    pos_12, neg_12 = hyperboles.generate_hyperbole_points(
        distance_difference=distance_difference_12,
        receiver_positions=(
            position_one,
            position_two,
        ),
    )
    pos_13, neg_13 = hyperboles.generate_hyperbole_points(
        distance_difference=distance_difference_13,
        receiver_positions=(
            position_one,
            position_three,
        ),
    )

    plt.plot(
        position_one.x, position_one.y, color="red", marker="o", label="Position 1"
    )
    plt.plot(
        position_two.x, position_two.y, color="orange", marker="o", label="Position 2"
    )
    plt.plot(
        position_three.x,
        position_three.y,
        color="magenta",
        marker="o",
        label="Position 3",
    )

    plt.plot(pos_12[0], pos_12[1], color="blue", label="pos_12")
    plt.plot(neg_12[0], neg_12[1], color="blue", label="neg_12")
    plt.plot(pos_13[0], pos_13[1], color="green", label="pos_13")
    plt.plot(neg_13[0], neg_13[1], color="green", label="neg_13")
    plt.grid()
    plt.legend()
    plt.show()
