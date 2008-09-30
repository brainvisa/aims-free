#!/usr/bin/env python

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
print 'created', nv, 'new nodes'

aims.write(graph, options.output_graph_name)

