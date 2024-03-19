"""Provides functions to mimic analog to digital conversion.

Even though generated signal is sampled in time domain. There's still a need to
convert the amplitude range and type to the expected values on the embedded
device.

    Typical usages example:

    converted_signal = convert_to_integer_type(
        resulting_type=np.uint16,

"""
import re

import numpy as np

_SIGNED_INTEGER_TYPES = {
    "int8": np.int8,
    "int16": np.int16,
    "int32": np.int32,
    "int64": np.int64,
}
_UNSIGNED_INTEGER_TYPES = {
    "uint8": np.uint8,
    "uint16": np.uint16,
    "uint32": np.uint32,
    "uint64": np.uint64,
}
_FLOAT_TYPES = {
    "float16": np.float16,
    "float32": np.float32,
    "float64": np.float64,
}

_type_pattern = re.compile(r"<class 'numpy\.(?P<type>u?int)(?P<bits>\d{1,2})'>")


def convert_to_integer_type(
    resulting_type: type,
    input_signal: np.array,
    pre_offset: float = 0.0,
) -> np.array:
    """Converts a float signal to an integer type.

    Args:
        resulting_type: The type the result should become. If the type is
            unsigned shift signal to only have positive values.
        input_signal: The input signal as numpy.array of type numpy.float.
        pre_offset: Add offset before signal clipping.

    Returns:
        A numpy.array of integer type. Where the input signal is clipped at [-1.0,1.0]
        and if resulting type is unsigned, also added an offset to result in only positive
        values.

    Raises:
        TypeError: When resulting_type not in SIGNED_INTEGER_TYPES or UNSIGNED_INTEGER_TYPES.
            Or when the type of the input numpy.array is not in FLOAT_TYPES.
    """
    if resulting_type not in (
        list(_SIGNED_INTEGER_TYPES.values()) + list(_UNSIGNED_INTEGER_TYPES.values())
    ):
        raise TypeError(f"Provided resulting type '{resulting_type}' is not valid")

    if input_signal.dtype not in _FLOAT_TYPES.values():
        raise TypeError(
            f"Input Signal of type '{type(input_signal)}' does not have a supported float base type"
        )

    regex_result = _type_pattern.match(str(resulting_type))
    base_type = regex_result.group("type")
    number_of_bits = int(regex_result.group("bits"))

    clipped_signal = np.clip((input_signal + pre_offset), -1.0, 1.0)

    if base_type == "uint":
        clipped_signal += 1.0

    # Signal range is [-1,1] or [0,2] at this point. So to fit an 8-bit unsigned for example,
    # signal has to be divided by 2 and then multiplied by 2 to the power of the number of bits
    # subtracted by one.
    # [0,2]/2 = [0,1] -> [0,1]*(2**(8)-1) = [0,255]
    return (clipped_signal * (2 ** (number_of_bits - 1) - 1)).astype(resulting_type)
