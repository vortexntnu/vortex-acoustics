import signal_generation.noise as noise


class TestGaussianNoise:
    @staticmethod
    def test_when_generate_gaussian_noise_then_no_error():
        gaussian = noise.generate_gaussian_noise(
            length=100,
            variance=0.5,
            maximum_allowed_value=None,
        )

    @staticmethod
    def test_given_length_then_length_correct():
        reference_length = 100

        gaussian = noise.generate_gaussian_noise(
            length=reference_length,
            variance=0.5,
            maximum_allowed_value=None,
        )

        result_length = len(gaussian)

        assert result_length == reference_length

    @staticmethod
    def test_given_maximum_then_no_values_exceed_maximum():
        maximum = 0.3

        gaussian = noise.generate_gaussian_noise(
            length=1000,
            variance=0.7,
            maximum_allowed_value=maximum,
        )

        assert all(abs(gaussian) <= maximum)


class TestUniformNoise:
    @staticmethod
    def test_when_generate_uniform_noise_then_no_error():
        uniform = noise.generate_uniform_noise(
            length=100,
            maximum_allowed_value=1.0,
        )

    @staticmethod
    def test_given_length_then_length_correct():
        reference_length = 100

        uniform = noise.generate_uniform_noise(
            length=reference_length,
            maximum_allowed_value=1.0,
        )

        result_length = len(uniform)

        assert result_length == reference_length

    @staticmethod
    def test_given_maximum_then_no_values_exceed_maximum():
        maximum = 0.3

        uniform = noise.generate_uniform_noise(
            length=1000,
            maximum_allowed_value=maximum,
        )

        assert all(abs(uniform) <= maximum)
