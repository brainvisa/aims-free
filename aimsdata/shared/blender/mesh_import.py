#!BPY

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
"""
Name: 'BrainVISA mesh (.mesh)...'
Blender: 232
Group: 'Import'
Tooltip: 'Import BrainVISA mesh File Format (.mesh)'
"""

from __future__ import print_function

from __future__ import absolute_import
from six.moves import range
__author__ = "Yann Cointepas"
__url__ = ("BrainVISA project, http://brainvisa.info",)
__version__ = "1.0"

__bpydoc__ = """\
This script imports BrainVISA mesh File format files to Blender.

Usage:<br>
	Execute this script from the "File->Import" menu and choose a mesh file to
open.

Notes:<br>
	If the mesh file contains several time steps, only the first one is imported.
"""


import struct
import Blender
from Blender import NMesh
import sys


def readAndUnpack( format, file ):
  return struct.unpack( format, file.read( struct.calcsize( format ) ) )

class BinaryItemReader(object):
  def __init__( self, bigEndian=True ):
    if bigEndian:
      self._endianess = '>'
    else:
      self._endianess = '<'
    
  def read( self, format, file ):
    result = ()
    format = format.split( 's' )
    if format:
      if format[0]:
        result = readAndUnpack( self._endianess + format[0], file )
      else:
        result = ()
      for f in format[ 1: ]:
        size = readAndUnpack( self._endianess + 'L', file )[0]
        result = result + ( file.read( size ), )
        result = result + readAndUnpack( self._endianess + f, file )
    return result
        
class MeshReader(object):
  def __init__( self, fileName ):
    self._file = None
    self._file = open( fileName, 'rb' )
    if self._file.read( 5 ) == 'binar':
      self._itemReader = BinaryItemReader( self._file.read( 4 ) == 'ABCD' )
    else:
      raise RuntimeError( 'Ascii mesh format not implemented' )
    void, self._polygonDimension, timeStepCount = self._itemReader.read( 'sLL', self._file )
    if timeStepCount == 0:
      raise RuntimeError( 'No mesh in this file' )
    instant = self._itemReader.read( 'L', self._file )[0]

    self.verticesCount = self._itemReader.read( 'L', self._file )[0]
    self._verticesRead = False
    self.normalsCount = None
    self._normalsRead = False
    self.facesCount = None
    self._facesRead = False
  
  def vertices( self ):
    if self._verticesRead:
      raise RuntimeError( 'Vertices can be read only once' )
    for i in range( self.verticesCount ):
      yield self._itemReader.read( 'fff', self._file )
    self._verticesRead = True
    self.normalsCount = self._itemReader.read( 'L', self._file )[0]
    
  def normals( self ):
    if not self._verticesRead:
      if self.verticesCount == 0:
        self._verticesRead = True
      else:
        raise RuntimeError( 'Vertices must be read before normals' )
    if self._normalsRead:
      raise RuntimeError( 'Normals can be read only once' )
    for i in range( self.normalsCount ):
      yield self._itemReader.read( 'fff', self._file )
    self._normalsRead = True
    textureCount = self._itemReader.read( 'L', self._file )[0]
    if textureCount != 0:
      raise RuntimeError( 'Texture in mesh file not supported' )
    self.facesCount = self._itemReader.read( 'L', self._file )[0]

  def faces( self ):
    if not self._verticesRead:
      if self.verticesCount == 0:
        self._verticesRead = True
      else:
        raise RuntimeError( 'Vertices must be read before faces' )
    if not self._normalsRead:
      if self.normalsCount == 0:
        self._normalsRead = True
        textureCount = self._itemReader.read( 'L', self._file )[0]
        if textureCount != 0:
          raise RuntimeError( 'Texture in mesh file not supported' )
        self.facesCount = self._itemReader.read( 'L', self._file )[0]
      else:
        raise RuntimeError( 'Normals must be read before faces' )
    if self._facesRead:
      raise RuntimeError( 'Faces can be read only once' )
    format = 'L' * self._polygonDimension
    for i in range( self.facesCount ):
      yield self._itemReader.read( format, self._file )
    self._facesRead = True
    self._file.close()
    self._file = None

  def __del__( self ):
    if self._file is not None:
      self._file.close()
  
  
def read(filename):
  reader = MeshReader( filename )
  mesh = NMesh.New()
  print(reader.verticesCount, 'vertices')
  for x, y, z in reader.vertices():
    mesh.verts.append( NMesh.Vert( x, y, z ) )
  count = 0
  print(reader.normalsCount, 'normals')
  for x, y, z in reader.normals():
    mesh.verts[ count ].no[ 0 ] = x
    mesh.verts[ count ].no[ 1 ] = y
    mesh.verts[ count ].no[ 2 ] = z
    count += 1
  print(reader.facesCount, 'faces')
  for f in reader.faces():
    face = NMesh.Face( [mesh.verts[i] for i in f] )
    face.smooth = True
    mesh.faces.append( face )
  
  object = Blender.Object.New( 'Mesh' )  
  object.name = Blender.sys.splitext(Blender.sys.basename(filename))[0]
  Blender.Scene.getCurrent().link( object )
  object.link( mesh )
  mesh.update( reader.normalsCount == 0 )
  
  Blender.Window.DrawProgressBar(1.0, '')  # clear progressbar

def fs_callback(filename):
  read(filename)

Blender.Window.FileSelector(fs_callback, "Import Mesh")
