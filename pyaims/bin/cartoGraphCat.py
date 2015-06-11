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
from soma import aims
from optparse import OptionParser

parser = OptionParser(description='Graphs concatenation')
parser.add_option('-o', '--output', dest='output', help='output graph')
parser.add_option('-i', '--input', dest='filename', action='append',
                  help='input graph(s)')

(options, args) = parser.parse_args()

if args and not options.output:
    options.output = args[0]
    del args[0]
if options.filename is None:
    options.filename = []
options.filename += args


r = aims.Reader()
g1 = r.read(options.filename[0])

for i in xrange(1, len(options.filename)):
    g2 = r.read(options.filename[i])

    # remove bucket indices in g2
    # toremove = ( 'bucket_label', 'Tmtktri_label', 'roi_label', 'cluster_label',
    #'ss_label', 'bottom_label', 'other_label' )
    for v in g2.vertices():
        for x in filter(lambda x: x.endswith('_label'), v.keys()):
            del v[x]
    # copy subgraph g2 into g1
    g2.extract(g1, g2.vertices())
    # TODO: update aims_objects_table in g1

w = aims.Writer()
w.write(g1, options.output)
