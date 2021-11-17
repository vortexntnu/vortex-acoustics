import numpy as np

import pytest

import signal_generation.positioning as sg_pos


class TestPosition:
    @staticmethod
    def test_when_initialized_then_success():
        position = sg_pos.Position()

    @staticmethod
    def test_given_close_to_equal_position_when_test_equality_then_equal():
        position_one = sg_pos.Position()
        position_two = sg_pos.Position(
            x=1 * 10 ** (-1) * sg_pos.Position.tol,
            y=(-1) * 10 ** (-10) * sg_pos.Position.tol,
            z=5 * 10 ** (-10) * sg_pos.Position.tol,
        )

        assert position_one == position_two

    @staticmethod
    def test_given_unequal_positions_when_test_equality_then_not_equal():
        position_one = sg_pos.Position()
        position_two = sg_pos.Position(
            x=-10,
            y=-20,
            z=-30,
        )

        assert position_two != position_one

    @staticmethod
    def test_given_two_positions_when_calculated_distance_then_correct():
        ref_x = 1
        ref_y = 4
        ref_z = 8

        ref_distance = 9

        x_0 = 10
        y_0 = 14
        z_0 = -22

        position_one = sg_pos.Position(x=x_0, y=y_0, z=z_0)
        position_two = sg_pos.Position(x=x_0 + ref_x, y=y_0 + ref_y, z=z_0 + ref_z)

        res_distance = abs(position_one - position_two)

        assert (res_distance - ref_distance) < sg_pos.Position.tol


def test_maximum_distance():
    positions = np.array(
        [
            sg_pos.Position(),
            sg_pos.Position(x=10.0),
            sg_pos.Position(x=2.0),
            sg_pos.Position(x=30.0),
        ]
    )
    ref_distance = 30.0

    res_distance = sg_pos.find_maximum_distance(positions=positions)

    assert res_distance == ref_distance


class TestCalculateDistances:
    @staticmethod
    def test_given_single_source_and_receiver_then_shape_as_expected():
        source_positions = np.array([sg_pos.Position()])
        receiver_positions = np.array([sg_pos.Position()])

        ref_shape = (len(source_positions), len(receiver_positions))

        distances = sg_pos.calculate_distances(
            source_positions=source_positions,
            receiver_positions=receiver_positions,
        )

        res_shape = np.shape(distances)

        assert res_shape == ref_shape

    @staticmethod
    def test_given_multiple_sources_and_receivers_then_shape_as_expected():
        source_positions = np.array(
            [sg_pos.Position(), sg_pos.Position()],
        )
        receiver_positions = np.array(
            [sg_pos.Position(), sg_pos.Position(), sg_pos.Position()],
        )

        ref_shape = (len(source_positions), len(receiver_positions))

        distances = sg_pos.calculate_distances(
            source_positions=source_positions,
            receiver_positions=receiver_positions,
        )

        res_shape = np.shape(distances)

        assert res_shape == ref_shape

    @staticmethod
    def test_given_source_and_receiver_position_then_distance_within_tolerance():
        source_positions = np.array([sg_pos.Position(x=0, y=0, z=1)])
        receiver_positions = np.array(
            [
                sg_pos.Position(x=1, y=0, z=1),
                sg_pos.Position(x=0, y=1, z=0),
            ]
        )

        ref_distances = np.array(
            [
                [
                    np.sqrt(1),
                    np.sqrt(1 + 1),
                ]
            ]
        )

        res_distances = sg_pos.calculate_distances(
            source_positions=source_positions,
            receiver_positions=receiver_positions,
        )

        assert (abs(res_distances - ref_distances) < 1 * 10 ** (-6)).all()
