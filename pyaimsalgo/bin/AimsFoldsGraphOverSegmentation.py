#!/usr/bin/env python2

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
from soma import aimsalgo
from soma import aims
import sys
from optparse import OptionParser

parser = OptionParser( description = 'Split fold nodes that are too big into '
  'smaller pieces' )
parser.add_option( '-i', '--input', dest='graph_name', help='input graph' )
parser.add_option( '-o', '--output', dest='output_graph_name',
  help='output graph' )
parser.add_option( '-l', '--length', dest='length', type='float',
  help='target length of split pieces [default: 20mm]' )
parser.add_option( '-m', '--minsize', dest='minsize', type='int',
  help='minimum size (in voxels) of split pieces [default: 50]' )

parser.set_defaults( length = 20, minsize = 50 )
(options, args) = parser.parse_args()

graph = aims.read(options.graph_name)
fov = aims.FoldArgOverSegment( graph )
nv = fov.subdivizeGraph( options.length, options.minsize )
print('created', nv, 'new nodes')

aims.write(graph, options.output_graph_name)

