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

    def generate_signal(
        self,
        amplitude: float = 1.0,
        length: int = 5000,  # [ms]
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

        number_of_periods = samples_per_output // self.samples_per_period
        for i in np.arange(number_of_periods + 1):
            start_pulse = i * self.samples_per_period
            end_pulse = start_pulse + self.samples_per_pulse - 1
            length_pulse = self.samples_per_pulse - 1

            if end_pulse > samples_per_output - 1:
                end_pulse = samples_per_output - 1
                length_pulse = samples_per_output - start_pulse - 1

            output[start_pulse:end_pulse] = np.sin(np.arange(length_pulse))

        return amplitude * output
