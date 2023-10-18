#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
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

from . import graph_comparison


BUCKET_TYPES = tuple(getattr(aims, t) for t in dir(aims)
                     if t.startswith('BucketMap_')
                     or t.startswith('rc_ptr_BucketMap_'))
VOLUME_TYPES = tuple(getattr(aims, t) for t in dir(aims)
                     if t.startswith('Volume_')
                     or t.startswith('rc_ptr_Volume_'))
# Texture_?
TEXTURE_TYPES = tuple(getattr(aims, t) for t in dir(aims)
                      if t.startswith('TimeTexture_')
                      or t.startswith('rc_ptr_TimeTexture_'))
MESH_TYPES = tuple(getattr(aims, t) for t in dir(aims)
                   if t.startswith('AimsTimeSurface_')
                   or t.startswith('rc_ptr_AimsTimeSurface_'))

TESTABLE_TYPES = (
    (aims.Graph, aims.AffineTransformation3d)
    + BUCKET_TYPES
    + VOLUME_TYPES
    + TEXTURE_TYPES
    + MESH_TYPES
)

DEFAULT_IGNORED_FIELDS = ['uuid']


DEFAULT_TEXT_BITS = {
    'array': 'volume',
    'Arrays': 'Volumes',
    'indent': '',
}

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


# TODO: kinds of array comparisons:
# - data value
# - geometry (spatial coordinates)
# - label (incl. binary)
# - bucket
# - mesh triangles


class Differentiator:
    def __init__(self):
        self.ignored_header_fields = list(DEFAULT_IGNORED_FIELDS)  # copy
        self.min_value = numpy.nan
        self.max_value = numpy.nan
        self.n_max_diff_to_print = 5
        self.structure_verbosity = 1
        self.data_verbosity = 1
        self.atol = 0
        self.rtol = 0

    def diff_headers(self, reference_header, test_header):
        difference_found = False
        for key, reference_value in reference_header.items():
            if key in self.ignored_header_fields:
                continue
            try:
                test_value = test_header[key]
            except KeyError:
                print("Header field {0} is missing from the test header"
                      "(reference value: {1}"
                      .format(key, reference_value))
                difference_found = True
                continue
            except TypeError as e:
                print("TypeError when reading {0}: {1}".format(key, e))
                continue
            if test_value != reference_value:
                # TODO special case for NaN
                print("Header field {0} differs:\n"
                      "  Reference value: {1}\n"
                      "  Test value: {2}"
                      .format(key, reference_value, test_value))
                difference_found = True
        for key, test_value in test_header.items():
            if key in self.ignored_header_fields:
                continue
            if key not in reference_header:
                print("Header field {0} is present in the test header, "
                      "not in the reference header (value: {1})"
                      .format(key, test_value))
                difference_found = True
        return difference_found


    def diff_volumes(self, reference_volume, test_volume):
        if reference_volume == test_volume:
            return False

        reference_arr = numpy.asarray(reference_volume)
        test_arr = numpy.asarray(test_volume)

        return self.diff_arrays(reference_arr, test_arr,
                           text_bits={
            'array': 'volume',
            'Arrays': 'Volumes',
            'indent': '',
        })


    def diff_arrays(self, reference_arr, test_arr, text_bits=DEFAULT_TEXT_BITS):
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
            if self.min_value is not None:
                reference_arr[reference_arr < self.min_value] = numpy.nan
                test_arr[test_arr < self.min_value] = numpy.nan
            if self.max_value is not None:
                reference_arr[reference_arr > self.max_value] = numpy.nan
                test_arr[test_arr > self.max_value] = numpy.nan

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
        for i in range(1, self.n_max_diff_to_print + 1):
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


    def diff_bucketmaps(self, reference_bucketmap, test_bucketmap):
        difference_found = False
        reference_keys = set(reference_bucketmap.keys())
        test_keys = set(test_bucketmap.keys())

        if reference_keys != test_keys:
            print("BucketMaps do not have the same set of keys "
                  "(i.e. time points):\n"
                  "  Reference keys: {0}\n"
                  "  Test keys:      {1}\n"
                  .format(reference_keys, test_keys))
            difference_found = True

        for key in set(reference_keys) & set(test_keys):
            difference_found = self.diff_single_bucket(
                reference_bucketmap, test_bucketmap, key, key) or difference_found

        return difference_found


    def diff_single_bucket(self, reference_bucketmap, test_bucketmap,
                           reference_key, test_key):
        difference_found = False
        difference_found = self.diff_mesh_vectors(
            reference_bucketmap[reference_key].keys(),
            test_bucketmap[test_key].keys(),
            'reference[{0}].keys()'.format(reference_key),
            'test[{0}].keys()'.format(test_key),
        ) or difference_found
        if isinstance(reference_bucketmap, aims.BucketMap_VOID.Bucket):
            difference_found = self.diff_mesh_vectors(
                reference_bucketmap[reference_key],
                test_bucketmap[test_key],
                'reference[{0}] bucket values'.format(reference_key),
                'test[{0}] bucket values'.format(test_key),
            ) or difference_found
        return difference_found


    def diff_meshes(self, reference_mesh, test_mesh):
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
            difference_found = self.diff_single_mesh(reference_mesh, test_mesh,
                                                     key, key) or difference_found

        return difference_found


    def diff_textures(self, reference_texture, test_texture):
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
            difference_found = self.diff_single_texture(
                reference_texture, test_texture, key, key) or difference_found

        return difference_found


    def diff_single_mesh(self, reference_mesh, test_mesh, reference_key, test_key):
        difference_found = False
        difference_found = self.diff_mesh_vectors(
            reference_mesh.vertex(reference_key),
            test_mesh.vertex(test_key),
            'reference.vertex({0})'.format(reference_key),
            'test.vertex({0})'.format(test_key)
        ) or difference_found
        difference_found = self.diff_mesh_vectors(
            reference_mesh.polygon(reference_key),
            test_mesh.polygon(test_key),
            'reference.polygon({0})'.format(reference_key),
            'test.polygon({0})'.format(test_key)
        ) or difference_found
        difference_found = self.diff_mesh_vectors(
            reference_mesh.normal(reference_key),
            test_mesh.normal(test_key),
            'reference.normal({0})'.format(reference_key),
            'test.normal({0})'.format(test_key)
        ) or difference_found
        difference_found = self.diff_mesh_vectors(
            reference_mesh.texture(reference_key),
            test_mesh.texture(test_key),
            'reference.texture({0})'.format(reference_key),
            'test.texture({0})'.format(test_key)
        ) or difference_found
        return difference_found


    def diff_mesh_vectors(self, reference_vec, test_vec, reference_name, test_name):
        if len(reference_vec) != len(test_vec):
            print("Vectors {0} and {2} differ in size:\n"
                  "  Size of {0}: {1}\n"
                  "  Size of {2}: {3}"
                  .format(reference_name, len(reference_vec),
                          test_name, len(test_vec)))
            return True

        if reference_vec == test_vec:
            if len(reference_vec) != 0:
                # print the message only for non-empty vectors
                print("Vectors {0} and {1} are identical."
                      .format(reference_name, test_name))
            return False

        # Vectors may compare unequal but still be equivalent (AIMS bug
        # https://github.com/brainvisa/aims-free/issues/42) so we do the comparison
        # with NumPy before printing the message

        reference_arr = numpy.asarray(reference_vec)
        test_arr = numpy.asarray(test_vec)

        if reference_name == test_name:
            vector_name = reference_name
        else:
            vector_name = reference_name + '/' + test_name

        return self.diff_arrays(reference_arr, test_arr,
                           text_bits={
            'array': 'vector {0}'.format(vector_name),
            'Arrays': 'Vectors {0}'.format(vector_name),
            'indent': '',
        })


    def diff_single_texture(self, reference_texture, test_texture,
                            reference_key, test_key):
        difference_found = False
        difference_found = self.diff_mesh_vectors(
            reference_texture[reference_key],
            test_texture[test_key],
            'reference[{0}]'.format(reference_key),
            'test[{0}]'.format(test_key),
        ) or difference_found
        return difference_found


    def diff_trm(self, reference_trm, test_trm):
        if reference_trm == test_trm:
            return False

        print("Transformations differ:\n"
              "======================:\n"
              "Reference transformation:\n{0}\n"
              "Test transformation:\n{1}\n"
              .format(reference_trm, test_trm))
        return True


    def diff_objects(self, reference_data, test_data):
        """Print the differences between two files read by AIMS."""
        kind = get_data_kind(type(reference_data))
        if get_data_kind(type(test_data)) != kind:
            print("Files contain different kinds of data:\n"
                  "  Kind of reference: {0}\n"
                  "  Kind of test: {1}"
                  .format(kind,
                          get_data_kind(type(test_data)) or repr(type(test_data))))
            return True

        # From now on the data are guaranteed to be of the same kind (not
        # necessarily the same datatype)

        # Handle rc_ptr
        if hasattr(reference_data, "isNull"):
            if reference_data.isNull():
                # Null == Null, non-null != null
                if hasattr(reference_data, "isNull"):
                    return reference_data.isNull()
                else:
                    return False
            else:
                reference_data = reference_data.get()
        if hasattr(test_data, "isNull"):
            if not test_data.isNull():
                test_data = test_data.get()
            else:
                return False

        if (hasattr(reference_data, "header")
                and hasattr(test_data, "header")):
            difference_found = self.diff_headers(reference_data.header(),
                                                 test_data.header())
            if not difference_found:
                print("Headers are identical")

        data_difference_found = None
        if type(reference_data) in VOLUME_TYPES:
            data_difference_found = self.diff_volumes(reference_data, test_data)
        elif type(reference_data) in MESH_TYPES:
            data_difference_found = self.diff_meshes(reference_data, test_data)
        elif type(reference_data) in TEXTURE_TYPES:
            data_difference_found = self.diff_textures(reference_data, test_data)
        elif type(reference_data) in BUCKET_TYPES:
            data_difference_found = self.diff_bucketmaps(reference_data, test_data)
        elif type(reference_data) is aims.AffineTransformation3d:
            data_difference_found = self.diff_trm(reference_data, test_data)
        elif type(reference_data) is aims.Graph:
            difference_found = not graph_comparison.same_graphs(
                reference_data, test_data, verbose=True)
        else:
            raise NotImplementedError("Comparison of {} is not implemented"
                                      .format(type(reference_data)))

        if data_difference_found is not None and not data_difference_found:
            print("Data are identical")
        difference_found = difference_found or data_difference_found

        return difference_found

    def same_graphs(self, ref_graph, test_graph, verbose=False):
        '''
        Compare two graphs and return if they are identical.
        This function is useful for testing and validation purposes.
        Graphs structure are compared, and vertices / edges attributes.
        AIMS objects inside attributes are not compared (meshes, bucket, volumes
        in sulci graphs for instance)

        Parameters
        ----------
        ref_graph: string or Graph object
            reference graph to be compared. A filename may be passed here: in this
            case the graph is read using aims.read() function.
        test_graph: string or Graph object
            test graph to be compared. A filename may be passed here: in this
            case the graph is read using aims.read() function.
        verbose: bool (optional, default: False)
            if True, messages are print on the standard output during comparison.

        Returns
        -------
            True if ref and test graphs are identical, or False otherwise.
        '''

        if isinstance(ref_graph, six.string_types):
            ref_graph = aims.read(ref_graph)
        if isinstance(test_graph, six.string_types):
            test_graph = aims.read(test_graph)

        ref_vertices = _build_vertice_dictionary(ref_graph)
        test_vertices = _build_vertice_dictionary(test_graph)

        if verbose:
            print("Compare vertices:")
        if not _same_dictionary(ref_vertices, test_vertices, _same_vertice,
                                verbose):
            if verbose:
                print("  differences in vertices")
            return False

        ref_edges = _build_edge_dictionary(ref_graph)
        test_edges = _build_edge_dictionary(test_graph)

        if verbose:
            print("Compare edges:")
        if not _same_dictionary(ref_edges, test_edges, _same_edge, verbose):
            if verbose:
                print("  differences in edges")
            return False

        return True


    def _build_vertice_dictionary(self, graph):
        vertice_dict = {}
        for vertice in graph.vertices():
            vertice_dict[vertice['index']] = vertice
        return vertice_dict


    def _build_edge_dictionary(self, graph):
        edge_dict = {}
        for edge in graph.edges():
            index = (edge.vertices()[0]['index'], edge.vertices()[1]['index'],
                     edge.getSyntax())
            assert(index not in edge_dict)
            edge_dict[index] = edge
        return edge_dict


    def _same_dictionary(self, ref_dict, test_dict, same_element_function, verbose=False):
        if verbose:
            print("  first:  " + str(len(ref_dict)) + " elements.")
            print("  second: " + str(len(test_dict)) + " elements.")
        if not len(ref_dict) == len(test_dict):
            if verbose:
                print("  different number of elements.")
            return False
        same_dict = True
        for index, ref in six.iteritems(ref_dict):
            same_item = True
            if index not in test_dict:
                same_item = False
                same_dict = False
                if verbose:
                    print("  test dict has no element with index " + repr(index))
                else:
                    break
            if same_item:
                same_item = same_element_function(ref, test_dict[index], verbose)
            if not same_item:
                same_dict = False
                if verbose:
                    print("difference in " + repr(index))
                else:
                    break
        return same_dict


    def _same_value(self, ref_value, test_value):
        if isinstance(ref_value, diff.TESTABLE_TYPES):
            return (diff.diff_objects(ref_value, test_value) == 0)
        elif isinstance(ref_value, float):
            if ref_value != test_value and \
                    abs(ref_value - test_value) / max(abs(ref_value),
                                                      abs(test_value)) \
                        > rel_flx_max_diff:
                return False
        elif not isinstance(ref_value, str) and hasattr(ref_value, '__len__'):
            #print('comp lists', ref_value, '*/*', test_value)
            if len(ref_value) != len(test_value):
                return False
            return len([1 for x, y in zip(ref_value, test_value)
                        if not _same_value(x, y)]) == 0
        elif str(ref_value) != str(test_value):
            return False
        return True


    def _same_vertice(ref_vertice, test_vertice, verbose):
        if not len(ref_vertice) == len(test_vertice):
            if verbose:
                print("  different number of arguments " + repr(len(ref_vertice))
                      + " " + repr(len(test_vertice)))
            return False
        for key in ref_vertice.keys():
            if key not in test_vertice:
                if verbose:
                    print(repr(key) + " not in test_vertice")
                return False
            ref_value = ref_vertice[key]
            test_value = test_vertice[key]
            if not _same_value(ref_value, test_value):
                if verbose:
                    print("vertex " + repr(key) + " " + str(ref_vertice[key])
                          + " != " + str(test_vertice[key]))
                return False
        return True


    def _same_edge(self, ref_edge, test_edge, verbose):
        # TODO remove the duplication with same_vertice
        same = len(ref_edge) == len(test_edge)
        if not same:
            if verbose:
                print("different numbers of arguments " + repr(len(ref_edge))
                      + " " + repr(len(test_edge)))
            return False
        for key in ref_edge.keys():
            if key not in test_edge:
                if verbose:
                    print(repr(key) + " not in test_edge")
                return False
            ref_value = ref_edge[key]
            test_value = test_edge[key]
            if not _same_value(ref_value, test_value):
                if verbose:
                    print("edge " + repr(key) + " " + str(ref_edge[key]) + " "
                          + str(test_edge[key]))
                return False
        return True


def diff_objects(reference_data, test_data,
                 min_value=None, max_value=None):
    """Print the differences between two AIMS objects."""
    differentiator = Differentiator()
    differentiator.min_value = min_value
    differentiator.max_value = max_value

    return differentiator.diff_objects(reference_data, test_data)


def diff_files(reference_file_name, test_file_name,
               min_value=None, max_value=None):
    """Print the differences between two files read by AIMS."""
    reference_data = aims.read(reference_file_name)
    test_data = aims.read(test_file_name)

    differentiator = Differentiator()
    differentiator.min_value = min_value
    differentiator.max_value = max_value

    return differentiator.diff_objects(reference_data, test_data)
