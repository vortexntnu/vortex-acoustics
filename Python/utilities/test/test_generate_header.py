import os
from pathlib import Path

import numpy as np
import pytest
import utilities.generate_headers as utils_gh


@pytest.fixture()
def temp_output_file():
    temp_file_path = Path() / "temp_header.h"
    temp_file_path.touch()
    yield temp_file_path
    os.remove(temp_file_path)


@pytest.mark.parametrize(
    "data_type, result_type",
    [
        [np.single, np.float32],
        [np.double, np.float32],
        [np.float16, np.float16],
        [np.float32, np.float32],
        [np.float64, np.float32],
        [np.int16, np.int16],
        [np.int32, np.int32],
        [np.int64, np.int32],
    ],
)
def test_given_data_type_then_compatible_type(data_type, result_type):
    data = np.array([42, 42], dtype=data_type)

    data = utils_gh.convert_to_compatible_type(data)

    assert data.dtype == result_type


@pytest.mark.parametrize(
    "input_data, expected_c_dimensions",
    [
        [np.zeros((3,)), "[3]"],
        [np.zeros((3, 4)), "[3][4]"],
        [np.zeros((3, 6, 3)), "[3][6][3]"],
    ],
)
def test_given_input_data_then_c_dimensions_generated(
    input_data, expected_c_dimensions
):
    generated_c_dimensions = utils_gh.convert_shape_to_c_dimension(input_data)

    assert generated_c_dimensions == expected_c_dimensions


def test_given_input_and_output_data_then_header_generated(temp_output_file):
    input_data = np.array([10, 10], dtype=np.float16)
    output_data = np.array([[42, 42], [21, 21]], dtype=np.float64)

    namespace = "uwotmate"

    utils_gh.generate_header(
        namespace=namespace,
        input_data=input_data,
        result_data=output_data,
        output_path=temp_output_file,
    )

    with open(temp_output_file, "r") as generated_file:
        data = generated_file.read()

    assert f"namespace {namespace}" in data  # namespace naming used properly
    assert "float16_t input_data" in data  # input data variable correct
    assert "float32_t reference_data" in data  # output data variable correct
