# -*- coding: utf-8 -*-
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
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

from __future__ import print_function

from __future__ import absolute_import
from soma import aims
import six
import sys

if sys.version_info[0] >= 3:
    six.string_types = str


def same_graphs(ref_graph, test_graph, verbose=False):
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


def _build_vertice_dictionary(graph):
    vertice_dict = {}
    for vertice in graph.vertices():
        vertice_dict[vertice['index']] = vertice
    return vertice_dict


def _build_edge_dictionary(graph):
    edge_dict = {}
    for edge in graph.edges():
        index = (edge.vertices()[0]['index'], edge.vertices()[1]['index'],
                 edge.getSyntax())
        assert(index not in edge_dict)
        edge_dict[index] = edge
    return edge_dict


def _same_dictionary(ref_dict, test_dict, same_element_function, verbose=False):
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
        if key not in ['aims_bottom', 'aims_other', 'aims_ss', 'aims_Tmtktri',
                       'bottom_label', 'other_label', 'ss_label',
                       'Tmtktri_label']:
            # XXX ref_vertice and test_vertice should be compared directly
            if str(ref_vertice[key]) != str(test_vertice[key]):
                if verbose:
                    print("vertice " + repr(key) + " " + str(ref_vertice[key])
                          + " != " + str(test_vertice[key]))
                return False
    return True


def _same_edge(ref_edge, test_edge, verbose):
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
        if key not in ['aims_cortical', 'aims_junction', 'aims_plidepassage',
                       'cortical_label', 'junction_label',
                       'plidepassage_label']:
            # XXX ref_edge and test_edge should be compared directly
            if str(ref_edge[key]) != str(test_edge[key]):
                if verbose:
                    print("edge " + repr(key) + " " + str(ref_edge[key]) + " "
                          + str(test_edge[key]))
                return False
    return True
