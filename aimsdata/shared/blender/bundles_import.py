#!BPY

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
