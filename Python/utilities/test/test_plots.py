import logging

import matplotlib.pyplot as plt
import numpy as np
import pytest

import signal_generation as sg
from multilateration import multilateration as ml
from multilateration import parameters as params
from signal_generation.positioning import Position
from utilities import hyperboles
from utilities import tdoa
from utilities import plots


@pytest.mark.plot
def test_plot_estimates():
    source_position = Position()

    hydrophone_positions = np.array(
        [
            Position(
                x=-0.11,
                y=0.31,
                z=0.1,
            ),
            Position(
                x=0.11,
                y=0.31,
                z=0.1,
            ),
            Position(
                x=0.0,
                y=-0.24,
                z=0.0,
            ),
            Position(
                x=0.5,
                y=-0.1,
                z=0.4,
            ),
            Position(
                x=0.4,
                y=0.0,
                z=-0.4,
            ),
        ]
    )

    sample_frequencies = [100000 * i for i in range(1, 10)]

    result_positions = []
    result_labels = []
    for frequency in sample_frequencies:
        tdoa_lag_array = tdoa.generate_lag_array(
            hydrophone_positions=hydrophone_positions,
            source_position=source_position,
            sample_frequency=frequency,
        )[:, 0][1:]

        res_x, res_y, res_z = ml.calculate_pinger_position(
            tdoa_lag_array=tdoa_lag_array,
            hydrophone_positions=hydrophone_positions,
            sample_frequency=frequency,
        )

        result_positions.append(
            Position(
                x=res_x,
                y=res_y,
                z=res_z,
            )
        )

        result_labels.append(
            f"fs={frequency/1000} kHz, pos=({res_x:.1f},{res_y:.1f},{res_z:.1f})"
        )

    plots.plot_positions(
        hydrophone_positions=hydrophone_positions,
        source_position=source_position,
        result_positions=result_positions,
        result_labels=result_labels,
    )


@pytest.mark.plot
def test_plot_hyperboles():
    radius = 5.0
    number_of_sources = 2

    hydrophone_positions = np.array(
        [
            Position(
                x=1.0,
                y=0,
                z=0,
            ),
            Position(
                x=-1.0,
                y=0,
                z=0,
            ),
            Position(
                x=1.0,
                y=2.2,
                z=0,
            ),
        ]
    )

    xs = np.linspace(-radius + 0.1, radius, number_of_sources)
    ys = np.sqrt(radius ** 2 - xs ** 2)

    source_positions = [(xs[i], ys[i]) for i, x in enumerate(xs)]

    plots.plot_hyperboles(
        source_positions=source_positions,
        hydrophone_positions=hydrophone_positions,
    )


@pytest.mark.plot
def test_plot_grid():
    source_position = Position(
        x=20,
        y=20,
        z=10,
    )
    sample_frequency = 300000

    hydrophone_positions = np.array(
        [
            Position(
                x=0.2,
                y=-0.3,
                z=-0.2,
            ),
            Position(
                x=-0.2,
                y=-0.3,
                z=0.2,
            ),
            Position(
                x=0.2,
                y=0.31,
                z=0.2,
            ),
            Position(
                x=-0.2,
                y=0.3,
                z=0.21,
            ),
            Position(
                x=-0.2,
                y=0.29,
                z=-0.2,
            ),
        ]
    )

    plots.plot_estimates_from_grid(
        source_position=source_position,
        hydrophone_positions=hydrophone_positions,
        sample_frequency=sample_frequency,
    )


@pytest.mark.plot
def test_plot_path():
    source_position = Position(
        x=10,
        y=10,
        z=10,
    )
    sample_frequency = 300000

    xs = np.linspace(0, 27, 10)
    ys = np.linspace(0, 23, 10)
    zs = np.linspace(0, 30, 10)

    hydrophone_positions = np.array(
        [
            Position(
                x=0.2,
                y=-0.3,
                z=-0.2,
            ),
            Position(
                x=-0.2,
                y=-0.3,
                z=0.2,
            ),
            Position(
                x=0.2,
                y=0.31,
                z=0.2,
            ),
            Position(
                x=-0.2,
                y=0.3,
                z=0.21,
            ),
            Position(
                x=-0.2,
                y=0.29,
                z=-0.2,
            ),
        ]
    )

    path_positions = np.array(
        [
            Position(
                x=0.0,
                y=0.0,
                z=0.0,
            ),
            Position(
                x=7.0,
                y=3.0,
                z=3.0,
            ),
            Position(
                x=14.0,
                y=8.0,
                z=8.0,
            ),
            Position(
                x=21.0,
                y=17.0,
                z=17.0,
            ),
            Position(
                x=28.0,
                y=28.0,
                z=28.0,
            ),
        ]
    )

    plots.plot_estimates_along_path(
        source_position=source_position,
        hydrophone_positions=hydrophone_positions,
        path_positions=path_positions,
        sample_frequency=sample_frequency,
    )
