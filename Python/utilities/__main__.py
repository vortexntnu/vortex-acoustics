import argparse
import logging

from utilities.plot_cli_helpers import plot_estimates
from utilities.plot_cli_helpers import plot_grid
from utilities.plot_cli_helpers import plot_hyperboles
from utilities.plot_cli_helpers import plot_path


# parent parser for shared arguments
parent = argparse.ArgumentParser(add_help=False)
parent.add_argument(
    "-d", "--debug",
    action="store_true",
    dest="debug_mode",
)

main_parser = argparse.ArgumentParser(description="", parents=[parent])
main_subparsers = main_parser.add_subparsers()

# plot options for CLI
plot_parsers = main_subparsers.add_parser("plot", help="Use this option for plotting utilities")
plot_subparsers = plot_parsers.add_subparsers()

position_parser = plot_subparsers.add_parser(
    "positions",
    help="Use this to plot error for positions",
    parents=[parent],
)
position_parser.add_argument(
    "-s", "--sample-frequency",
    type=int,
    default=100000,
    help="The sampling frequency to be used for the calculations",
)
position_parser.add_argument(
    "-x",
    type=float,
    default=0.0,
    help="x-coordinate for the source"
)
position_parser.add_argument(
    "-y",
    type=float,
    default=0.0,
    help="y-coordinate for the source"
)
position_parser.add_argument(
    "-z",
    type=float,
    default=0.0,
    help="z-coordinate for the source"
)
position_parser.set_defaults(func=plot_estimates)

hyperbole_parser = plot_subparsers.add_parser(
    "hyperbole",
    help="Plot 2D hyperboles",
    parents=[parent],
)
hyperbole_parser.add_argument(
    "-r", "--radius",
    type=float,
    dest="radius",
    default=5.0,
    help="The radius on which to place sources on",
)
hyperbole_parser.add_argument(
    "-n", "--number-of-sources",
    type=int,
    dest="n",
    default=6,
    help="The number of sources to place on radius (equally spaced)",
)
hyperbole_parser.set_defaults(func=plot_hyperboles)

grid_parser = plot_subparsers.add_parser(
    "grid",
    help="Use to plot grid and the estimates",
    parents=[parent],
)
grid_parser.add_argument(
    "-s", "--sample-frequency",
    dest="sample_frequency",
    type=float,
    default=100000,
    help="The sampling frequency to be used for the calculations",
)
grid_parser.add_argument(
    "-x",
    type=float,
    default=20.0,
    help="x-coordinate for the source"
)
grid_parser.add_argument(
    "-y",
    type=float,
    default=20.0,
    help="y-coordinate for the source"
)
grid_parser.add_argument(
    "-z",
    type=float,
    default=20.0,
    help="z-coordinate for the source"
)
grid_parser.set_defaults(func=plot_grid)

path_parser = plot_subparsers.add_parser(
    "path",
    help="Use to plot path and estimates",
    parents=[parent],
)
path_parser.add_argument(
    "-s", "--sample-frequency",
    dest="sample_frequency",
    type=float,
    default=100000,
    help="The sampling frequency to be used for the calculations",
)
path_parser.add_argument(
    "-x",
    type=float,
    default=20.0,
    help="x-coordinate for the source"
)
path_parser.add_argument(
    "-y",
    type=float,
    default=20.0,
    help="y-coordinate for the source"
)
path_parser.add_argument(
    "-z",
    type=float,
    default=20.0,
    help="z-coordinate for the source"
)
path_parser.set_defaults(func=plot_path)

args = main_parser.parse_args()

if args.debug_mode:
    logging.basicConfig(format="%(levelname)s: %(message)s", level=logging.DEBUG)
else:
    logging.basicConfig(format="%(levelname)s: %(message)s", level=logging.INFO)

try:
    args.func(args)
except AttributeError:
    logging.warning("Wrong usage! Consult --help")
    main_parser.print_usage()
