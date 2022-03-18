import numpy as np

import signal_generation.conversion as sg_conv
import signal_generation.noise as sg_noise
import signal_generation.positioning as sg_pos
import signal_generation.receiver as sg_rec


class Drone:
    def __init__(
        self,
        hydrophone_positions: np.array,
        initial_position: sg_pos.Position = sg_pos.Position(),
    ):
        self._hydrophone_positions = hydrophone_positions
        self._position = initial_position

    @property
    def position(self) -> sg_pos.Position:
        return self._position

    @position.setter
    def position(
        self,
        position: sg_pos.Position,
    ):
        difference = position - self._position
        self.move(
            x=difference.x,
            y=difference.y,
            z=difference.z,
        )

    @property
    def hydrophone_positions(self) -> np.array:
        return self._hydrophone_positions

    def move(
        self,
        x: float,
        y: float,
        z: float,
    ):
        self._position = sg_pos.Position(
            x=self._position.x + x,
            y=self._position.y + y,
            z=self._position.z + z,
        )

        for index, position in enumerate(self._hydrophone_positions):
            self._hydrophone_positions[index] = sg_pos.Position(
                x=position.x + x,
                y=position.y + y,
                z=position.z + z,
            )

    def generate_received_signals(
        self,
        pinger: sg_pos.Position,
        frame_length: float = 2000,
        noise_variance: float = 0.01,
    ) -> np.array:
        received_signals = 0.7 * sg_rec.generate_signals(
            sources=pinger,
            receivers=self._hydrophone_positions,
            output_length=frame_length,
            sound_speed=1500.0,
        )

        for index, signal in enumerate(received_signals):
            received_signals[index] += sg_noise.generate_gaussian_noise(
                length=frame_length,
                variance=noise_variance,
            )

        received_signals = sg_conv.convert_to_integer_type(
            resulting_type=np.uint16,
            input_signal=received_signals,
        )

        return received_signals
