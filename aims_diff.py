#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright CEA (2018-2021).
# Copyright Forschungszentrum Jülich GmbH (2017).
#
# Contributor: Yann Leprince <yann.leprince@ylep.fr>.
#
# This file is part of highres-cortex, a collection of software designed
# to process high-resolution magnetic resonance images of the cerebral
# cortex.
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

from __future__ import print_function

import argparse
import sys

import numpy as np

from soma import aims


BUCKET_TYPES = [getattr(aims, t) for t in dir(aims)
                if t.startswith('BucketMap_')]
# Graph
VOLUME_TYPES = [getattr(aims, t) for t in dir(aims)
                if t.startswith('Volume_')]
# Texture_?
TEXTURE_TYPES = [getattr(aims, t) for t in dir(aims)
                 if t.startswith('TimeTexture_')]
MESH_TYPES = [getattr(aims, t) for t in dir(aims)
              if t.startswith('AimsTimeSurface_')]


def get_data_kind(data_type):
    # Names of data kinds are the human-readable AIMS names (see the list of
    # types for --input in AimsFileConvert --help).
    if data_type in BUCKET_TYPES:
        return 'Bucket'
    elif data_type in VOLUME_TYPES:
        return 'Volume'
    elif data_type in TEXTURE_TYPES:
        return 'Texture'
    elif data_type in MESH_TYPES:
        return 'Mesh'


def diff_headers(reference_header, test_header):
    difference_found = False
    for key, reference_value in reference_header.items():
        try:
            test_value = test_header[key]
        except KeyError:
            print("Header field {0} is missing from the test header"
                  .format(key))
            difference_found = True
        except TypeError as e:
            print("TypeError when reading {0}: {1}".format(key, e))
            continue
        # TODO Special cases for fields containing UUIDs (uuid, referentials).
        # Difference in their values (or in their presence) should not count
        # (except maybe in a strict mode).
        if test_value != reference_value:
            print("Header field {0} differs:\n"
                  "  Reference value: {1}\n"
                  "  Test value: {2}"
                  .format(key, reference_value, test_value))
            difference_found = True
    for key, test_value in test_header.items():
        if key not in reference_header:
            print("Header field {0} is present in the test header, "
                  "not in the reference header (value: {1})"
                  .format(key, test_value))
            difference_found = True
    return difference_found


def diff_volumes(reference_volume, test_volume):
    reference_arr = np.asarray(reference_volume)
    test_arr = np.asarray(test_volume)

    if reference_arr.shape != test_arr.shape:
        print("Volumes have differing shapes:\n"
              "  Shape of reference: {0}\n"
              "  Shape of test: {1}"
              .format(reference_arr.shape, test_arr.shape))
        return True

    if np.array_equiv(reference_arr, test_arr):
        return False

    bool_diff = (reference_arr != test_arr)
    total_diff = np.count_nonzero(bool_diff)
    bool_diff[np.isnan(reference_arr)] = False
    bool_diff[np.isnan(test_arr)] = False
    print("Count of differing values ({0} / {1} total):"
          .format(total_diff, bool_diff.size))
    print("                     reference\n"
          "t          │      NaN      │    non-NaN    │\n"
          "e ─────────┼───────────────┼───────────────┤\n"
          "s      NaN │ {0:13d} │ {1:13d} │\n"
          "t  non-NaN │ {2:13d} │ {3:13d} │\n"
          .format(
              np.count_nonzero(np.logical_and(
                np.isnan(test_arr), np.isnan(reference_arr))),
              np.count_nonzero(np.logical_and(np.isnan(test_arr),
                np.logical_not(np.isnan(reference_arr)))),
              np.count_nonzero(np.logical_and(np.isnan(reference_arr),
                np.logical_not(np.isnan(test_arr)))),
              np.count_nonzero(bool_diff)
          ))
    del bool_diff

    print("Min-max of reference: {0} -- {1}\n"
          "Min-max of test: {2} -- {3}"
          .format(np.nanmin(reference_arr), np.nanmax(reference_arr),
                  np.nanmin(test_arr), np.nanmax(test_arr)))

    abs_diff = np.abs(test_arr - reference_arr)
    print("average absolute difference: {0}".format(np.nanmean(abs_diff)))
    with np.errstate(divide="ignore", invalid="ignore"):
        avg_rel_diff = np.nanmean(abs_diff / np.abs(reference_arr))
    print("average relative difference: {0:.2} %".format(100 * avg_rel_diff))
    print("absolute RMS difference: {0}"
          .format(np.sqrt(np.nanmean(np.square(abs_diff)))))
    return True


def diff_meshes(reference_mesh, test_mesh):
    difference_found = False
    reference_keys = set(reference_mesh.keys())
    test_keys = set(test_mesh.keys())

    if reference_keys != test_keys:
        print("Meshes do not have the same set of keys (i.e. time points):\n"
              "  Reference keys: {0}\n"
              "  Test keys:      {1}\n"
              .format(reference_keys, test_keys))
        difference_found = True

    for key in set(reference_keys) & set(test_keys):
        difference_found = diff_single_mesh(reference_mesh, test_mesh,
                                            key, key) or difference_found

    return difference_found


def diff_textures(reference_texture, test_texture):
    difference_found = False
    reference_keys = set(reference_texture.keys())
    test_keys = set(test_texture.keys())

    if reference_keys != test_keys:
        print("Textures do not have the same set of keys (i.e. time points):\n"
              "  Reference keys: {0}\n"
              "  Test keys:      {1}\n"
              .format(reference_keys, test_keys))
        difference_found = True

    for key in set(reference_keys) & set(test_keys):
        difference_found = diff_single_texture(reference_texture, test_texture,
                                               key, key) or difference_found

    return difference_found


def diff_single_mesh(reference_mesh, test_mesh, reference_key, test_key):
    difference_found = False
    difference_found = diff_mesh_vectors(
        reference_mesh.vertex(reference_key),
        test_mesh.vertex(test_key),
        'reference.vertex({0})'.format(reference_key),
        'test.vertex({0})'.format(test_key)
    ) or difference_found
    difference_found = diff_mesh_vectors(
        reference_mesh.polygon(reference_key),
        test_mesh.polygon(test_key),
        'reference.polygon({0})'.format(reference_key),
        'test.polygon({0})'.format(test_key)
    ) or difference_found
    difference_found = diff_mesh_vectors(
        reference_mesh.normal(reference_key),
        test_mesh.normal(test_key),
        'reference.normal({0})'.format(reference_key),
        'test.normal({0})'.format(test_key)
    ) or difference_found
    difference_found = diff_mesh_vectors(
        reference_mesh.texture(reference_key),
        test_mesh.texture(test_key),
        'reference.texture({0})'.format(reference_key),
        'test.texture({0})'.format(test_key)
    ) or difference_found
    return difference_found


def diff_mesh_vectors(reference_vec, test_vec, reference_name, test_name):
    if reference_vec.size() != test_vec.size():
        print("Vectors {0} and {2} differ in size:\n"
              "  Size of {0}: {1}\n"
              "  Size of {2}: {3}"
              .format(reference_name, reference.size(),
                      test_name, test.size()))
        return True

    if reference_vec == test_vec:
        return False

    # Vectors may compare unequal but still be equivalent (AIMS bug
    # https://github.com/brainvisa/aims-free/issues/42) so we do the comparison
    # with NumPy before printing the message

    reference_arr = np.asarray(reference_vec)
    test_arr = np.asarray(test_vec)

    if np.array_equiv(reference_arr, test_arr):
        return False

    print("Vectors {0} and {1} differ in values:"
          .format(reference_name, test_name))

    # The shapes should always be the same because we return earlier if size()
    # is different.
    assert reference_arr.shape == test_arr.shape

    # Texture arrays have only one dimension
    if reference_arr.ndim < 2:
        reference_arr = reference_arr[:, np.newaxis]
        test_arr = test_arr[:, np.newaxis]

    # TODO handle NaNs (see diff_volumes)
    bool_diff = np.any(reference_arr != test_arr, axis=1)
    print("  {0} / {1} lines differ"
          .format(np.count_nonzero(bool_diff), len(bool_diff)))

    print("  Min-max of reference: {0} -- {1}\n"
          "  Min-max of test: {2} -- {3}"
          .format(np.nanmin(reference_arr), np.nanmax(reference_arr),
                  np.nanmin(test_arr), np.nanmax(test_arr)))

    # TODO reconsider what is meaningful here
    abs_diff = np.abs(test_arr - reference_arr)
    print("  average absolute difference: {0}".format(np.nanmean(abs_diff)))
    with np.errstate(divide="ignore", invalid="ignore"):
        avg_rel_diff = np.nanmean(abs_diff / np.abs(reference_arr))
    print("  average relative difference: {0:.2} %".format(100 * avg_rel_diff))
    print("  absolute RMS difference: {0}"
          .format(np.sqrt(np.nanmean(np.square(abs_diff)))))
    return True


def diff_single_texture(reference_texture, test_texture,
                        reference_key, test_key):
    difference_found = False
    difference_found = diff_mesh_vectors(
        reference_texture[reference_key],
        test_texture[test_key],
        'reference[{0}]'.format(reference_key),
        'test[{0}]'.format(test_key)
    ) or difference_found
    return difference_found


def diff_trm(reference_trm, test_trm):
    if reference_trm == test_trm:
        return False

    print("Transformations differ:\n"
          "======================:\n"
          "Reference transformation:\n{0}\n"
          "Test transformation:\n{1}\n"
          .format(reference_trm, test_trm))
    return True


def diff_files(reference_file_name, test_file_name):
    """Print the differences between two files read by AIMS."""
    reference_data = aims.read(reference_file_name)
    test_data = aims.read(test_file_name)

    kind = get_data_kind(type(reference_data))
    if get_data_kind(type(test_data)) != kind:
        print("Files contain different kinds of data:\n"
              "  Kind of reference: {0}\n"
              "  Kind of test: {1}"
              .format(kind,
                      get_data_kind(type(test_data)) or repr(type(test_data))))
        return 1

    # From now on the data are guaranteed to be of the same kind
    #
    # TODO handle data of the same kind (volumes/mesh/bucket) but of different
    # datatype (VOID/FLOAT/...)

    if type(reference_data) != type(test_data):
        print("Files have differing types, comparison is not yet supported:\n"
              "  Type of reference: {0}\n"
              "  Type of test: {1}"
              .format(type(reference_data), type(test_data)))
        return 1

    difference_found = diff_headers(reference_data.header(),
                                    test_data.header())
    if not difference_found:
        print("Headers are identical")

    data_difference_found = None
    if type(reference_data) in VOLUME_TYPES:
        data_difference_found = diff_volumes(reference_data, test_data)
    elif type(reference_data) in MESH_TYPES:
        data_difference_found = diff_meshes(reference_data, test_data)
    elif type(reference_data) in TEXTURE_TYPES:
        data_difference_found = diff_textures(reference_data, test_data)
    elif type(reference_data) is aims.AffineTransformation3d:
        data_difference_found = diff_trm(reference_data, test_data)
    else:
        print("Comparison of data is not implemented")

    if data_difference_found is not None and not data_difference_found:
        print("Data are identical")
    difference_found = difference_found or data_difference_found

    return 1 if difference_found else 0


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    parser = argparse.ArgumentParser(
        description="""\
Compare two files read by AIMS and summarize the differences""")
    parser.add_argument("reference_file",
                        help="reference data file")
    parser.add_argument("test_file",
                        help="test data file")

    args = parser.parse_args(argv[1:])
    return args

def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    return diff_files(args.reference_file, args.test_file) or 0

if __name__ == "__main__":
    sys.exit(main())
