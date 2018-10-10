#! /usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright CEA (2017, 2018).
#
# Contributor: Yann Leprince <yann.leprince@cea.fr>.
# Contributor: Jessica Lebenberg <jessica.lebenberg@cea.fr>.
#
# This software and supporting documentation are distributed by
#     Institut Federatif de Recherche 49
#     CEA/NeuroSpin, Batiment 145,
#     91191 Gif-sur-Yvette cedex
#     France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.


import sys

from soma import aims
import numpy as np


EXIT_FAILURE = 1


def write_coordinate_volumes(reference_filename,
                             image_x_filename,
                             image_y_filename,
                             image_z_filename):
    """Write 3 coordinate volumes in the geometry of a reference volume."""
    finder = aims.Finder()
    success = finder.check(reference_filename)
    if not success:
        return EXIT_FAILURE
    reference_header = finder.header()

    try:
        dim = reference_header['volume_dimension']
        voxel_size = reference_header['voxel_size']
    except KeyError:
        sys.stderr.write('Cannot find the dimensions and voxel size of the '
                         'reference, aborting.\n')
        return EXIT_FAILURE

    # Volumes are written with force_disk_data_type in order to prevent the
    # soma-io Nifti writer (libsomanifti) from encoding the data as scaled
    # int16 (we want to have the full precision of float32, moreover external
    # software would most probably not play well with such encoded data).

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    vect = voxel_size[0] * np.arange(dim[0], dtype=np.float32)
    np.asarray(vol_out)[..., 0] = vect[:, np.newaxis, np.newaxis]
    aims.write(vol_out, image_x_filename,
               options={'force_disk_data_type': True})

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    vect = voxel_size[1] * np.arange(dim[1], dtype=np.float32)
    np.asarray(vol_out)[..., 0] = vect[np.newaxis, :, np.newaxis]
    aims.write(vol_out, image_y_filename,
               options={'force_disk_data_type': True})

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    vect = voxel_size[2] * np.arange(dim[2], dtype=np.float32)
    np.asarray(vol_out)[..., 0] = vect[np.newaxis, np.newaxis, :]
    aims.write(vol_out, image_z_filename,
               options={'force_disk_data_type': True})


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    import argparse
    parser = argparse.ArgumentParser(
        description="""\
Create three volumes matching the geometry of a reference volume, respectively
containing the fields of X, Y, and Z coordinates expressed in millimetres.

This is a companion script to the AimsRegisterFFDFromCoordImages command, see
its documentation for more information.

Coordinates are in the sense of AIMS, i.e. (0, 0, 0) is in the centre of the
origin voxel, and coordinates increase toward the Left (X), Posterior (Y),
Inferior (Z) directions (the volume is internally in LPS+ orientation).
""")
    parser.add_argument("reference_volume", help="input reference volume")
    parser.add_argument("output_x", help="output image of X coordinates")
    parser.add_argument("output_y", help="output image of Y coordinates")
    parser.add_argument("output_z", help="output image of Z coordinates")

    args = parser.parse_args(argv[1:])
    return args


def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    return write_coordinate_volumes(args.reference_volume,
                                    args.output_x,
                                    args.output_y,
                                    args.output_z) or 0


if __name__ == "__main__":
    sys.exit(main())
