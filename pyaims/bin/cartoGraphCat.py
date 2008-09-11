#!/usr/bin/env python

from soma import aims
from optparse import OptionParser

parser = OptionParser( description = 'Graphs concatenation' )
parser.add_option( '-o', '--output', dest='output', help='output graph' )
parser.add_option( '-i', '--input', dest='filename', action='append',
                   help='input graph(s)' )

(options, args) = parser.parse_args()

if args and not options.output:
    options.output = args[0]
    del args[0]
if options.filename is None:
    options.filename = []
options.filename += args


r = aims.Reader()
g1 = r.read( options.filename[0] )

for i in xrange( 1, len( options.filename ) ):
  g2 = r.read( options.filename[i] )

  # remove bucket indices in g2
  #toremove = ( 'bucket_label', 'Tmtktri_label', 'roi_label', 'cluster_label',
    #'ss_label', 'bottom_label', 'other_label' )
  for v in g2.vertices():
    for x in filter( lambda x: x.endswith( '_label' ), v.keys() ):
      del v[ x ]
  # copy subgraph g2 into g1
  g2.extract( g1, g2.vertices() )
  # TODO: update aims_objects_table in g1

w = aims.Writer()
w.write( g1, options.output )

