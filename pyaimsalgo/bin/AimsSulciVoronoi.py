#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

from __future__ import absolute_import
import sys, os
from soma import aims, aimsalgo
import numpy
import optparse

parser = optparse.OptionParser( description='Voronoi diagram of the sulci ' \
  'nodes regions, in the grey matter, and extending to the whole 3D space' )
parser.add_option( '-g', '--greywhite', dest='lgw',
  help='left grey/white mask' )
parser.add_option( '-o', '--output', dest='voronoi',
  help='output voronoi diagram volume' )
parser.add_option( '-f', '--folds', dest='graph',
  help='sulci graph file' )

options, args = parser.parse_args()

lgw_vol_file = options.lgw
fold_graph_file = options.graph
voronoi_vol_file = options.voronoi

if lgw_vol_file is None and len( args ) > 0:
  lgw_vol_file = args[0]
  del args[0]
if voronoi_vol_file is None and len( args ) > 0:
  voronoi_vol_file = args[0]
  del args[0]
if fold_graph_file is None and len( args ) > 0:
  fold_graph_file = args[0]
  del args[0]
if lgw_vol_file is None or voronoi_vol_file is None \
  or fold_graph_file is None or len( args ) != 0:
  parser.parse( [ '-h' ] )

lgw_vol = aims.read( lgw_vol_file )
fold_graph = aims.read( fold_graph_file )
LCR_label = 255
GM_label = 100

seed = - lgw_vol
voxel_size = lgw_vol.header()["voxel_size"]

def printbucket( bck, vol, value ):
  c = aims.RawConverter_BucketMap_VOID_rc_ptr_Volume_S16( False, True, value )
  c.printToVolume( bck._get(), vol )

seed_label_list = []
for v in fold_graph.vertices():
  try:
    b = v[ 'aims_ss' ]
    index = v[ 'skeleton_label' ]
    seed_label_list.append(int(index))
    printbucket( b, seed, index )
    printbucket( b, lgw_vol, LCR_label ) #pour que le lcr rentre jusqu au fond des silons
    try:
      b = v[ 'aims_bottom' ]
      printbucket( b, seed, index )
    except:
      pass
    try:
      b = v[ 'aims_other' ]
      printbucket( b, seed, index )
    except:
      pass
  except:
    pass

f1 = aims.FastMarching()
print("Voronoi in Grey matter")
f1.doit(seed, [-LCR_label, -GM_label], seed_label_list)
voronoi_vol = f1.voronoiVol()
print("Voronoi in White matter")
f1 = aims.FastMarching()
n = numpy.array( voronoi_vol, copy=False )
n[ n == -1 ] = -100
f1.doit( voronoi_vol, [-100], seed_label_list )
# f1.doit( voronoi_vol, [-100], [ 940, 760] )
voronoi_vol = f1.voronoiVol()

aims.write( voronoi_vol, voronoi_vol_file )

