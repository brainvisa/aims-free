#!/usr/bin/env python

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
def graphAddAttributeFromCsv(input_graph, output_graph, input_csv, table_key_col, graph_key_attr, graph_val_attr, table_val_col, verbose):
    from soma import aims
    import pandas as pd
    import numpy as np
    #
    r = aims.Reader()
    w = aims.Writer()
    g = r.read(input_graph)
    delims = ',;\t '
    for i, delim in enumerate(delims):
        try:
            d = pd.read_csv(input_csv, sep='\s*%s\s*' % delim, engine='python')
            break
        except:
            if i == len(delims) - 1:
                raise
            pass

    for v in g.vertices():
        label = v.get(graph_key_attr)
        if not label:
            continue
        mask_idx = np.where(d[d.columns[table_key_col]] == label)[0]
        if len(mask_idx) == 0:
            if verbose:
                print(v[graph_key_attr], "\thas no correspondance in the table",
                      input_csv)
            continue
        if len(mask_idx) > 1:
            if verbose:
                print(v[graph_key_attr],
                      "\thas multiple correspondances in the table", input_csv)
            continue
        val = d.loc[mask_idx, d.columns[table_val_col]].item()
        v[graph_val_attr] = val
    w.write(g, output_graph)


if __name__ == '__main__':
    # Configure parser --------------------------------------------------------
    from optparse import OptionParser
    import sys
    import os.path
    parser = OptionParser(
        description='Add an attritute to some nodes of a graph. The attribute stem from a csv file with an header. Each line of this file is a pair of node-name value',
        epilog="""%s -i input_fold_graph.arg -j stats-values.csv""" % os.path.basename(
            sys.argv[0])
    )
    parser.add_option(
        "-i", dest="input_graph", help="Input graph", metavar="FILE")
    parser.add_option("-o", dest="output_graph",
                      help="Output graph (default=out.arg)", metavar="FILE", default="out.arg")
    parser.add_option(
        "-j", dest="input_csv", help="Input csv file",  metavar="FILE")
    # graph attributes options
    parser.add_option("-k", dest="graph_key_attr", default="name",
                      help="graph key attribute: the attribute name which identifies the graph nodes (default=name)", metavar="STRING")
    parser.add_option("-l", dest="graph_val_attr", default="custom_num_val",
                      help="graph value attribute: the attribute name of the graph nodes that will be used to store the value, (default=custom_num_val)", metavar="STRING")
    #  table (csv) columns names options
    parser.add_option("-m", dest="table_key_col", default=0,
                      help="table (csv) column name|number which contains keys to be matched with attribute name of the graph (default=0, first column)", metavar="STRING|INT")
    parser.add_option("-n", dest="table_val_col", default=1,
                      help="csv column name|number which contains values to be set with attribute name of the graph (default=1, second column)", metavar="STRING|INT")
    parser.add_option("-q", "--quiet",
                      action="store_false", dest="verbose", default=True,
                      help="don't print status messages to stdout")

    # Run parser -------------------------------------------------------------
    (options, args) = parser.parse_args()

    print(options)
    if options.input_graph is None or options.input_csv is None:
        print("Error: input graph or csv file is missing")
        parser.parse_args(['-h'])
    # class O(object):pass
    # options=O()
    # options.input_csv='stats-values.csv';options.verbose=False;options.table_key_col=0;options.graph_key_attr='name';options.graph_val_attr='custom_num_val';options.output_graph='out.arg';options.input_graph='input_fold_graph.arg';options.table_val_col=1

    #  DO IT     -------------------------------------------------------------
    graphAddAttributeFromCsv(input_graph=options.input_graph,
                             output_graph=options.output_graph,
                             input_csv=options.input_csv,
                             table_key_col=options.table_key_col,
                             graph_key_attr=options.graph_key_attr,
                             graph_val_attr=options.graph_val_attr,
                             table_val_col=options.table_val_col,
                             verbose=options.verbose)
