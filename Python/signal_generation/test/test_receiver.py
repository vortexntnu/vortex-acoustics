import numpy as np

import signal_generation.receiver as rec
import signal_generation.positioning as pos
import signal_generation.source as src


class TestHydrophoneArray:
    @staticmethod
    def test_when_initialized_then_success():
        positions = [pos.Position(), pos.Position()]
        hydro_array = rec.HydrophoneArray(
            positions=positions,
        )

    @staticmethod
    def test_given_pinger_and_one_receiver_when_generate_signals_then_one_signal():
        positions = [
            pos.Position(),
        ]

        hydro_array = rec.HydrophoneArray(
            positions=positions,
        )

        pinger = src.Pinger(
            frequency=10.0,  # [kHz]
        )
        pinger_position = pos.Position(
            x=10.0,
            y=-20.2,
            z=14.2,
        )

        output_length = 100.0

        result = hydro_array.generate_signals(
            output_length=output_length,
            source=pinger,
            source_position=pinger_position,
            output_data_type=np.uint32,
            sound_speed=1500.0,
        )

        assert np.shape(result) == (1, int(output_length * pinger.sampling_frequency))

    @staticmethod
    def test_given_pinger_and_multiple_receiver_when_generate_signals_then_multiple_signal():
        hydro_positions = [
            pos.Position(x=0.5, y=0.8, z=3.0),
            pos.Position(x=-0.5, y=0.8, z=3.5),
            pos.Position(x=0.7, y=-0.4, z=4.2),
            pos.Position(x=-0.6, y=-0.3, z=2.5),
        ]

        hydro_array = rec.HydrophoneArray(
            positions=hydro_positions,
        )

        pinger = src.Pinger(
            frequency=10.0,
            period=100.0,
            pulse_length=20.0,
        )
        pinger_position = pos.Position(
            x=20.0,
            y=-50.2,
            z=19.3,
        )

        output_length = 100.0

        result = hydro_array.generate_signals(
            output_length=output_length,
            source=pinger,
            source_position=pinger_position,
            output_data_type=np.uint32,
            sound_speed=1500.0,
        )

        assert np.shape(result) == (4, int(output_length * pinger.sampling_frequency))
