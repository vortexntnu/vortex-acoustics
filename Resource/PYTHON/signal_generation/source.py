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

        self.frequency = frequency
        self.sampling_frequency = sampling_frequency
        self.pulse_length = pulse_length
        self.period = period

        self._sample_period = np.zeros(self.samples_per_period)
        self._sample_period[0 : self.samples_per_pulse] = np.sin(
            2
            * np.pi
            * self.frequency
            / self.sampling_frequency
            * np.arange(self.samples_per_pulse)
        )

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

    def get_offset_period(
        self,
        offset_in_samples: int,  # number of samples
    ) -> np.array:
        """Generates an offset pinger period where the pulse is wrapped around if necessary.

        Providing an offset while generating the pinger output can be used to mimic
        different times of arrival.

        Args:
            offset_in_samples: Number of samples the pulse should be offset with.

        Returns:
            A numpy.array containing one period of the pinger signal, but offset by the
            provided number of samples.
        """
        offset = offset_in_samples % self.samples_per_period
        offset_period = np.zeros(self.samples_per_period)

        if (offset + self.samples_per_pulse) <= self.samples_per_period:
            start_index = offset
            end_index = start_index + self.samples_per_pulse
            offset_period[start_index:end_index] = self._sample_period[
                0 : self.samples_per_pulse
            ]
        else:
            # offset and pulse length are larger than period length => wrap around is needed
            length_end_segment = self.samples_per_period - offset
            length_start_segment = self.samples_per_pulse - length_end_segment
            offset_period[offset:] = self._sample_period[0:length_end_segment]
            offset_period[0:length_start_segment] = self._sample_period[
                length_end_segment : self.samples_per_pulse
            ]
        return offset_period

    def generate_signal(
        self,
        amplitude: float = 1.0,
        offset: float = 0.0,  # [ms]
        length: float = 5000,  # [ms]
    ) -> np.array:
        """Generates an array of the output signal for the given length.

        Args:
            amplitude: The amplitude used for scaling the pulse amplitude.
            length: The length of the output signal in ms.

        Returns:
            An numpy.array with type numpy.float64. The output is in the range
            of [-amplitude,amplitude].
        """
        samples_per_output = int(length * self.sampling_frequency)  # [ms*kHz]
        output = np.zeros(samples_per_output)

        offset_in_samples = int(offset * self.sampling_frequency)  # [ms*kHz]
        offset_period = self.get_offset_period(
            offset_in_samples=offset_in_samples,
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

            output[output_start:output_end] = offset_period[offset_start:offset_end]

        return amplitude * output
