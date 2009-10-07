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
Name: 'BrainVISA bundles (.bundles)...'
Blender: 237
Group: 'Import'
Tooltip: 'Import BrainVISA bundles File Format (.bundles)'
"""

__author__ = "Yann Cointepas"
__url__ = ("BrainVISA project, http://brainvisa.info",)
__version__ = "1.0"

__bpydoc__ = """\
This script imports BrainVISA curves bundles files to Blender curves.

Usage:<br>
  Execute this script from the "File->Import" menu and choose a bundles file to
open.
"""


import struct
import Blender
from Blender import Curve, Object, Scene

class BundlesReader:
  class CurveIterator:
    def __init__( self, name, dataFile, count ):
      self.name = name
      self._dataFile = dataFile
      self._count = count
      self._read = 0
    
    def __len__( self ):
      return int( self._count - self._read )
    
    def __iter__( self ):
      size = struct.calcsize( 'L' )
      for i in xrange( len( self ) ):
        self._read += 1
        count = struct.unpack( 'L', self._dataFile.read( size ) )[0]
        yield BundlesReader.PointIterator( self._dataFile, count )
  
    def next( self ):
      if self._read < self._count:
        self._read += 1
        size = struct.calcsize( 'L' )
        count = struct.unpack( 'L', self._dataFile.read( size ) )[0]
        return BundlesReader.PointIterator( self._dataFile, count )
      raise StopIteration
        
  class PointIterator:
    def __init__( self, dataFile, count ):
      self._dataFile = dataFile
      self._count = count
      self._read = 0

    def __len__( self ):
      return int( self._count - self._read )
  
    def __iter__( self ):
      size = struct.calcsize( 'fff' )
      for i in xrange( len( self ) ):
        self._read += 1
        yield struct.unpack( 'fff', self._dataFile.read( size ) )

    def next( self ):
      if self._read < self._count:
        self._read += 1
        size = struct.calcsize( 'fff' )
        return struct.unpack( 'fff', self._dataFile.read( size ) )
      raise StopIteration

  def __init__( self, fileName ):
      attributes = {}
      execfile( fileName, attributes, attributes )
      attributes = attributes[ 'attributes' ]
      if not attributes[ 'binary' ]:
        raise RuntimeError( 'Ascii bundle reading not implemented' )
      b = attributes[ 'bundles' ]
      bundles = []
      if b:
        i = 0
        while i < len( b )-2:
          n = b[ i + 3 ] - b[ i + 1 ]
          bundles.append( ( b[ i ], n ) )
          i += 2
        bundles.append( ( b[-2], attributes[ 'curves_count' ] - b[-1] ) )
      self.bundles = bundles
      self.dataFile = open( fileName + 'data', 'rb' )
  
  def __del__( self ):
    if self.dataFile:
      self.dataFile.close()
  
  def __len__( self ):
    return len( self.bundles )
      
  def __iter__( self ):
    for name, count in self.bundles:
      yield BundlesReader.CurveIterator( name, self.dataFile, count )
  
  
def read(filename):
#  proximityThreshold = 50
  for bundle in BundlesReader( filename ):
    c = Curve.New()             # create new  curve data
    cur = Scene.getCurrent()    # get current scene
    ob = Object.New( 'Curve' )    # make curve object
    ob.name = bundle.name
    ob.link(c)                  # link curve data with this object
    cur.link(ob)                # link object into scene

    c.setExt1( 0.1 )
#    c.setExt2( 0.06 )

    lastCurve = -1
    for curve in bundle:
      lastCurve += 1
      if len( curve ) > 1:
        lastPoint = curve.next()
        nurb = c.appendNurb( lastPoint + ( 0, ) )
        nurb.setType( 0 )
        for point in curve:
#          if max( [abs(x-y) for x,y in zip(lastPoint,point)] ) > proximityThreshold:
          c.appendPoint( lastCurve, point + ( 0, ) )
  
  Blender.Window.DrawProgressBar(1.0, '')  # clear progressbar

def fs_callback(filename):
  read(filename)

Blender.Window.FileSelector(fs_callback, "Import Bundles")
