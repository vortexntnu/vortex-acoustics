import logging
import os
from pathlib import Path

import numpy as np
from jinja2 import Environment, FileSystemLoader


FILE_PATH = os.path.abspath(os.path.dirname(__file__)) 
TEMPLATES_DIR = Path(FILE_PATH) / "templates"

TYPE_TABLE = {
    np.float16: "float16_t",
    np.float32: "float32_t",
    np.int16: "int16_t",
    np.int32: "int32_t",
}

CRITICAL_FLOAT_TYPES = [
    np.double,
    np.longdouble,
    np.float64,
    np.float128,
]

CRITICAL_INT_TYPES = [
    np.int,
    np.int_,
    np.int64,
    np.longlong,
]

JINJA_ENV = Environment(
    loader=FileSystemLoader(TEMPLATES_DIR),
)

TEMPLATE = JINJA_ENV.get_template("template_header.h")


def convert_to_compatible_type(data):
    if data.dtype in CRITICAL_FLOAT_TYPES:
        logging.debug("BEWARE: down conversion on float type")
        return data.astype(np.float32)

    if data.dtype in CRITICAL_INT_TYPES:
        logging.debug("BEWARE: down conversion on int type")
        return data.astype(np.int32)

    return data


def convert_shape_to_c_dimension(data):
    c_dimension_indicator = ""
    for length in np.shape(data):
        c_dimension_indicator += f"[{length}]"

    return c_dimension_indicator


def generate_header(
    input_data: np.ndarray,
    result_data: np.ndarray,
    namespace: str = None,
    output_path: Path = Path() / "temp.h",
):
    """Generates header file with the provided input and result data.

    Can be used to generate test data for the Teensy from Numpy arrays.

    Args:
        input_data: A Numpy array containing the necessary input values.
        result_data: A Numpy array containing the expected output values.
    """
    if (type(input_data) is not np.ndarray) or (type(result_data) is not np.ndarray):
        raise TypeError("Arguments must be of type np.ndarray")

    input_data = convert_to_compatible_type(input_data)
    result_data = convert_to_compatible_type(result_data)

    with open(output_path, "w") as output_file:
        output_file.write(TEMPLATE.render(
            namespace=namespace,
            input_type="{data_type}_t".format(data_type=str(input_data.dtype)),
            input_dimensions=convert_shape_to_c_dimension(input_data),
            input_data=input_data.flatten(),
            result_type="{data_type}_t".format(data_type=str(result_data.dtype)),
            result_dimensions=convert_shape_to_c_dimension(result_data),
            result_data=result_data.flatten(),
        ))

