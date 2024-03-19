"""Provides functions to generate noise to introduce to generated signals.

The noise generated here will be of type float and should be introduced before
converting the signals to integers.
"""

import numpy as np


def generate_gaussian_noise(
    length: int,
    variance: float,
    maximum_allowed_value: float = None,
) -> np.array:
    """Generates white (zero-mean) gaussian noise with given length.

    Args:
        length: The length of the output array in samples.
        variance: The variance of the gaussian distribution.
        maximum_allowed: If a maximum allowed value is supplied, then the values
            are clipped at [-max_value, max_value]. Beware that this messes with
            the noise being a gaussian distribution.

    Return:
        An array of white gaussian noise with given length that is optionally clipped
        to output range [-maximum_allowed_value, maximum_allowed_value] of type float.
    """

    noise = np.random.normal(0, np.sqrt(variance), length)
    if maximum_allowed_value is not None:
        noise[np.where(noise > maximum_allowed_value)] = maximum_allowed_value
        noise[np.where(noise < -maximum_allowed_value)] = -maximum_allowed_value

    return noise


def generate_uniform_noise(
    length: int,
    maximum_allowed_value: float = 1.0,
) -> np.array:
    """Generates white uniform noise with given length.

    Args:
        length: The length of the output array in samples.

    Return:
        An array of white uniform noise with given length with output range from
        [-maximum_allowed_value, maximum_allowed_value].
    """

    return np.random.uniform(
        low=-maximum_allowed_value,
        high=maximum_allowed_value,
        size=length,
    )
