from typing import List

import numpy as np

import signal_generation.conversion as sg_conv
import signal_generation.positioning as sg_pos
import signal_generation.source as sg_source


class HydrophoneArray:
    def __init__(
        self,
        positions: List[sg_pos.Position],
    ):
        self.positions = np.array(positions)

    def _calculate_travel_time(
        self,
        source_position: sg_pos.Position,
        sound_speed: float = 1500,  # [m/s]
    ) -> List[float]:
        """Calculate the travel time from the source to the hydrophones.

        Args:
            source_position: Provide a position for the source.
            sound_speed: The expected sound speed in the medium in meter per seconds.

        Returns:
            A numpy.array containing the travel times in milliseconds (ms).
        """
        distances = np.array([abs(pos - source_position) for pos in self.positions])
        return distances / sound_speed * 10 ** 3

    def generate_signals(
        self,
        output_length: float,  # [ms]
        source: sg_source.Pinger,
        source_position: sg_pos.Position,
        amplitude: float = 1.0,
        output_data_type: type = np.uint32,
        sound_speed: float = 1500.0,
    ) -> List[np.array]:
        """Generate signals received by the hydrophones placed in the array.

        Args:
            output_length: The length of the expected output signal in milliseconds (ms).
            source: Provide an initialized Pinger signal source.
            source_position: Provide a position for the source.
            amplitude: The scaling of the amplitudes of the pinger signal.
            output_data_type: The datatype used for converting from float to integer. Must be
                a numpy integer type.
            sound_speed: The expected sound speed in the medium in meter per seconds.

        Returns:
            A list of numpy.arrays containing the received signal at the hydrophones
            placed in the array.
        """
        travel_times = self._calculate_travel_time(
            source_position=source_position,
            sound_speed=sound_speed,
        )

        nr_of_samples = int(output_length * source.sampling_frequency)

        signals = np.empty((len(self.positions), nr_of_samples), dtype=output_data_type)

        for index, time in enumerate(travel_times):
            converted_signal = sg_conv.convert_to_integer_type(
                resulting_type=output_data_type,
                input_signal=source.generate_signal(
                    amplitude=amplitude,
                    offset=time,
                    length=output_length,
                ),
            )
            signals[index] = converted_signal

        return signals
