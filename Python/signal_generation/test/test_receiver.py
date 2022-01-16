import numpy as np

import signal_generation.conversion as sg_conv
import signal_generation.receiver as sg_rec
import signal_generation.positioning as sg_pos
import signal_generation.source as sg_src


class TestHydrophoneArray:
    @staticmethod
    def test_when_initialized_then_success():
        positions = [sg_pos.Position(), sg_pos.Position()]
        hydro_array = sg_rec.HydrophoneArray(
            positions=positions,
        )


class TestGenerateSignals:
    @staticmethod
    def test_given_one_pinger_and_one_receiver_when_generate_signals_then_one_signal():
        positions = np.array(
            [
                sg_pos.Position(),
            ]
        )

        receivers = sg_rec.HydrophoneArray(
            positions=positions,
        )

        pinger_position = sg_pos.Position(
            x=10.0,
            y=-20.2,
            z=14.2,
        )

        sources = np.array(
            [
                sg_src.Pinger(
                    frequency=10.0,
                    position=pinger_position,
                )
            ]
        )

        output_length = 100

        result = sg_rec.generate_signals(
            sources=sources,
            receivers=receivers,
            output_length=output_length,
            sound_speed=1500.0,
        )

        assert np.shape(result) == (len(receivers), output_length)

    @staticmethod
    def test_given_one_pinger_and_multiple_receiver_when_generate_signals_then_multiple_signal():
        hydro_positions = np.array(
            [
                sg_pos.Position(x=0.5, y=0.8, z=3.0),
                sg_pos.Position(x=-0.5, y=0.8, z=3.5),
                sg_pos.Position(x=0.7, y=-0.4, z=4.2),
                sg_pos.Position(x=-0.6, y=-0.3, z=2.5),
            ]
        )

        receivers = sg_rec.HydrophoneArray(
            positions=hydro_positions,
        )

        pinger_position = sg_pos.Position(
            x=20.0,
            y=-50.2,
            z=19.3,
        )
        sources = np.array(
            [
                sg_src.Pinger(
                    frequency=10.0,
                    period=100.0,
                    pulse_length=20.0,
                    position=pinger_position,
                )
            ]
        )

        output_length = 100

        result = sg_rec.generate_signals(
            sources=sources,
            receivers=receivers,
            output_length=output_length,
            sound_speed=1500.0,
        )

        assert np.shape(result) == (len(receivers), output_length)

    @staticmethod
    def test_given_geometric_spreading_then_max_amplitude_far_below_input_amplitude():
        amplitude = 1.0

        positions = np.array(
            [
                sg_pos.Position(),
            ]
        )

        receivers = sg_rec.HydrophoneArray(
            positions=positions,
        )

        pinger_position = sg_pos.Position(
            x=10.0,
            y=11.2,
            z=10.2,
        )
        sources = np.array(
            [
                sg_src.Pinger(
                    amplitude=amplitude,
                    frequency=10.0,  # [kHz]
                    position=pinger_position,
                )
            ]
        )

        output_length = 100

        result = sg_rec.generate_signals(
            sources=sources,
            receivers=receivers,
            output_length=output_length,
            sound_speed=1500.0,
            geometric_spreading=True,
        )

        assert np.max(abs(result)) <= (amplitude / abs(pinger_position))
