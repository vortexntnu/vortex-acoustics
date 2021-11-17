"""Provides signal sources for testing multilateration algorithm.

        Typical usage example:

        pinger = Pinger(
            frequency=10.0,
            sampling_frequency=100.0,
            pulse_length=100,
            period=1000,
        )
        output_signal = pinger.generate_signal(
            amplitude=0.6,
            length=5000,
        )
"""
import logging
from dataclasses import dataclass

import numpy as np
import scipy.signal.windows

from signal_generation.positioning import Position


def generate_pulse_window(
    pulse_length: int,
    start_end_percentage: int = 10,
    start_offset: float = None,
) -> np.array:
    """
    Args:
        pulse_length: The length of the pulse to generate the window for.
        start_end_percentage: The amount of the pulse that is affected by
            the window function in %. E.g. 10% will apply the rising flank of
            the window on the 5% at the start and falling flank at the 5% at
            the end, combining to 10%. The maximum value is 100%.
        start_offset: The offset in samples to shift the window by.

    Returns:
        A window with length of the pulse, that can be used to smoothen the
        edges of the pulse.
    """
    if start_end_percentage > 100:
        raise ValueError(
            f"start_end_percentage of '{start_end_percentage:.2f}' exceeds limit of 100"
        )

    window_length = pulse_length * start_end_percentage // 100
    # make window an odd amount of samples for peak in middle
    is_even = not (window_length % 2)
    window_length = window_length - 1 if is_even else window_length

    window = scipy.signal.windows.hann(
        M=window_length,
        sym=True,
    )

    pulse_window = np.ones(pulse_length)
    pulse_window[0 : len(window) // 2] = window[0 : len(window) // 2]
    pulse_window[-len(window) // 2 :] = window[-len(window) // 2 :]

    if start_offset:
        n_original = np.arange(len(pulse_window), dtype=np.float64)
        n_offset = n_original + start_offset
        pulse_window = np.interp(x=n_offset, xp=n_original, fp=pulse_window)

    return pulse_window


def generate_pulse(
    frequency: float,  # [kHz]
    pulse_length: int,
    sampling_frequency: float,  # [kHz]
    start_offset: float,  # [Samples]
) -> np.array:
    """Generate a pinger pulse of given length.

    Args:
        frequency: The frequency of the sine used during the pulse.
        sampling_frequency: The sampling frequency used within the system.
        pulse_length: The length of the pulse in samples.
        start_offset: Offset to apply to sine at start. Must be smaller than one,
            because more is achieved with shifting by samples.

    Returns:
        A numpy array with (length,) shape containing the ON part off the pinger
        period, also called pulse.
    """
    n = np.arange(pulse_length) + start_offset
    t = n / sampling_frequency

    return np.sin(2 * np.pi * frequency * t)


class Pinger:
    """Class to model idealized acoustic pinger.

    This class provides a simple way of generating pulsed signals, comparable
    to the output of an acoustic pinger.
    """

    def __init__(
        self,
        frequency: float,  # [kHz]
        sampling_frequency: float = 100.0,  # [kHz]
        pulse_length: int = 500,  # [ms]
        period: int = 1000,  # [ms]
        amplitude: float = 1.0,
        use_window: bool = False,
        position: Position = Position(),
    ):
        """Initializes Pinger class with given parameters.

        Args:
            frequency: The frequency of the sine wave generated during the pulse.
                Frequency must fulfill Shannon-Nyquist criterium: f <= fs/2.
            sampling_frequency: The sampling frequency that is being used for
                the signal in digital domain.
            pulse_length: Length of the ON-section or pulse during a period
                of the pinger output in milliseconds/ms.
            period: Length of the period of the pinger output in milliseconds/ms.
            use_window: If True applies a window function to smoothen out the
                5% at the start and end of the pulse.

        Returns:
            An object of type Pinger initialized with the given parameters. The
            Pinger object generates signals of type float.

        Raises:
            AttributeError: When the frequency does not fullfil Shannon-Nyquist
                criterium.
        """
        if frequency > (sampling_frequency / 2):
            raise ValueError(
                f"Frequency is not in Nyquist Range: {frequency} > {sampling_frequency}/2"
            )

        self.amplitude = amplitude
        self.frequency = frequency
        self.sampling_frequency = sampling_frequency
        self.pulse_length = pulse_length
        self.period = period
        self.use_window = use_window
        self.position = position

    @property
    def samples_per_period(self) -> int:
        """
        Returns:
            The amount of samples a period of the output signal uses with the
            given period in ms and sampling rate in kHz.
        """
        return int(self.period * self.sampling_frequency)  # [ms*kHz]

    @property
    def samples_per_pulse(self) -> int:
        """
        Returns:
            The amount of samples a pulse of the output signal uses with the
            given pulse length in ms and sampling rate in kHz.
        """
        return int(self.pulse_length * self.sampling_frequency)  # [ms*kHz]

    def get_pinger_period_with_added_offset(
        self,
        offset: float,  # [ms]
    ) -> np.array:
        """Generates an offset pinger period where the pulse is wrapped around if necessary.

        Providing an offset while generating the pinger output can be used to mimic
        different times of arrival.

        Args:
            offset: The offset in ms with which the pulse is shifted.

        Returns:
            A numpy.array containing one period of the pinger signal, but offset by the
            provided number of samples.
        """
        sample_period = 1 / self.sampling_frequency

        offset_in_samples, offset_modulus = divmod(
            offset, sample_period
        )  # [Samples], [ms]
        offset_modulus_in_samples = (
            offset_modulus * self.sampling_frequency
        )  # [Samples]

        offset = int(offset_in_samples) % self.samples_per_period
        pinger_period_with_offset = np.zeros(self.samples_per_period)

        pulse = generate_pulse(
            frequency=self.frequency,
            pulse_length=self.samples_per_pulse,
            sampling_frequency=self.sampling_frequency,
            start_offset=offset_modulus_in_samples,
        )

        if self.use_window:
            pulse *= generate_pulse_window(
                pulse_length=self.samples_per_pulse,
                start_end_percentage=10,
                start_offset=offset_modulus_in_samples,
            )

        if (offset + self.samples_per_pulse) <= self.samples_per_period:
            start_index = offset
            end_index = start_index + self.samples_per_pulse
            pinger_period_with_offset[start_index:end_index] = pulse
        else:
            # offset and pulse length are larger than period length => wrap around is needed
            length_end_segment = self.samples_per_period - offset
            length_start_segment = self.samples_per_pulse - length_end_segment
            pinger_period_with_offset[offset:] = pulse[0:length_end_segment]
            pinger_period_with_offset[0:length_start_segment] = pulse[
                length_end_segment : self.samples_per_pulse
            ]

        return pinger_period_with_offset

    def generate_signal(
        self,
        offset: float = 0.0,  # [ms]
        length: int = 10000,
    ) -> np.array:
        """Generates an array of the output signal for the given length.

        Args:
            amplitude: The amplitude used for scaling the pulse amplitude.
            length: The length of the output signal in number of samples.

        Returns:
            An numpy.array with type numpy.float64. The output is in the range
            of [-amplitude,amplitude].
        """
        samples_per_output = length
        output = np.zeros(samples_per_output)

        pinger_period_with_offset = self.get_pinger_period_with_added_offset(
            offset=offset,
        )

        number_of_periods = samples_per_output // self.samples_per_period
        for i in np.arange(number_of_periods + 1):
            segment_length = self.samples_per_period

            output_start = i * segment_length
            output_end = output_start + segment_length

            if output_end >= samples_per_output:
                output_end = samples_per_output
                segment_length = output_end - output_start

            offset_start = output_start % self.samples_per_period
            offset_end = offset_start + segment_length

            output[output_start:output_end] = pinger_period_with_offset[
                offset_start:offset_end
            ]

        return self.amplitude * output
