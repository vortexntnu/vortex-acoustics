import signal_generation.positioning as pos


class TestPosition:
    @staticmethod
    def test_when_initialized_then_success():
        position = pos.Position()

    @staticmethod
    def test_given_close_to_equal_position_when_test_equality_then_equal():
        position_one = pos.Position()
        position_two = pos.Position(
            x=1 * 10 ** (-1) * pos.Position.tol,
            y=(-1) * 10 ** (-10) * pos.Position.tol,
            z=5 * 10 ** (-10) * pos.Position.tol,
        )

        assert position_one == position_two

    @staticmethod
    def test_given_unequal_positions_when_test_equality_then_not_equal():
        position_one = pos.Position()
        position_two = pos.Position(
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

        position_one = pos.Position(x=x_0, y=y_0, z=z_0)
        position_two = pos.Position(x=x_0 + ref_x, y=y_0 + ref_y, z=z_0 + ref_z)

        res_distance = abs(position_one - position_two)

        assert (res_distance - ref_distance) < pos.Position.tol
