import pytest
import scipy.signal.windows

import signal_generation.source as source


class TestWindowGeneration:
    @staticmethod
    def test_given_odd_pulse_length_100_percent_window_then_equal_to_hann():
        reference = scipy.signal.windows.hann(
            M=101,
            sym=True,
        )

        result = source.generate_pulse_window(
            pulse_length=101,
            start_end_percentage=100,
        )

        assert (result == reference).all()

    @staticmethod
    def test_given_10_percent_window_then_roughly_90_percent_ones():
        percentage = 10
        reference_percentage = 100.0 - percentage

        window = source.generate_pulse_window(
            pulse_length=10000,
            start_end_percentage=percentage,
        )

        result_percentage = sum(window == 1.0) / len(window) * 100

        assert abs(result_percentage - reference_percentage) < 2


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
    def test_given_frequency_is_half_of_sampling_frequency_when_initialized_then_success():
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

    @staticmethod
    def test_given_offset_and_pulse_length_larger_than_period_when_generate_signal_then_success():
        s = source.Pinger(
            frequency=10,
            pulse_length=10,
            period=30,
        )
        out = s.generate_signal(
            offset=25,
            length=30,
        )

    @staticmethod
    def test_given_offset_and_pulse_length_larger_than_period_when_generate_one_period_then_last_samples_not_zero():
        s = source.Pinger(
            frequency=10,
            pulse_length=10,
            period=20,
        )
        out = s.generate_signal(
            offset=15,
            length=20,
        )

        assert any(out[-10:] != 0)

    @staticmethod
    def test_given_small_offset_when_generate_signal_then_first_samples_zero():
        s = source.Pinger(
            frequency=10,
            pulse_length=10,
            period=20,
        )
        out = s.generate_signal(
            offset=5,
            length=20,
        )

        assert all(out[0:10] == 0)
