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
from sys import stderr

from xml.sax import make_parser
import sys
from pprint import pprint

from xml.sax.xmlreader import XMLReader
from xml.sax.handler import ContentHandler
from base64 import b64decode
from struct import unpack
import operator, zlib
import soma.aims


#-------------------------------------------------------------------------------
class EmptyHandler( ContentHandler ):
  def __init__( self, parentHandler, xmlReader ):
    self.parentHandler = parentHandler
    self.xmlReader = xmlReader
    self.depth = 0
  
  
  def startElement( self, name, attrs ):
    self.depth += 1
  
  
  def endElement( self, name ):
    if self.depth:
      self.depth -= 1
    else:
      self.xmlReader.setContentHandler( self.parentHandler )
      self.xmlReader = None
      self.parentHandler = None


#-------------------------------------------------------------------------------
class GIFTIHandler( ContentHandler ):
  _dataTypes = {
    'NIFTI_TYPE_UINT8': ( int, 'B' ),
    'NIFTI_TYPE_INT32': ( int, 'i' ),
    'NIFTI_TYPE_FLOAT32': ( float, 'f' ),
  }
  
  class Item: pass
  
  
  def __init__( self, metaData, mesh, texture, xmlReader ):
    self.result = [ self._noneIfFalse(i) for i in ( metaData, mesh, texture ) ]
    self.xmlReader = xmlReader
  
  
  def _noneIfFalse( b ):
    if not b:
      return None
    return bool( b )
  _noneIfFalse = staticmethod( _noneIfFalse )
  
  
  def _noneIfTrue( b ):
    if b is True:
      return None
    return b
  _noneIfTrue = staticmethod( _noneIfTrue )
  
  
  def startElement( self, name, attrs ):
    if name == 'MetaData' :
      if self.result[ 0 ] is None:
        self.xmlReader.setContentHandler( EmptyHandler( self, self.xmlReader ) )
      else:
        self.result[ 0 ] = {}
        self.xmlReader.setContentHandler( MetaDataHandler( self.result[ 0 ], self, self.xmlReader ) )
    elif name == 'DataArray':
      dimensions = [ int( attrs.getValue( 'Dim' + str(i) ) ) \
                     for i in xrange( int( attrs.getValue( 'Dimensionality' ) ) ) ]
      category = attrs.getValue( 'Intent' )
#      print '!category!', category
#      print '!dimensions!', dimensions
      if category == 'NIFTI_INTENT_POINTSET':
        if self.result[ 1 ] is None:
          ignore = True
        else:
          if self.result[ 1 ] is True:
            self.result[ 1 ] = soma.aims.AimsTimeSurface_3()
          if len( dimensions ) != 2 or dimensions[ 1 ] != 3:
            raise RuntimeError( 'invalid dimension for %(category)s: %(dim)s' % {
                                  'category': category, 'dim': str( dimensions ) } )
          self.feedArray = self.feedCoordinates
          self.noMoreFeedArray = self.noMoreFeedCoordinatesNormalsOrTriangles
          ignore = False
      elif category == 'NIFTI_INTENT_VECTOR':
        if self.result[ 1 ] is None:
          ignore = True
        else:
          if self.result[ 1 ] is True:
            self.result[ 1 ] = soma.aims.AimsTimeSurface_3()
          if len( dimensions ) != 2 or dimensions[ 1 ] != 3:
            raise RuntimeError( 'invalid dimension for %(category)s: %(dim)s' % {
                                  'category': category, 'dim': str( dimensions ) } )
          self.feedArray = self.feedNormals
          self.noMoreFeedArray = self.noMoreFeedCoordinatesNormalsOrTriangles
          ignore = False
      elif category == 'NIFTI_INTENT_TRIANGLE':
        if self.result[ 1 ] is None:
          ignore = True
        else:
          if self.result[ 1 ] is True:
            self.result[ 1 ] = soma.aims.AimsTimeSurface_3()
          if len( dimensions ) != 2 or dimensions[ 1 ] != 3:
            raise RuntimeError( 'invalid dimension for %(category)s: %(dim)s' % {
                                  'category': category, 'dim': str( dimensions ) } )
          self.feedArray = self.feedTriangles
          self.noMoreFeedArray = self.noMoreFeedCoordinatesNormalsOrTriangles
          ignore = False
      elif (category == 'NIFTI_INTENT_TIME_SERIES' or category == 'NIFTI_INTENT_TTEST' or category== 'NIFTI_INTENT_SHAPE' or category == 'NIFTI_INTENT_NONE' or category == 'NIFTI_INTENT_LABEL'):
        if self.result[ 2 ] is None:
          ignore = True
        else:
          if self.result[ 2 ] is True:
            self.result[ 2 ] = soma.aims.TimeTexture_FLOAT()
          if ((len( dimensions ) != 1) and ( len( dimensions ) != 2 or dimensions[ 1 ] != 1)):
            raise RuntimeError( 'invalid dimension for %(category)s: %(dim)s' % {
                                  'category': category, 'dim': str( dimensions ) } )
          # Adds a new texture
          newTexture = self.result[ 2 ][ self.result[ 2 ].size() ]
          self.feedArray = self.feedTexture
          self.noMoreFeedArray = self.noMoreFeedTexture
          ignore = False
      else:
        print >> stderr, 'WARNING: Ignoring DataArray with "' + category + '" category'
        ignore = True
      if ignore:
        self.xmlReader.setContentHandler( EmptyHandler( self, self.xmlReader ) )
      else:
        dataLocation = attrs.getValue( 'ExternalFileName' )
        if dataLocation != '':
          raise RuntimeError( 'External data location "%s" not supported' % dataLocation )
        encoding = attrs.getValue( 'Encoding' )
        dataType = attrs.getValue( 'DataType' )
        if self._dataTypes.has_key( dataType ):
          #print >> stderr, 'Data type is ' + dataType
          asciiConverter, unpackString = self._dataTypes[ dataType ]
        else:
          raise RuntimeError( 'data type "%s" not supported' % dataType )
        count = str( reduce( operator.mul, dimensions ) )
        if attrs.getValue( 'Endian' ) == 'BigEndian':
          unpackString = '>' + count + unpackString
        else:
          unpackString = '<' + count + unpackString
        if encoding == 'ASCII':
          self.xmlReader.setContentHandler( AsciiDataArrayHandler( asciiConverter, self, self.xmlReader ) )
        elif encoding == 'Base64Binary':
          self.xmlReader.setContentHandler( Base64DataArrayHandler( unpackString, asciiConverter, self, self.xmlReader ) )
        elif encoding == 'GZipBase64Binary':
          self.xmlReader.setContentHandler( GZipBase64DataArrayHandler( unpackString, asciiConverter, self, self.xmlReader ) )
        else:
          raise RuntimeError( 'Encoding "%s" not supported' % encoding )


  def feedCoordinates( self, values ):
    vertices = self.result[ 1 ].vertex()
    i = 0
    while i < len( values ):
      vertices.append( values[i:i+3] )
      i += 3
  
  
  def feedNormals( self, values ):
    normals = self.result[ 1 ].normal()
    i = 0
    while i < len( values ):
      normals.append( values[i:i+3] )
      i += 3
  
  
  def feedTriangles( self, values ):
    triangles = self.result[ 1 ].polygon()
    i = 0
    while i < len( values ):
      triangles.append( values[i:i+3] )
      i += 3
      
      
  def noMoreFeedCoordinatesNormalsOrTriangles( self, metaData ):
    header = self.result[ 1 ].header()
    for k, v in metaData.iteritems():
      header[ str( k ) ] = str( v )
  
  
  def feedTexture( self, values ):
    pass
    lastTexture = self.result[ 2 ][ self.result[ 2 ].size() - 1 ]
    for v in values:
      lastTexture.push_back( v )
      
      
  def noMoreFeedTexture( self, metaData ):
    pass
    header = self.result[ 2 ].header()
    for k, v in metaData.iteritems():
      header[ str( k ) ] = str( v )
  
  
#-------------------------------------------------------------------------------
class MetaDataHandler( ContentHandler ):
  def __init__( self, result, parentHandler, xmlReader ):
    self.parentHandler = parentHandler
    self.xmlReader = xmlReader
    self.__key = None
    self.dict = result
    self.__characters = None

  def startElement( self, name, attrs ):
    if name in ( 'Name', 'Value' ):
      self.__characters = []
  
  def endElement( self, name ):
    if name == 'Name':
      self.__key = ''.join( self.__characters )
      self.__characters = None
    elif name == 'Value':
      self.dict[ self.__key ] = ''.join( self.__characters )
      self.__characters = None
    elif name == 'MetaData':
      self.xmlReader.setContentHandler( self.parentHandler )
      self.xmlReader = None
      self.parentHandler = None
      
  
  def characters( self, content ):
    if self.__characters is not None:
      self.__characters.append( content )


#-------------------------------------------------------------------------------
class AsciiDataArrayHandler( ContentHandler ):
  def __init__( self, dataConverter, giftiHandler, xmlReader ):
    self.giftiHandler = giftiHandler
    self.xmlReader = xmlReader
    self.metaData = {}
    self.__dataConverter = dataConverter
    self.__characters = None
  
  
  def startElement( self, name, attrs ):
    if name == 'MetaData':
      if self.metaData is None:
        self.xmlReader.setContentHandler( EmptyHandler( self, self.xmlReader ) )
      else:
        self.xmlReader.setContentHandler( MetaDataHandler( self.metaData, self, self.xmlReader ) )
    elif name == 'Data':
      self.__characters = []
    else:
      self.xmlReader.setContentHandler( EmptyHandler( self, self.xmlReader ) )
  
  
  def endElement( self, name ):
    if name == 'Data':
      values = self.getValues( ''.join( self.__characters ) )
      self.__characters = None
      self.giftiHandler.feedArray( values )
    elif name == 'DataArray':
      self.giftiHandler.noMoreFeedArray( self.metaData )
      self.xmlReader.setContentHandler( self.giftiHandler )
      self.xmlReader = None
      self.giftiHandler = None
  
  
  def characters( self, content ):
    if self.__characters is not None:
      self.__characters.append( content )


  def getValues( self, text ):
    return [self.__dataConverter(i) for i in text.split()]
  
  
#-------------------------------------------------------------------------------
class Base64DataArrayHandler( AsciiDataArrayHandler ):
  def __init__( self, unpackString, dataConverter, giftiHandler, xmlReader ):
    AsciiDataArrayHandler.__init__( self, dataConverter, giftiHandler, xmlReader )
    self.unpackString = unpackString
  
  
  def getValues( self, text ):
    return unpack( self.unpackString, b64decode( text ) )



#-------------------------------------------------------------------------------
class GZipBase64DataArrayHandler( Base64DataArrayHandler ):
  def getValues( self, text ):
    return unpack( self.unpackString, zlib.decompress( b64decode( text ) ) )


#-------------------------------------------------------------------------------
def readGifti( giftiFile, metaData=True, mesh=True, texture=True ):
  parser = make_parser()
  handler = GIFTIHandler( metaData, mesh, texture, parser )
  parser.setContentHandler( handler )
  parser.parse( giftiFile )
  #count = 1
  #for line in open( sys.argv[1] ):
    #if count % 1000 == 1:
      #print '!line!', count
    #count += 1
    #parser.feed( line )
  return [handler._noneIfTrue(i) for i in handler.result]


#-------------------------------------------------------------------------------
def startConvert( fileIn, fileOut):
  #from xml.sax import make_parser
  #import sys
  #from pprint import pprint
  

  print '-' * 70
  print fileIn
  print '-' * 70
  metaData, mesh, texture = readGifti( fileIn )
  sys.stdout.write( 'meta data: ' )
  pprint( metaData, indent=2 )
  sys.stdout.write( 'mesh: ' )
  if mesh is None:
      print None
  else:
      print mesh.vertex().size(), 'vertices,', mesh.normal().size(), 'normals,', mesh.polygon().size(), 'polygons'
      soma.aims.Writer().write( mesh, fileOut + '.mesh' )
  sys.stdout.write( 'texture: ' )
  if texture is None:
      print None
  else:
      print texture.size(), 'texture(s) with', texture[0].size(), 'elements'
      soma.aims.Writer().write( texture, fileOut + '.tex' )

