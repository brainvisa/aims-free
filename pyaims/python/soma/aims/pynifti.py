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

'''This class fakes the pynifti nifti.NiftiImage class using an underlying aims
volume and aims IO'''

import types
from soma import aims
import numpy

class NiftiImage( object ):
  '''This class fakes the API of nifti.NiftiImage from pynifti, but using
  an underlying AIMS volume and AIMS IO. It can be built on top of a
  soma.aims.Volume_* and behaves the way pynifti does.

  Important exceptions are the aim of this class:

  1. the array data is in Aims orientation, not in Nifti (disk)
     orientation.
     * That's the main improvement over pynifti. *
  2. data are already scaled: NiftiImage.data and
     NiftiImage.getScaledData() return the same thing. But who needs
     unscaled data ?
  3. the header is a proxy to the Aims header with overloads to show /
     convert on-the-fly pynifti-specific ones and hide aims-specific ones.
     It is be used transparently.
  4. The load/save functions are aims IO so work with any format
     handled by aims, not only for Nifti/Analyze.
  '''

  class _FakeHeader( object ):
    _forbiddenkeys = ( 'volume_dimension', 'voxel_size' )

    class FakeHeaderAttributes( object ):
      def __init__( self, fakeheader ):
        self._header = fakeheader

      def _attributes( self ):
        return filter( lambda x: not x.startswith( '_' ), dir( self ) )

      def dim( self ):
        vd = self._header._header[ 'volume_dimension' ]
        dim = [ len( vd ), 1, 1, 1, 1, 1, 1, 1 ]
        dim[ 1:len(vd) ] = list(vd)
        return dim

      def _set_dim( self, value ):
       raise ValueError( 'this attribute cannot be set' )

      def pixdim( self ):
        try:
          pd = self._header._header[ 'voxel_size' ]
          pixdim = [ 0., 1., 1., 1., 1., 1., 1., 1. ]
          # TODO: reshape in disk ref, not in memory ref
          pixdim[ 1:len(pd) ] = list( pd )
          return pixdim
        except:
          return [ 1., 1., 1. ]

      def _set_pixdim( self, value ):
       raise ValueError( 'this attribute cannot be set' )

      def scl_slope( self ):
        try:
          return self._header._header[ 'scale_factor' ]
        except:
          return 1.

      def _set_scl_slope( self, value ):
        self._header._header[ 'scale_factor' ] = value

      def scl_inter( self ):
        try:
          return self._header._header[ 'scale_offset' ]
        except:
          return 0.

      def _set_scl_inter( self, value ):
        self._header._header[ 'scale_offset' ] = value

      def _getStorageToMemory( self ):
        try:
          return aims.Motion( self._header._header[ 'storage_to_memory' ] )
        except:
          # return "standard" orientation
          mot = aims.Motion()
          mot.rotation().setValue( -1, 1, 1 )
          mot.rotation().setValue( -1, 2, 2 )
          vs = self[ 'pixdim' ]
          mot.translation()[1] = vs[1]-1
          mot.translation()[2] = vs[2]-1
          return mot

      def _set_storage_to_memory( self, value ):
        if not isinstance( value, aims.Motion ):
          value = aims.Motion( value )
        self._header._header[ 'storage_to_memory' ] = value.toVector()

      def _getMemoryToStorage( self ):
        return self._getStorageToMemory().inverse()

      def _setMemoryToStorage( self, value ):
        if not isinstance( value, aims.Motion ):
          value = aims.Motion( value )
        self._header._header[ 'storage_to_memory' ] \
          = value.inverse().toVector()

      def _getVoxelSizeMotion_aims( self ):
        mot = aims.Motion()
        try:
          vs = self._header._header[ 'voxel_size' ]
          if len( vs ) > 0:
            mot.rotation().setValue( vs[0], 0, 0, )
            if len( vs ) > 1:
              mot.rotation().setValue( vs[1], 1, 1, )
              if len( vs ) > 2:
                mot.rotation().setValue( vs[2], 2, 2, )
        except:
          pass
        return mot

      def _getVoxelSizeMotion_storage( self ):
        vsm = self[ 'pixdim' ][ 1:4 ]
        m2s = self._getMemorytoStorage()
        vs2 = m2s.transform( vsm ) - m2s.transform( [ 0, 0, 0, ] )
        vss = aims.Motion()
        vss.rotation().setValue( abs( vs2[0] ), 0, 0 )
        vss.rotation().setValue( abs( vs2[1] ), 1, 1 )
        vss.rotation().setValue( abs( vs2[2] ), 2, 2 )
        return vss

      def _getTransformation( self, num, failIfNotFound=False ):
        try:
          trs = self._header._header[ 'transformations' ]
          if num < 0:
            num = len( trs ) - num - 1
          if len( trs ) > num:
            return aims.Motion( trs[num] )
        except:
          pass
        if failIfNotFound:
          raise KeyError( 'transformation ' + str( num ) + ' is not found' )
        return aims.Motion()

      def _getReferential( self, num, failIfNotFound=False ):
        try:
          refs = self._header._header[ 'referentials' ]
          if num < 0:
            num = len( refs ) - num - 1
          if len( refs ) > num:
            return refs[ num ]
        except:
          pass
        if failIfNotFound:
          raise KeyError( 'referential ' + str( num ) + ' is not found' )
        return 'unknown'

      def _getReferentialCode( self, num ):
        return self._form_code( self._getReferential( num ) )

      def sform( self ):
        try:
          trs = self._header._header[ 'transformations' ]
          if len( trs ) > 0:
            mot = aims.Motion( trs[len(trs)-1] )
            s2m = self._getStorageToMemory()
            vs = aims.Motion()
            vvs = self._header._header[ 'voxel_size' ]
            vs.rotation().setValue( vvs[0], 0, 0 )
            vs.rotation().setValue( vvs[1], 1, 1 )
            vs.rotation().setValue( vvs[2], 2, 2 )
            s2q = mot * vs * s2m
            mat = numpy.identity( 4 )
            mat[ 0:3, 0:3 ] \
              = numpy.array( s2q.rotation().volume() ).reshape( 3,3 )
            mat[ 0:3, 3 ] = s2q.translation()
            return mat
        except:
          pass
        return None

      def _set_sform( self, value ):
        if not isinstance( value, aims.Motion ):
          value = aims.Motion( value )
        s2m = self._getStorageToMemory()
        vsm = self._getVoxelSizeMotion_aims()
        mot = value * ( vsm * s2m ).inverse()
        num = 0
        try:
          trs = self._header._header[ 'transformations' ]
          if len( trs ) > 1:
            num = len( trs ) - 1
          elif len( trs ) == 0:
            num = 0
          else:
            if self.qform_code() == 0:
              num = 0
            else:
              num = 1
        except:
          trs = []
          self._header._header[ 'transformations' ] = trs
        if len( trs ) <= num:
          trs.append( mot.toVector() )
        else:
          trs[ num ] = mot.toVector()
        try:
          refs = self._header._header[ 'referentials' ]
        except:
          refs = []
          self._header._header[ 'referentials' ] = refs
        if len( refs ) <= num:
          refs.append(
            'Coordinates aligned to another file or to anatomical truth' )

      def sform_code( self ):
        try:
          sform = self.sform()
          if sform is None:
            return 0
          refs = self._header._header[ 'referentials' ]
          if len( refs ) > 0:
            ref = refs[ len( refs ) - 1 ]
            return self._form_code( ref )
        except:
          pass
        return 0

      def _set_sform_code( self, value ):
        raise ValueError( 'this attribute cannot be set directly yet' )

      def _qform( self ):
        try:
          trs = self._header._header[ 'transformations' ]
          if len( trs ) > 0:
            mot = aims.Motion( trs[0] )
            s2m = aims.Motion( self._header._header[ 'storage_to_memory' ] )
            vs = aims.Motion()
            vvs = self._header._header[ 'voxel_size' ]
            vs.rotation().setValue( vvs[0], 0, 0 )
            vs.rotation().setValue( vvs[1], 1, 1 )
            vs.rotation().setValue( vvs[2], 2, 2 )
            s2q = mot * vs * s2m
            # verify it is a quaternion
            s2qv = mot * s2m
            s2qv.setTranslation( [ 0, 0, 0 ] )
            q = aims.Quaternion()
            q.buildFromMotion( s2qv )
            if aims.Motion( q ) != s2qv:
              m = aims.Motion()
              m.rotation().setValue( -1, 2, 2 ) # invert Z
              s2qv *= m
              q.buildFromMotion( s2qv )
              if aims.Motion( q ) != s2qv:
                return None # not a quaternion
              else:
                self._qfac = -1
            else:
              self._qfac = 1
            mat = numpy.identity( 4 )
            mat[ 0:3, 0:3 ] \
              = numpy.array( s2q.rotation().volume() ).reshape( 3,3 )
            mat[ 0:3, 3 ] = s2q.translation()
            return mat
        except:
          pass
        return None

      def _set_qform( self, value ):
       raise ValueError( 'this attribute cannot be set directly yet' )

      def _form_code( self, ref ):
        if ref == 'Scanner-based anatomical coordinates':
          return 1
        elif ref == \
          'Coordinates aligned to another file or to anatomical truth':
          return 2
        elif ref == 'Talairach-AC/PC-Anatomist':
          return 3
        elif ref == 'Talairach-MNI template-SPM':
          return 4
        return 0

      def qform_code( self ):
        try:
          qform = self._qform()
          if qform is None:
            return 0
          refs = self._header._header[ 'referentials' ]
          if len( refs ) > 0:
            ref = refs[0]
            return self._form_code( ref )
        except:
          pass
        return 0

      def quatern( self ):
        qform = self._qform()
        if qform is not None:
          qfac = self._qfac
          mot = aims.Motion( qform.reshape( 16 ) )
          vs = aims.Motion()
          vvs = self._header._header[ 'voxel_size' ]
          vs.rotation().setValue( 1. / vvs[0], 0, 0 )
          vs.rotation().setValue( 1. / vvs[1], 1, 1 )
          vs.rotation().setValue( 1. / vvs[2], 2, 2 )
          mot *= vs
          if qfac < 0:
            m = aims.Motion()
            m.rotation().setValue( -1, 2, 2 )
            mot *= m
          q = aims.Quaternion()
          q.buildFromMotion( mot )
          return q.vector()[:3]
        return [ 0., 0., 0. ]

      def _set_quater( self, value ):
        raise ValueError( 'this attribute cannot be set directly yet' )

      def qfac( self ):
        if self._qform() is not None:
          return self._qfac
        return 0

      def _set_qfac( self, value ):
        raise ValueError( 'this attribute cannot be set directly yet' )

      def qoffset( self ):
        qform = self._qform()
        if qform is None:
          return [ 0., 0., 0. ]
        return list( qform[ 0:3, 3 ] )

      def _set_qoffset( self, value ):
        raise ValueError( 'this attribute cannot be set directly yet' )

      # end of FakeHeaderAttributes

    class FakeHeaderIter( object ):
      def __init__( self, fakeheader, iteritems=False ):
        self._fakeheader = fakeheader
        try:
          self._iter = fakeheader._header.iteritems()
          self._iteritemused = True
        except:
          self._iter = fakeheader._header.__iter__()
          self._iteritemused = False
        self._index = -1
        self._iteritems = iteritems
      def __iter__( self ):
        return self
      def next( self ):
        if self._index < 0:
          try:
            if self._iteritemused:
              x, y = self._iter.next()
            else:
              x = self._iter.next()
            while x in self._fakeheader._forbiddenkeys:
              if self._iteritemused:
                x, y = self._iter.next()
              else:
                x = self._iter.next()
            if self._iteritems:
              if self._iteritemused:
                return ( x, y )
              else:
                return ( x, self._fakeheader[x] )
            else:
              return x
          except:
            self._index = 0
        if self._index < len( self._fakeheader._fakekeys ):
          x = self._fakeheader._fakekeys[ self._index ]
          self._index += 1
          if self._iteritems:
            return ( x, self._fakeheader[ x ] )
          else:
            return x
        else:
          raise StopIteration( 'iterator outside bounds' )

      # end of FakeHeaderIter

    # _FakeHeader methods

    def __init__( self, header ):
      self._header = header

    def __getitem__( self, item ):
      if not item.startswith( '_' ):
        px = self.FakeHeaderAttributes( self )
        a = getattr( px, item, None )
        if a:
          return a()
      return self._header[ item ]

    def __setitem__( self, item, value ):
      if not item.startswith( '_' ):
        px = self.FakeHeaderAttributes( self )
        a = getattr( px, '_set_' + item, None )
        if a:
          a( value )
      else:
        self._header[ item ] = value

    def keys( self ):
      k = list( self._header.keys() )
      for x in self._forbiddenkeys:
        k.remove( x )
      k += self.FakeHeaderAttributes( self )._attributes()
      return tuple( k )

    def items( self ):
      it = []
      for key, value in  self._header.items():
        if key not in self._forbiddenkeys:
          it.append( ( key, value ) )
      fk = self.FakeHeaderAttributes( self )
      for key in fk._attributes():
        it.append( self[ key ] )
      return tuple( it )

    def __iter__( self ):
      return self.FakeHeaderIter( self )

    def iteritems( self ):
      return self.FakeHeaderIter( self, True )

    def update( self, d ):
      for x, y in d.iteritems:
        self[ x ] = y

    def __repr__( self ):
      return repr( dict( self ) )

    def __str__( self ):
      return str( dict( self ) )

    # end of _FakeHeader

  class _ProxyAttributes( object ):
    def __init__( self, image ):
      self._image = image

    def voxdim( self ):
      return tuple( self._image._aims.header()[ 'voxel_size' ] )

    def pixdim( self ):
      return tuple( self._image.header[ 'pixdim' ][1:] )

    def volextent( self ):
      return tuple( self._image._aims.header()[ 'volume_dimension' ] )

    def nvox( self ):
      dim = self._image._aims.header()[ 'volume_dimension' ]
      if len( dim ) >= 4:
        d = dim[3]
      else:
        d = 1
      if len( dim ) >= 3:
        d *= dim[2]
      if len( dim ) >= 2:
        d *= dim[1]
      return d * dim[0]

    def bbox( self ):
      bb = []
      vd = self._image._aims.header()[ 'volume_dimension' ]
      n = len( vd )
      for i in xrange( n ):
        bb.append( ( 0, vd[n-i-1] - 1 ) )
      return tuple( bb )

    def description( self ):
      return self._image._aims.header()[ 'descrip' ]

    def _set_description( self, val ):
      self._image._aims.header()[ 'descrip' ] = val

    def extent( self ):
      return self.volextent()

    def timepoints( self ):
      return self._image._aims.getSizeT()

    def max( self ):
      dt = type( self._image._aims ).__name__[ 7: ]
      return getattr( aims, 'AimsData_' + dt )( self._image._aims ).maximum()

    def min( self ):
      dt = type( self._image._aims ).__name__[ 7: ]
      return getattr( aims, 'AimsData_' + dt )( self._image._aims ).minimum()

    def rtime( self ):
      try:
        vs = self._image._aims.header()[ 'voxel_size' ]
        if len( vs ) >= 4:
          return vs[3]
      except:
        pass
      return 1.

    def _set_rtime( self, value ):
      try:
        vs = self._image._aims.header()[ 'voxel_size' ]
        if len( vs ) >= 4:
          vs[3] = value
        else:
          while len( vs ) < 3:
            vs.append( 1. )
          vs.append( value )
      except:
        self._image._aims.header()[ 'voxel_size' ] = [ 1., 1., 1., value ]

    def slope( self ):
      try:
        s = self._image._aims.header()[ 'scale_factor' ]
        return s
      except:
        return 1.

    def _set_slope( self, value ):
      self._image._aims.header()[ 'scale_factor' ] = value

    def intercept( self ):
      try:
        s = self._image._aims.header()[ 'scale_offset' ]
        return s
      except:
        return 0.

    def _set_intercept( self, value ):
      self._image._aims.header()[ 'scale_offset' ] = value

    def sform( self ):
      return self._image.header[ 'sform' ]

    def _set_sform( self, value ):
      self._image.header[ 'sform' ] = value

    def sform_inv( self ):
      sf = self.sform()
      return numpy.linalg.inv( sf )

    def _set_sform_inv( self, value ):
      raise ValueError( 'this attribute cannot be set directly yet' )

    def qform( self ):
      qf = self._image.header.FakeHeaderAttributes( self._image.header ) \
        ._qform()
      if qf is None:
        # I just follow the strange behaviour of pynifti...
        qf = numpy.diag( self._image.voxdim[ :3 ] + ( 1, ) )
      return qf

    def _set_qform( self, value ):
      raise ValueError( 'this attribute cannot be set directly yet' )

    def qform_inv( self ):
      qf = self.qform()
      return numpy.linalg.inv( qf )

    def _set_qform_inv( self, value ):
      raise ValueError( 'this attribute cannot be set directly yet' )

    def quatern( self ):
      return self._image.header[ 'quatern' ]

    def _set_quatern( self, value ):
      self._image.header[ 'quatern' ] = value

    def qfac( self ):
      return self._image.header[ 'qfac' ]

    def _set_qfac( self, value ):
      self._image.header[ 'qfac' ] = value

    def qoffset( self ):
      return self._image.header[ 'qoffset' ]

    def _set_qoffset( self, value ):
      self._image.header[ 'qoffset' ] = value

    # end of _ProxyAttributes

  # NiftiImage methods
  def __init__( self, aimsvolume ):
    if type( aimsvolume ) in types.StringTypes:
      filename = aimsvolume
      aimsvolume = aims.read( filename )
      self._aims = aimsvolume
      self.filename = filename
    elif type( aimsvolume ).__name__.startswith( 'AimsData_' ):
      aimsvolume = aimsvolume.volume().get()
      self._aims = aimsvolume
      self.filename = ''
    elif type( aimsvolume ).__name__.startswith( 'Volume_' ):
      self._aims = aimsvolume
      self.filename = ''
    else:
      raise TypeError( 'Wrong argument type: expecting filename or AIMS ' \
        'volume' )
    self.data = aimsvolume.arraydata()
    header = aimsvolume.header()
    self.header = self._FakeHeader( header )

  def save( self, filename ):
    aims.write( self._aims, filename )
    self.filename = filename

  def load( self, filename ):
    self._aims = aims.read( filename )
    self.filename = filename
    self.data = self._aims.arraydata()
    self.header = self._FakeHeader( self._aims.header() )

  def asDict( self ):
    return self.header

  def asarray( self ):
    return self.data

  def getVolumeExtent( self ):
    return self.volextent

  def getBoundingBox( self ):
    return self.bbox

  def getExtent( self ):
    return self.extent

  def getFilename( self ):
    return self.filename

  def getPixDims( self ):
    return self.pixdim

  def getVoxDims( self ):
    return self.voxdim

  def getDataArray( self ):
    return self.data

  def getInverseQForm( self ):
    return self.qform_inv

  def getInverseSForm( self ):
    return self.sform_inv

  def getQForm( self ):
    return self.qform

  def getQOffset( self ):
    return self.qoffset

  # def getQOrientation( self ): TODO...

  def getQuaternion( self ):
    return self.quaternion

  def getRepetitionTime( self ):
    return self.rtime

  def getSForm( self ):
    return self.sform

  # def getSOrientation( self ): TODO...

  def getScaledData( self ):
    # should convert to float/double ?
    return self.data

  def getTimepoints( self ):
    return self.timepoints

  def setDescription( self, value ):
    self.description = value

  def setFilename( self, value ):
    self.filename = value

  def setIntercept( self, value ):
    self.intercept = value

  def setPixDims( self, value ):
    self.pixdim = value

  def setQFac( self, value ):
    self.qfac = value

  def setQForm( self, value ):
    self.qform = value

  def setQOffset( self, value ):
    self.qoffset = value

  def setQuaternion( self, value ):
    self.quatern = value

  def setRepetitionTime( self, value ):
    self.rtime = value

  def setSForm( self, value ):
    self.sform = value

  def setSlope( self, value ):
    self.slope = value

  def setVoxDims( self, value ):
    self.voxdim = value

  #def setXFormCode( self, value ):

  def updateCalMinMax( self ):
    self.header[ 'cal_min' ] = self.min
    self.header[ 'cal_max' ] = self.max

  def updateFromDict( self, hdrdict ):
    for x, y in hdrdict.iteritems():
      if x not in [ 'bitpix', 'cal_min', 'cal_max', 'data_type', 'datatype',
        'dim' ]:
        self.header[x] = y

  def updateHeader( self, hdrdict ):
    self.updateFromdict( hdrdict )

  def updateQFormFromQuaternion( self ):
    pass # just do nothing, it's done dynamically

  def __getattr__( self, attr ):
    if not attr.startswith( '_' ):
      px = NiftiImage._ProxyAttributes( self )
      a = getattr( px, attr, None )
      if a:
        return a()
    return object.__getattribute__( self, attr )

  def __setattr__( self, attr, value ):
    if not attr.startswith( '_' ):
      px = NiftiImage._ProxyAttributes( self )
      a = getattr( px, '_set_' + attr, None )
      if a:
        return a( value )
    return object.__setattr__( self, attr, value )

  def _getAttributeNames( self ):
    '''for IPython completion'''
    m = self.__dict__.keys()
    m += filter( lambda x: not x.startswith( '_' ),
      self._ProxyAttributes.__dict__.keys() )
    return m

  # new methods not in pynifti API
  def getTransformation( self, num, failIfNotFound=False ):
    '''Get the AIMS transformation by its number in AIMS header.
    This transformation is from the AIMS referential to the corresponding
    destination referential (see getReferential() method).
    If the transformation is not available in the header, if failIfNotFound is
    True, an exceptin is raised; otherwise an identity transformation is
    returned.
    Normally the 1st transformation (num=0) would correspond to qform (but
    from the AIMS referential, not the disk referential), and the 2nd one
    would correspond to sform (if qform is also here).
    '''
    return self.header.FakeHeaderAttributes( self.header ). \
      _getTransformation( num, failIfNotFound )

  def getReferential( self, num, failIfNotFound=False ):
    return self.header.FakeHeaderAttributes( self.header ). \
      _getReferential( num, failIfNotFound )

  def getReferentialCode( self, num ):
    return self.header.FakeHeaderAttributes( self.header ). \
      _getReferentialCode( num )

  def getStorageToMemory( self ):
    '''Get the AIMS transformation from disk space (storage referential, in mm)
    to AIMS space (also in mm)
    '''
    return self.header.FakeHeaderAttributes( self.header ).\
      _getStorageToMemory()

  def setStorageToMemory( self, value ):
    '''Set the AIMS transformation from disk space (storage referential, in mm)
    to AIMS space (also in mm). In other words: change the disk save
    orientation.
    value may be an aims.Motion, a (4,4) numpy array, or a line vector
    '''
    self.header[ 'storage_to_memory' ] = value

  def getMemorytoStorage( self ):
    '''Get the AIMS transformation from AIMS space (in mm) to disk space
    (storage referential, also in mm)
    '''
    return self.header.FakeHeaderAttributes( self.header ).\
      _getMemoryToStorage()

  def setMemoryToStorage( self, value ):
    '''Set the AIMS transformation from AIMS space (in mm) to disk space
    (storage referential, also in mm). In other words: change the disk save
    orientation.
    value may be an aims.Motion, a (4,4) numpy array, or a line vector.
    '''
    self.header.FakeHeaderAttributes( self.header ).\
      _setMemoryToStorage( value )

  def getVoxelSizeMotion( self ):
    '''Returns the 3D voxel sizes as an aims.Motion object, in AIMS
    orientation.
    '''
    return self.header.FakeHeaderAttributes( self.header ).\
      _getVoxelSizeMotion_aims()

  def getVoxelSizeMotion_storage( self ):
    '''Returns the 3D voxel sizes as an aims.Motion object, in disk storage
    orientation.'''
    self.header.FakeHeaderAttributes( self.header ).\
      _getVoxelSizeMotion_storage()
