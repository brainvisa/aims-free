#!BPY

# Copyright IFR 49 (1995-2009)
#
#  This software and supporting documentation were developed by
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
"""
Name: 'BrainVISA mesh (.mesh)...'
Blender: 232
Group: 'Export'
Tooltip: 'Export selected mesh to BrainVISA mesh Format (.mesh)'
"""

__author__ = "Yann Cointepas"
__url__ = ("BrainVISA project, http://brainvisa.info",)
__version__ = "1.0"

__bpydoc__ = """\
This script exports meshes to BrainVISA mesh file format.

Usage:<br>
	Select meshes to be exported and run this script from "File->Export" menu.
"""


import Blender
import sys
from struct import pack

def packString( s ):
  return pack( '>L', len( s ) ) + s
  

def write(filename):
  file = open( filename, 'wb' )

  objects = Blender.Object.GetSelected()
  objname = objects[0].name
  meshname = objects[0].data.name
  mesh = Blender.NMesh.GetRaw(meshname)
  obj = Blender.Object.Get(objname)

  # Get polygon dimension
  polygonDimension = len( mesh.faces[0].v )
  
  file.write( 'binarABCD' ) # big-endian binary format
  file.write( packString( 'VOID' ) )
  file.write( pack( '>L', polygonDimension ) )
  file.write( pack( '>L', 1 ) ) # One time step
  
  # The single time step
  file.write( pack( '>L', 0 ) ) # instant
  # write vertices
  file.write( pack( '>L', len( mesh.verts ) ) )
  for v in mesh.verts:
    file.write( ''.join( [pack('>f',i) for i in v.co] ))
  # write normals
  file.write( pack( '>L', len( mesh.verts ) ) )
  for v in mesh.verts:
    file.write( ''.join( [pack('>f',i) for i in v.no] ))
  # No texture allowed
  file.write( pack( '>L', 0 ) )
  # Write polygons
  file.write( pack( '>L', len( mesh.faces ) ) )
  for f in mesh.faces:
    if len( f.v ) != polygonDimension:
      raise RuntimeError( 'BrainVISA mesh format does not support meshes with several polygon dimensions (e.g. triangles and squares)' )
    file.write( ''.join( [pack('>L',v.index) for v in f.v] ) )
  file.close()

  Blender.Window.DrawProgressBar(1.0, '')  # clear progressbar

def fs_callback(filename):
	# if not filename.endswith('.mesh'): filename += '.mesh'
	write(filename)

Blender.Window.FileSelector(fs_callback, "Export BrainVISA mesh")
