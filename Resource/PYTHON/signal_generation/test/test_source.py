import pytest

import signal_generation.source as source


class TestPinger:
    @staticmethod
    def test_when_initialized_then_sample_per_period_correct():
        s = source.Pinger(
            frequency=0.1,
            sampling_frequency=1,
            pulse_length=10,
            period=10,
        )
        assert s.samples_per_period == 10

    @staticmethod
    def test_when_initialized_then_sample_per_pulse_correct():
        s = source.Pinger(
            frequency=0.1,
            sampling_frequency=1,
            pulse_length=10,
            period=10,
        )
        assert s.samples_per_pulse == 10

    @staticmethod
    def test_given_frequency_greater_than_half_sampling_frequency_when_initialized_then_error():
        with pytest.raises(ValueError):
            s = source.Pinger(
                frequency=1.0001,
                sampling_frequency=2,
                pulse_length=10,
                period=100,
            )

    @staticmethod
    def test_given_frequency_is_half_of_sampling_frequency_when_initialized_then_succes():
        s = source.Pinger(
            frequency=1,
            sampling_frequency=2,
            pulse_length=10,
            period=100,
        )

    @staticmethod
    def test_when_generate_signal_then_success():
        s = source.Pinger(
            frequency=10,
            pulse_length=100,
            period=1000,
        )
        out = s.generate_signal(
            length=2000,
        )

    @staticmethod
    def test_given_amplitude_when_generate_signal_then_max_smaller_or_equal_to_amplitude():
        s = source.Pinger(
            frequency=10,
            pulse_length=100,
            period=1000,
        )

        ref_amplitude = 0.3
        out = s.generate_signal(
            amplitude=ref_amplitude,
            length=2000,
        )

        res_max = max(out)
        res_min = min(out)

        assert res_max < ref_amplitude + 1 * 10 ** (-9)
        assert abs(res_min) < ref_amplitude + 1 * 10 ** (-9)

    @staticmethod
    def test_given_length_double_the_period_when_generate_signal_then_two_pulses():
        """
        Take low frequency to only have positive values in pulse and find
        number of sections that are zero.
        """
        s = source.Pinger(
            frequency=0.01,
            pulse_length=10,
            period=1000,
        )
        out = s.generate_signal(
            length=2000,
        )

        number_of_zero_sections = 0
        last_sample_zero = False
        for i, current_sample in enumerate(out):
            if i == 0:
                continue

            if last_sample_zero == False and current_sample == 0:
                last_sample_zero = True
                number_of_zero_sections += 1
            elif last_sample_zero == True and current_sample != 0:
                last_sample_zero = False

        assert number_of_zero_sections == 2
