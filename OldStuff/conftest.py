import pytest


def pytest_addoption(parser):
    parser.addoption(
        "--runplot",
        action="store_true",
        default=False,
        help="Run tests with plots",
    )


def pytest_configure(config):
    config.addinivalue_line("markers", "plot: mark test containing plotting")


def pytest_collection_modifyitems(config, items):
    if config.getoption("--runplot"):
        # --runplot given in cli: do not skip test with plots
        return
    skip_plots = pytest.mark.skip(reason="need --runplot option to run")
    for item in items:
        if "plot" in item.keywords:
            item.add_marker(skip_plots)
