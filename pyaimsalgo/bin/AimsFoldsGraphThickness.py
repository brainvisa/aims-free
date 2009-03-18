#!/usr/bin/env python

from soma import aimsalgo
from soma import aims
from soma.wip.aimsalgo import foldsgraphthickness
import numpy
import sys
from optparse import OptionParser

parser = OptionParser( description = 'Processing of Cortical thickness attributes in Cortical Folds Graph' )
parser.add_option( '-i', '--input', dest='graph_name', help='input graph' )
parser.add_option( '-c', '--cortex', dest='hemi_cortex_name', help='segmented cortex volume (LCR + Grey)' )
parser.add_option( '-g', '--gw', dest='GW_interface_name', help='segmented cortex volume (Grey, White, LCR)' )
parser.add_option( '-w', '--white', dest='white_mesh_name', help='white mesh' )
parser.add_option( '-l', '--lcr', dest='hemi_mesh_name', help='hemi mesh (Grey-LCR interface)' )

parser.add_option( '-o', '--output', dest='output_graph_name', help='output graph' )
parser.add_option( '-m', '--mid', dest='output_mid_interface_name', help='output mid_interface (optional)' )
parser.add_option( '-s', '--savelgw', dest='output_lcr_grey_white_name', help='output volume of lcr, grey and white matter for the use of computing thickness (optional)' )

(options, args) = parser.parse_args()

r = aims.Reader()
graph = r.read(options.graph_name)
white_mesh = r.read(options.white_mesh_name)
hemi_mesh = r.read(options.hemi_mesh_name)
vol_grey_white = r.read(options.GW_interface_name)
vol_cortex = r.read(options.hemi_cortex_name)

vol = aims.Volume_S16( vol_grey_white.getSizeX(), vol_grey_white.getSizeY(), 
                                        vol_grey_white.getSizeZ() )

vol.header()[ 'voxel_size' ] = vol_cortex.header()[ 'voxel_size' ]

dvolgw = vol_grey_white.arraydata()
dvolc = vol_cortex.arraydata()
dvol = vol.arraydata()
dvol.fill( 0 )
dvol[numpy.where(dvolc==255)] = 255 # LCR & GM = 255
dvol[numpy.where(dvolgw==100)] = 100 # GM = 100
dvol[numpy.where(dvolgw==200)] = 200 # WM = 200
fd = foldsgraphthickness.FoldsGraphThickness(graph, vol, white_mesh, hemi_mesh)
graph_out = fd.process()
w = aims.Writer()
w.write(graph_out, options.output_graph_name)
if options.output_mid_interface_name:
  mid=fd.mid_interface
  print "mid : " ,type(mid), mid.getSizeX()
  w.write(mid.get(), options.output_mid_interface_name)
  
if options.output_lcr_grey_white_name:
  w.write(vol, options.output_lcr_grey_white_name)
