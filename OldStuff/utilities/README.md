# Utilities Package Usage
This package contains reusable code that's not directly needed for functional code but is
helpful for writing tests and plotting.

## Plots
At this point the package only provides plotting via CLI.
This can be invoked via
```
python -m utilities plot {positions, hyperbole, grid, path}
```
To get more detailed for any of the options use the help function, e.g.,
```
python -m utilities plot path --help


usage: __main__.py plot path [-h] [-d] [-s SAMPLE_FREQUENCY] [-x X] [-y Y]
                             [-z Z]

optional arguments:
  -h, --help            show this help message and exit
  -d, --debug
  -s SAMPLE_FREQUENCY, --sample-frequency SAMPLE_FREQUENCY
                        The sampling frequency to be used for the calculations
  -x X                  x-coordinate for the source
  -y Y                  y-coordinate for the source
  -z Z                  z-coordinate for the source
```
