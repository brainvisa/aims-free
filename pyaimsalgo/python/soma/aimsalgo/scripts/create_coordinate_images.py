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


import os.path
import sys

from soma import aims
import numpy as np


class CommandError(Exception):
    """Exception for abnormal termination of the command."""
    pass


def read_reference_header(reference_filename):
    """Read the header of a reference image."""
    finder = aims.Finder()
    success = finder.check(reference_filename)
    if not success:
        raise CommandError("cannot read the header of the reference image "
                           "'{0}'".format(reference_filename))
    return finder.header()


def create_coordinate_vectors(reference_header):
    """Create three 1D vectors of X, Y, and Z coordinates, respectively."""
    try:
        dim = reference_header['volume_dimension']
        voxel_size = reference_header['voxel_size']
    except KeyError:
        raise CommandError('cannot find the dimensions and voxel size of the '
                           'reference image, aborting.\n')

    x_coord_vector = voxel_size[0] * np.arange(dim[0], dtype=np.float32)
    y_coord_vector = voxel_size[1] * np.arange(dim[1], dtype=np.float32)
    z_coord_vector = voxel_size[2] * np.arange(dim[2], dtype=np.float32)

    return (x_coord_vector, y_coord_vector, z_coord_vector)


def write_coordinate_volumes(reference_filename,
                             output_x_filename,
                             output_y_filename,
                             output_z_filename):
    """Write 3 coordinate volumes in the geometry of a reference volume."""
    reference_header = read_reference_header(reference_filename)
    x_vect, y_vect, z_vect = create_coordinate_vectors(reference_header)
    dim = [len(x_vect), len(y_vect), len(z_vect)]

    # Volumes are written with force_disk_data_type in order to prevent the
    # soma-io Nifti writer (libsomanifti) from encoding the data as scaled
    # int16 (we want to have the full precision of float32, moreover external
    # software would most probably not play well with such encoded data).

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    np.asarray(vol_out)[..., 0] = x_vect[:, np.newaxis, np.newaxis]
    aims.write(vol_out, output_x_filename,
               options={'force_disk_data_type': True})

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    np.asarray(vol_out)[..., 0] = y_vect[np.newaxis, :, np.newaxis]
    aims.write(vol_out, output_y_filename,
               options={'force_disk_data_type': True})

    vol_out = aims.Volume(dim[0], dim[1], dim[2], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    np.asarray(vol_out)[..., 0] = z_vect[np.newaxis, np.newaxis, :]
    aims.write(vol_out, output_z_filename,
               options={'force_disk_data_type': True})


def write_multichannel_4d_coordinate_volume(reference_filename,
                                            output_filename):
    """Write a 3-channel volume of coordinates in a reference geometry."""
    reference_header = read_reference_header(reference_filename)
    x_vect, y_vect, z_vect = create_coordinate_vectors(reference_header)
    dim = [len(x_vect), len(y_vect), len(z_vect)]

    # Volumes are written with force_disk_data_type in order to prevent the
    # soma-io Nifti writer (libsomanifti) from encoding the data as scaled
    # int16 (we want to have the full precision of float32, moreover external
    # software would most probably not play well with such encoded data).

    vol_out = aims.Volume(dim[0], dim[1], dim[2], 3, dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    arr_out = np.asarray(vol_out)
    arr_out[..., 0] = x_vect[:, np.newaxis, np.newaxis]
    arr_out[..., 1] = y_vect[np.newaxis, :, np.newaxis]
    arr_out[..., 2] = z_vect[np.newaxis, np.newaxis, :]
    aims.write(vol_out, output_filename,
               options={'force_disk_data_type': True})


def write_multichannel_5d_coordinate_volume(reference_filename,
                                            output_filename):
    """Write a 3-channel volume of coordinates in a reference geometry."""
    reference_header = read_reference_header(reference_filename)
    x_vect, y_vect, z_vect = create_coordinate_vectors(reference_header)
    dim = [len(x_vect), len(y_vect), len(z_vect)]

    # Volumes are written with force_disk_data_type in order to prevent the
    # soma-io Nifti writer (libsomanifti) from encoding the data as scaled
    # int16 (we want to have the full precision of float32, moreover external
    # software would most probably not play well with such encoded data).

    vol_out = aims.Volume([dim[0], dim[1], dim[2], 1, 3], dtype='FLOAT')
    vol_out.copyHeaderFrom(reference_header)
    arr_out = np.asarray(vol_out)
    arr_out[..., 0, 0] = x_vect[:, np.newaxis, np.newaxis]
    arr_out[..., 0, 1] = y_vect[np.newaxis, :, np.newaxis]
    arr_out[..., 0, 2] = z_vect[np.newaxis, np.newaxis, :]
    aims.write(vol_out, output_filename,
               options={'force_disk_data_type': True})


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    import argparse
    parser = argparse.ArgumentParser(
        usage="%(prog)s [-h] [--4d | --5d] reference_volume "
              "output [output_y output_z]",
        description="""\
Create three volumes, or a 3-channel volume, matching the geometry of a
reference volume, with each volume or channel containing the X, Y, and Z
coordinates expressed in millimetres, respectively.

This is a companion script to the AimsRegisterFFDFromCoordImages command, see
its documentation for more information.

Coordinates are in the sense of AIMS, i.e. (0, 0, 0) is in the centre of the
origin voxel, and coordinates increase toward the Left (X), Posterior (Y),
Inferior (Z) directions (the volume is internally in LPS+ orientation).
""")
    parser.add_argument("reference_volume", help="input reference volume")
    parser.add_argument("output",
                        help="output image volume (either 3-channel if "
                        "output_y and output_z are omitted, or 1-channel "
                        "containing X coordinates if they are present)")
    parser.add_argument("output_y", nargs='?',
                        help="output image volume of Y coordinates")
    parser.add_argument("output_z", nargs='?',
                        help="output image volume of Z coordinates")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--4d", action="store_false",
                       dest="five_d", default=None,
                       help="generate a 4-D multi-channel volume of shape "
                       "(X, Y, Z, 3) [default]")
    group.add_argument("--5d", action="store_true", dest="five_d",
                       help="generate a 5-D multi-channel volume of shape "
                       "(X, Y, Z, 1, 3)")

    args = parser.parse_args(argv[1:])
    if args.five_d is not None:
        if args.output_y is not None or args.output_z is not None:
            parser.error("passing output_y or output_z does not make sense "
                         "in --4d or --5d mode.")
    if args.output_y is None and args.output_z is None:
        args.multichannel = True
    else:
        if args.output_y is None or args.output_z is None:
            parser.error("output_y and output_z must either be both "
                         "specified (for separate channels), or both "
                         "omitted (for a multi-channel output).")
        args.multichannel = False
    return args


def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    try:
        if args.multichannel and args.five_d:
            return write_multichannel_5d_coordinate_volume(
                args.reference_volume,
                args.output) or 0
        elif args.multichannel:
            return write_multichannel_4d_coordinate_volume(
                args.reference_volume,
                args.output) or 0
        else:
            return write_coordinate_volumes(args.reference_volume,
                                            args.output,
                                            args.output_y,
                                            args.output_z) or 0
    except CommandError as e:
        sys.stderr.write("{0}: error: {1}\n"
                         .format(os.path.basename(argv[0]), e))
        return 1


if __name__ == "__main__":
    sys.exit(main())
