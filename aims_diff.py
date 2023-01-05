#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright CEA (2018-2023).
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

from __future__ import (absolute_import, division,
                        print_function, unicode_literals)

import argparse
import sys

import numpy

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

IGNORED_FIELDS = ['uuid']


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
        if key in IGNORED_FIELDS:
            continue
        try:
            test_value = test_header[key]
        except KeyError:
            print("Header field {0} is missing from the test header"
                  .format(key))
            difference_found = True
        except TypeError as e:
            print("TypeError when reading {0}: {1}".format(key, e))
            continue
        if test_value != reference_value:
            print("Header field {0} differs:\n"
                  "  Reference value: {1}\n"
                  "  Test value: {2}"
                  .format(key, reference_value, test_value))
            difference_found = True
    for key, test_value in test_header.items():
        if key in IGNORED_FIELDS:
            continue
        if key not in reference_header:
            print("Header field {0} is present in the test header, "
                  "not in the reference header (value: {1})"
                  .format(key, test_value))
            difference_found = True
    return difference_found


def diff_volumes(reference_volume, test_volume,
                 min_value=None, max_value=None):
    if reference_volume == test_volume:
        return False

    reference_arr = numpy.asarray(reference_volume)
    test_arr = numpy.asarray(test_volume)

    return diff_arrays(reference_arr, test_arr,
                       min_value=min_value, max_value=max_value,
                       text_bits={
        'array': 'volume',
        'Arrays': 'Volumes',
        'indent': '',
    })


DEFAULT_TEXT_BITS = {
    'array': 'volume',
    'Arrays': 'Volumes',
    'indent': '',
}


def diff_arrays(reference_arr, test_arr, text_bits=DEFAULT_TEXT_BITS,
                min_value=None, max_value=None):
    if reference_arr.shape != test_arr.shape:
        print("{indent}{Arrays} have differing shapes:\n"
              "{indent}  Shape of reference: {0}\n"
              "{indent}  Shape of test: {1}"
              .format(reference_arr.shape, test_arr.shape, **text_bits))
        return True

    # Only arrays with numeric dtypes can be compared. Return right away if
    # one array has a non-numeric dtype (dtypes with unsupported conversion to
    # NumPy, like Point3df before AIMS 4.7, are converted to Object).
    if (reference_arr.dtype.fields is not None
            and 'v' in reference_arr.dtype.fields):
        reference_arr = reference_arr['v']
    if not numpy.issubdtype(reference_arr.dtype, numpy.number):
        print("{indent}A detailed data comparison cannot be done, because the "
              "NumPy data type for the reference {array} ({0}) is not numeric."
              .format(reference_arr.dtype, **text_bits))
        return True

    if (test_arr.dtype.fields is not None
            and 'v' in test_arr.dtype.fields):
        test_arr = test_arr['v']
    if not numpy.issubdtype(test_arr.dtype, numpy.number):
        print("{indent}A detailed data comparison cannot be done, because the "
              "NumPy data type for the test {array} ({0}) is not numeric."
              .format(test_arr.dtype, **text_bits))
        return True

    try:
        common_dtype = numpy.promote_types(reference_arr.dtype, test_arr.dtype)
    except TypeError:
        common_dtype = numpy.void

    if common_dtype == numpy.void:
        print("{indent}{Arrays} have incompatible data types:\n"
              "{indent}  Data type of reference: {0}\n"
              "{indent}  Data type of test: {1}"
              .format(reference_arr.dtype, test_arr.dtype, **text_bits))
        return True

    if numpy.array_equiv(reference_arr, test_arr):
        return False

    print("{indent}{Arrays} have different data:"
          .format(**text_bits))

    # Do this test before converting to floating-point (no loss of precision)
    bool_diff = (reference_arr != test_arr)

    # Do the numerical comparisons using a floating-point type
    comparison_dtype = numpy.promote_types(common_dtype, numpy.float32)
    reference_arr = reference_arr.astype(comparison_dtype, casting='safe')
    test_arr = test_arr.astype(comparison_dtype, casting='safe')

    with numpy.errstate(invalid="ignore"):
        if min_value is not None:
            reference_arr[reference_arr < min_value] = numpy.nan
            test_arr[test_arr < min_value] = numpy.nan
        if max_value is not None:
            reference_arr[reference_arr > max_value] = numpy.nan
            test_arr[test_arr > max_value] = numpy.nan

    has_nans = False
    reference_nans = numpy.isnan(reference_arr)
    if numpy.any(reference_nans):
        bool_diff[reference_nans] = False
        has_nans = True
    del reference_nans

    test_nans = numpy.isnan(test_arr)
    if numpy.any(test_nans):
        bool_diff[test_nans] = False
        has_nans = True
    del test_nans

    total_diff_count = numpy.count_nonzero(bool_diff)

    print("{indent}  {0} value(s) differ ({2:.02%} of {1})"
          .format(total_diff_count,
                  '×'.join(str(n) for n in bool_diff.shape),
                  total_diff_count / bool_diff.size,
                  **text_bits))
    if has_nans:
        print(
            "{indent}  Breakdown of differing values:"
            "{indent}                         reference\n"
            "{indent}    t          │      NaN      │    non-NaN    │\n"
            "{indent}    e ─────────┼───────────────┼───────────────┤\n"
            "{indent}    s      NaN │ {0:13d} │ {1:13d} │\n"
            "{indent}    t  non-NaN │ {2:13d} │ {3:13d} │\n".format(
                numpy.count_nonzero(numpy.logical_and(
                    numpy.isnan(test_arr),
                    numpy.isnan(reference_arr)
                )),
                numpy.count_nonzero(numpy.logical_and(
                    numpy.isnan(test_arr),
                    numpy.logical_not(numpy.isnan(reference_arr))
                )),
                numpy.count_nonzero(numpy.logical_and(
                    numpy.isnan(reference_arr),
                    numpy.logical_not(numpy.isnan(test_arr))
                )),
                numpy.count_nonzero(bool_diff),
                **text_bits
            ))
    del bool_diff

    # Print most different values and their coordinates
    MAX_DIFF_TO_PRINT = 5
    diff_arr = numpy.abs(test_arr - reference_arr)
    # Do not show differences where one of the inputs is NaN. Note that we do
    # not test for diff_arr to be NaN, because NaN could be the result of Inf -
    # Inf, which is a useful difference to show.
    diff_arr[numpy.isnan(test_arr)] = 0
    diff_arr[numpy.isnan(reference_arr)] = 0
    max_diff_indices = numpy.argsort(diff_arr, axis=None)
    print(
        "{indent}  "
        "Summary of the most different values (in absolute difference):\n"
        "{indent}    "
        "│ Reference value │   Test value    │ Abs. difference │ Coordinates\n"
        "{indent}    "
        "├─────────────────┼─────────────────┼─────────────────┼─────────────"
        .format(**text_bits)
    )
    for i in range(1, MAX_DIFF_TO_PRINT + 1):
        current_diff_index = max_diff_indices[-i]
        if diff_arr.flat[current_diff_index] == 0:
            break  # only display values that actually differ
        print(
            "{indent}    │{0: ^17.17g}│{1: ^17.17g}│{2: ^17.17g}│ {3}"
            .format(reference_arr.flat[current_diff_index],
                    test_arr.flat[current_diff_index],
                    diff_arr.flat[current_diff_index],
                    numpy.unravel_index(current_diff_index, diff_arr.shape),
                    **text_bits)
        )

    print("{indent}  Min-max of reference: {0} -- {1}"
          .format(numpy.nanmin(reference_arr), numpy.nanmax(reference_arr),
                  **text_bits))
    print("{indent}  Min-max of test: {0} -- {1}"
          .format(numpy.nanmin(test_arr), numpy.nanmax(test_arr), **text_bits))

    abs_diff = numpy.abs(test_arr - reference_arr)
    print("{indent}  Average absolute difference: {0}"
          .format(numpy.nanmean(abs_diff), **text_bits))
    print("{indent}  Min-max absolute difference: {0} -- {1}"
          .format(numpy.nanmin(abs_diff), numpy.nanmax(abs_diff), **text_bits))
    print("{indent}  Median absolute difference: {0}"
          .format(numpy.nanmedian(abs_diff), **text_bits))
    print("{indent}  Mean absolute difference: {0}"
          .format(numpy.nanmean(abs_diff), **text_bits))
    print("{indent}  RMS absolute difference: {0}"
          .format(numpy.sqrt(numpy.nanmean(numpy.square(abs_diff))),
                  **text_bits))

    with numpy.errstate(divide="ignore", invalid="ignore"):
        avg_rel_diff = 2 * numpy.nanmean(abs_diff / (numpy.abs(reference_arr)
                                                     + numpy.abs(test_arr)))
    print("{indent}  Average relative difference: {0:%}"
          .format(avg_rel_diff, **text_bits))

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


def diff_textures(reference_texture, test_texture,
                  min_value=None, max_value=None):
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
        difference_found = diff_single_texture(
            reference_texture, test_texture, key, key,
            min_value=min_value, max_value=max_value) or difference_found

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


def diff_mesh_vectors(reference_vec, test_vec, reference_name, test_name,
                      min_value=None, max_value=None):
    if reference_vec.size() != test_vec.size():
        print("Vectors {0} and {2} differ in size:\n"
              "  Size of {0}: {1}\n"
              "  Size of {2}: {3}"
              .format(reference_name, reference_vec.size(),
                      test_name, test_vec.size()))
        return True

    if reference_vec == test_vec:
        if reference_vec.size() != 0:
            # print the message only for non-empty vectors
            print("Vectors {0} and {1} are identical."
                  .format(reference_name, test_name))
        return False

    # Vectors may compare unequal but still be equivalent (AIMS bug
    # https://github.com/brainvisa/aims-free/issues/42) so we do the comparison
    # with NumPy before printing the message

    reference_arr = numpy.asarray(reference_vec)
    test_arr = numpy.asarray(test_vec)

    bool_diff = numpy.any(reference_arr != test_arr, axis=1)
    print("  {0} / {1} lines differ"
          .format(numpy.count_nonzero(bool_diff), len(bool_diff)))

    if reference_name == test_name:
        vector_name = reference_name
    else:
        vector_name = reference_name + '/' + test_name

    return diff_arrays(reference_arr, test_arr,
                       min_value=min_value, max_value=max_value,
                       text_bits={
        'array': 'vector {0}'.format(vector_name),
        'Arrays': 'Vectors {0}'.format(vector_name),
        'indent': '',
    })


def diff_single_texture(reference_texture, test_texture,
                        reference_key, test_key,
                        min_value=None, max_value=None):
    difference_found = False
    difference_found = diff_mesh_vectors(
        reference_texture[reference_key],
        test_texture[test_key],
        'reference[{0}]'.format(reference_key),
        'test[{0}]'.format(test_key),
        min_value=min_value, max_value=max_value,
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


def diff_files(reference_file_name, test_file_name, reader="AIMS",
               min_value=None, max_value=None):
    if reader == "AIMS":
        return diff_files_AIMS(reference_file_name, test_file_name,
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
    difference_found = diff_arrays(reference_arr, test_arr,
                                   min_value=min_value,
                                   max_value=max_value,
                                   text_bits={
        'array': 'volume',
        'Arrays': 'Volumes',
        'indent': '',
    })
    return 1 if difference_found else 0


def diff_files_AIMS(reference_file_name, test_file_name,
                    min_value=None, max_value=None):
    """Print the differences between two files read by AIMS."""
    from soma import aims
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
        data_difference_found = diff_volumes(reference_data, test_data,
                                             min_value=min_value,
                                             max_value=max_value)
    elif type(reference_data) in MESH_TYPES:
        data_difference_found = diff_meshes(reference_data, test_data)
    elif type(reference_data) in TEXTURE_TYPES:
        data_difference_found = diff_textures(reference_data, test_data,
                                             min_value=min_value,
                                             max_value=max_value)
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
    parser.add_argument("--reader", default="AIMS",
                        choices=("AIMS", "nibabel"),
                        help="library used for reading the data")
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
    return diff_files(args.reference_file, args.test_file,
                      reader=args.reader,
                      min_value=args.min_value,
                      max_value=args.max_value) or 0


if __name__ == "__main__":
    sys.exit(main())
