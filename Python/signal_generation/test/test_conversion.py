import numpy as np
import pytest

import signal_generation.conversion as conv


class TestConversionToIntegerType:
    @staticmethod
    def test_given_correct_arguments_then_succes():
        conv.convert_to_integer_type(
            resulting_type=np.int32,
            input_signal=np.sin(np.linspace(start=0, stop=2 * np.pi, num=1000)),
            pre_offset=0.0,
        )

    @staticmethod
    def test_given_wrong_resulting_type_then_error_raised():
        with pytest.raises(TypeError):
            conv.convert_to_integer_type(
                resulting_type=np.float32,
                input_signal=np.zeros(10),
                pre_offset=0.0,
            )

    @staticmethod
    def test_given_wrong_input_signal_type_then_error_raised():
        with pytest.raises(TypeError):
            conv.convert_to_integer_type(
                resulting_type=np.float32,
                input_signal=np.zeros(10, dtype=np.int32),
                pre_offset=0.0,
            )
