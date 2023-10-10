#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright CEA (2018-2023).
# Copyright Forschungszentrum Jülich GmbH (2017).
#
# Author: Yann Leprince <yann.leprince@ylep.fr>.
#
# This software is governed by the CeCILL licence under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and/or redistribute the software under the terms of the CeCILL
# licence as circulated by CEA, CNRS and INRIA at the following URL:
# <http://www.cecill.info/>.
#
# As a counterpart to the access to the source code and rights to copy,
# modify and redistribute granted by the licence, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights, and the successive licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading, using, modifying and/or developing or reproducing the
# software by the user in light of its specific status of scientific
# software, that may mean that it is complicated to manipulate, and that
# also therefore means that it is reserved for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and, more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL licence and that you accept its terms.

import argparse
import sys

import numpy

from soma import aims
import soma.aims.diff


def diff_files(reference_file_name, test_file_name, reader="AIMS",
               min_value=None, max_value=None):
    if reader == "AIMS":
        return aims.diff.diff_files(reference_file_name, test_file_name,
                                    min_value=min_value, max_value=max_value)
    elif reader == "nibabel":
        return diff_files_nibabel(reference_file_name, test_file_name,
                                  min_value=min_value, max_value=max_value)


def diff_files_nibabel(reference_file_name, test_file_name,
                       min_value=None, max_value=None):
    """Print the differences between two files read by nibabel."""
    import nibabel
    reference_data = nibabel.load(reference_file_name)
    reference_arr = numpy.array(reference_data.dataobj)
    test_data = nibabel.load(test_file_name)
    test_arr = numpy.array(test_data.dataobj)
    difference_found = aims.diff.diff_arrays(reference_arr, test_arr,
                                             min_value=min_value,
                                             max_value=max_value,
                                             text_bits={
        'array': 'volume',
        'Arrays': 'Volumes',
        'indent': '',
    })
    return difference_found


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    parser = argparse.ArgumentParser(
        description="""\
Compare two files read by AIMS and summarize the differences.

Please note that this program is intended for interactive use. The formatting
of the output should not be considered stable, it may be modified or extended
in the future.
""")
    parser.add_argument("reference_file",
                        help="reference data file")
    parser.add_argument("test_file",
                        help="test data file")
    parser.add_argument("--reader", default="AIMS",
                        choices=("AIMS", "nibabel"),
                        help="library used for reading the data (nibabel "
                             "is experimental)")
    parser.add_argument("--min-value", type=float, default=None,
                        help="minimum value to be taken into account in "
                        "comparisons (smaller values are replaced by NaN "
                        "internally")
    parser.add_argument("--max-value", type=float, default=None,
                        help="maximum value to be taken into account in "
                        "comparisons (larger values are replaced by NaN "
                        "internally")

    args = parser.parse_args(argv[1:])
    return args


def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    return 1 if diff_files(args.reference_file, args.test_file,
                           reader=args.reader,
                           min_value=args.min_value,
                           max_value=args.max_value) else 0


if __name__ == "__main__":
    sys.exit(main())
