# -*- coding: utf-8 -*-
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

'''
The aims module allows access to the AIMS library in python.

- organization: `NeuroSpin <http://www.neurospin.org>`_ and `IFR 49 <http://www.ifr49.org>`_

- license: `CeCILL-B <http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html>`_ (a free licence comparable to BSD)

Most of it is a set of direct bindings to the
`C++ library <../../aimsdata-4.1/doxygen/index.html>`_ API. But a few
classes have been customized or specially written to make easy links to the
python world.

aims contains mainly the AIMS and carto data structures and IO system.
It covers several C++ libraries: cartobase, cartodata, graph, aimsdata, plus
some C++ classes from the standard library (like std::vector)

Main classes:
  - :py:class:`Reader` and :py:class:`Writer` for the generic IO system,
    which allows to read and write everything, and which can be, in most
    cases, replaced with the more convenient global :py:func:`read` and
    :py:func:`write` functions. :py:class:`Finder` is
    also part of the IO system to identify files and get information about
    them.
  - Volume_<type> and AimsData_<type>: volumes (the cartodata newer Volume
    and the older aimsdata version). An important feature of Volumes is
    the link with the numpy_ arrays, so all the power of numpy_ can be
    used to work on volumes. See for instance :py:class:`Volume_FLOAT`.
    Volumes of various types can be build using the convenience factory
    functions :py:func:`Volume` and :py:func:`AimsData`.
  - :py:class:`carto.GenericObject`: the dynamic C++ generic object which behaves
    much like a python object, and its proxy :py:class:`Object` which should
    always be used to access a :py:class:`GenericObject`
  - AimsVector_<type>_<size> for fixed-size vectors, and their aliases
    for points: :py:class:`Point3df`, :py:class:`Point2df`,
    :py:class:`Point3d`
  - vector_<type>, set_<type>, list_<type>, map_<type1>_<type2> provide
    bindings to the C++ std::vector, std::list, std::set, std::map template
    classes and easy conversions from / to python lists
  - rc_ptr_<type>: bridge with C++ reference counters, which should
    interact quite well, and most of the time transparently, with python
    reference counting
  - :py:class:`BucketMap_VOID`: list of voxels. Created using the factory
    function :py:func:`BucketMap`
  - AimsTimeSurface_<polygons_size>: meshes, and the factory function
    :py:func:`AimsTimeSurface`
  - TimeTexture_<type>: textures to map on meshes, and the factory function
    :py:func:`TimeTexture`.
  - :py:class:`Graph`
  - :py:class:`Tree`
  - :py:class:`AffineTransformation3d`: the transformation class that allows
    referential conversions
  - Converter_<type1>_<type2> and ShallowConverter_<type1>_<type2>:
    conversions between data types (mainly volume types). See the factory
    functions :py:func:`Converter` and :py:func:`ShallowConverter`
  - a few algorithms will be added

.. _numpy: http://numpy.scipy.org/
'''
__docformat__ = 'restructuredtext en'


import collections
import types
import sip
import os

# check for share dir, and set the BRAINVISA_SHARE environment var if it is not
# already set
if 'BRAINVISA_SHARE' not in os.environ:
  sharepath = os.path.join( os.path.dirname( os.path.dirname( os.path.dirname( \
    os.path.dirname( __file__ ) ) ) ), 'share' )
  os.environ[ 'BRAINVISA_SHARE' ] = sharepath
  os.putenv( 'BRAINVISA_SHARE', sharepath ) # environ[] and putenv() seem to
  # have slightly different scopes...
del os

from soma.aims import aimssip
#from soma.functiontools import partial
from soma.importer import ExtendedImporter, GenericHandlers

# Rename sip modules and reorganize namespaces using the Importer class
# this class manages changes that can occur during import of other modules
# for example the import of aimsalgo module modifies the aims module. The
# Importer class manages the rules to apply and applies all rules after each
# module import.
ExtendedImporter().importInModule( '', globals(), locals(), 'aimssip' )
ExtendedImporter().importInModule( '', globals(), locals(), 'aimssip', [],
  [GenericHandlers.removeChildren], ['Reader_', 'Writer_']  )
ExtendedImporter().importInModule( '', globals(), locals(), 'aimssip',
  ['aimssip.aims'] )
# move Object out of carto namespace
Object = carto.Object

del aims, ExtendedImporter

from soma.aims import hierarchy
del hierarchy # init has been done, get rid of it...
try:
  from soma.aims.spmnormalizationreader import *
except:
  pass # probably cannot import scipy.io

# typedefs

try:
  AimsSurfaceTriangle = AimsTimeSurface_3
except:
  pass # no surface support

# RCObject constructor / destructor

def RCObject_init( self, *args ):
  # print 'RCObject init'
  carto.RCObject.__oldinit__( self, *args )
  carto.RCObject._setupRC( self )

def RCObject_del( self ):
  # print 'RCObject_del', self
  # Prevent a corner case when an object is created in python side and
  # deleted within C++ side. Thus, the sip wrapper doesn't contain its
  # wrapped C++ object any more, so all deleting works are commited to C++.
  if sip.isdeleted( self ):
    # print 'already deleted'
    return
  carto.RCObject._releaseRC( self )
  if hasattr( self, '_dontdel' ):
    del self._dontdel
  else:
    if getattr( carto.RCObject, '__olddel__', None ):
      carto.RCObject.__olddel__( self )

carto.RCObject.__oldinit__ = carto.RCObject.__init__
carto.RCObject.__init__ = RCObject_init
if getattr( carto.RCObject, '__del__', None ):
  carto.RCObject.__olddel__ = carto.RCObject.__del__
carto.RCObject.__del__ = RCObject_del

del RCObject_init, RCObject_del


# generic Reader class

class Reader:
  '''
  Generic reader that can theorerically load any SIP-mapped AIMS or
  Cartograph object. A translation table can be provided to correctly map
  readers and objects.
  For quick and simple operations, you can also use the gloabl
  :py:func:`soma.aims.read` and :py:func:`soma.aims.write` functions, which use
  a Reader object internally.
  '''
  def __init__( self, typemap = None, allocmode = None, options = None ):
    '''
    typemap can be provided to specify which reader may
    be used to load objects whose type has been read. A default internal map
    is present but can be replaced.

    The map has 2 modes:

    - object_type : object_type (ex: ``{'Volume' : 'AimsData'}`` )
    - object_type : dict( data_type : full_type )
      (ex: ``'Mesh' : { 'VOID' : 'AimsSurfaceTriangle' }`` )

      A default object_type can be specified if the data_type is not
      found:

      .. code-block:: python

        'Mesh' : { 'VOID' : 'AimsSurfaceTriangle',
                    'default_object_type' : 'AimsTimeSurface_3' }

      (and this example corresponds to the default internal map if none is
      specified)
    '''
    if typemap is None:
      self._typemap = { #'Volume' : 'AimsData', \
                        'Segments' : \
                        { 'VOID' : 'AimsTimeSurface_2',
                          'default_object_type' : 'AimsTimeSurface_2',
                          },
                        'Mesh' : \
                        { 'VOID' : 'AimsTimeSurface_3',
                          'default_object_type' : 'AimsTimeSurface_3',
                          },
                        'Mesh4' : \
                        { 'VOID' : 'AimsTimeSurface_4',
                          'default_object_type' : 'AimsTimeSurface_4',
                          },
                        'Graph' : { 'VOID' : 'Graph', },
                        'AffineTransformation3d' : \
                        { 'VOID' : 'AffineTransformation3d', },
                        'Bucket' : 'BucketMap',
                        'Texture' : 'TimeTexture',
                        'Tree' : { 'hierarchy' : 'Hierarchy' },
                        'genericobject' : { 'any' : 'Object' },
                        }
    else:
      self._typemap = typemap
    if allocmode is None:
      self.allocmode = None
    elif isinstance( allocmode, carto.AllocatorContext ):
      self.allocmode = allocmode
    elif isinstance( allocmode, carto.AllocatorStrategy.DataAccess ):
      self.allocmode = carto.AllocatorContext( allocmode )
    else:
      raise TypeError( 'allocmode argument (2) must be either a ' \
        'aims.carto.AllocatorContext or a carto.AllocatorStrategy.DataAccess' )
    self.options = options

  def read( self, filename, border = 0, frame = -1, dtype=None ):
    '''Reads the object contained in the file <filename>, whatever the type
    of the contents of the file. All objects types supported by Aims IO system
    can be read. A border width and a frame number may be specified and will
    be only used by formats that support them.
    If <dtype> is specified, the corresponding object/data type is forced. It
    may be useful to force reading a volume with float voxels for instance.
    It is only supported by a few formats. <dtype> may contain a string or
    a type object, as accepted by :py:func:`soma.aims.typeCode`.
    The read function may follow other object/data types rules, allocators and
    options, as specified in the Reader constructor.
    '''
    f = Finder()
    if not f.check( filename ):
      open(filename).close() # "file not found"-case raising first
      raise IOError( 'Unknown file format or missing meta-file(s): ' \
        + filename )
    if dtype is not None:
      finaltype = typeCode( dtype )
    else:
      otype = f.objectType()
      dtype = f.dataType()
      otype2 = self._typemap.get( otype, otype )
      if type( otype2 ) is types.StringType:
        finaltype = otype2 + '_' + dtype
        otype = otype2
      else:
        finaltype = otype2.get( dtype )
        if finaltype is None:
          otype2 = otype2.get( 'default_object_type' )
          if otype2 is not None:
            finaltype = otype2 + '_' + dtype
          else:
            finaltype = otype + '_' + dtype
    rdr = 'Reader_' + finaltype
    r = getattr( aimssip, rdr, None )
    if r is None:
      raise IOError( 'Unsupported object type: ' + finaltype )
    r = r( filename )
    if self.allocmode is not None:
      r.setAllocatorContext( self.allocmode )
    if self.options:
      r.setOptions( self.options )
    return r.read( border, f.format(), frame )

  def mapType( self, iotype, aimstype ):
    self._typemap[ iotype ] = aimstype


# generic Writer class

class Writer:
  def __init__( self ):
    self._objectType = None
    self._dataType = None
    self._fullType = None
    pass

  def write( self, obj, filename, format=None, options={} ):
    '''Writes the object <obj> in a file named <filename>, whatever the type of
    <obj>, as format <format>.
    All objects types and formats supported by the Aims IO system can be used.
    <obj> may be a reference-counter to an object type supported by the IO
    system.
    Additional specific options may be passed to the underlying IO system in an
    optional <options> dictionary.
    '''
    c = obj.__class__.__name__.split( '.' )[ -1 ]
    wr = 'Writer_' + c
    self._objectType = None
    self._dataType = None
    self._fullType = None
    try:
      W = getattr( aimssip, wr )
    except:
      if c.startswith( 'rc_ptr_' ):
        obj = obj._get()
        # build a list of parent classes, so that we can try a more
        # generic writer if the exact type of the object does not have a
        # specific Writer in C++
        tryclass = [ obj.__class__ ]
      else:
        tryclass = list( obj.__class__.__bases__ )
      tried = set()
      W = None
      while len( tryclass ) != 0:
        ocl = tryclass[0]
        del tryclass[0]
        wr = 'Writer_' + ocl.__name__.split( '.' )[ -1 ]
        W = getattr( aimssip, wr, None )
        if W is None:
          tried.add( ocl )
          tryclass += [ x for x in ocl.__bases__ if x not in tried ]
        else:
          break
      if W is None:
        raise AttributeError( 'no Writer for type ' + \
          obj.__class__.__name__ )
    w = W( filename, options )
    w.write( obj, False, format )
    try:
      self._objectType = w.writtenObjectType()
      self._dataType = w.writtenObjectDataType()
      self._fullType = w.writtenObjectFullType()
    except:
      raise #'Unsupported object type: ' + obj.__class__.__name__

  def writtenObjectType( self ):
    return self._objectType

  def writtenObjectDataType( self ):
    return self._dataType

  def writtenObjectFullType( self ):
    return self._fullType


# simple IO functions

def read( filename, border=0, frame=-1, dtype=None, allocmode=None,
    options=None ):
  '''Equivalent to:

  .. code-block:: python

    r = Reader( allocmode=allocmode, options=options )
    return r.read( filename, border=border, frame=frame, dtype=dtype )
  '''
  r = Reader( allocmode=allocmode, options=options )
  return r.read( filename, border=border, frame=frame, dtype=dtype )

def write( obj, filename, format=None, options={} ):
  '''Equivalent to:

  .. code-block:: python

    w = Writer()
    w.write( obj, filename, format=format, options=options )
  '''
  w = Writer()
  w.write( obj, filename, format=format, options=options )


# vector-like iterator

class VecIter:
  '''iterator class for some aims containers (AimsVector)'''
  def __init__( self, vec ):
    self._vector = vec
    self._index = 0
  def __iter__( self ):
    return self
  def next( self ):
    if self._index >= len( self._vector ):
      raise StopIteration( 'iterator outside bounds' )
    val = self._vector[ self._index ]
    self._index += 1
    return val

# Iterator (doesn't work when implemented in SIP so far)
def newiter( self ):
  return self.__objiter__()

def newnext( self ):
  return self.__objnext__()

def objiter( self ):
  return self._get().__iter__()

def objnext( self ):
  return self._get().next()

def objiteritems( self ):
  class iterator( object ):
    def __init__( self, it ):
      self.iterator = it
    def __iter__( self ):
      return self
    def next( self ):
      if not self.iterator.isValid():
        raise StopIteration( "iterator outside bounds" )
      res = ( self.iterator.key(), self.iterator.currentValue() )
      self.iterator.next()
      return res
  return iterator( self.objectIterator() )

def objitervalues( self ):
  class iterator( object ):
    def __init__( self, it ):
      self.iterator = it
    def __iter__( self ):
      return self
    def next( self ):
      if not self.iterator.isValid():
        raise StopIteration( "iterator outside bounds" )
      res = self.iterator.currentValue()
      self.iterator.next()
      return res
  return iterator( self.objectIterator() )

class BckIter:
  '''iterator class for bucket containers'''
  def __init__( self, bucket ):
    self._bck = bucket
    self._iter = None
  def __iter__( self ):
    return self
  def next( self ):
    if self._iter is None:
      self._iter = iter( self._bck.keys() )
    elem = self._iter.next()
    return self._bck[ elem ]

class BckIterItem:
  '''item iterator class for bucket containers'''
  def __init__( self, bucket ):
    self._bck = bucket
    self._iter = None
  def __iter__( self ):
    return self
  def next( self ):
    if self._iter is None:
      self._iter = iter( self._bck.keys() )
    elem = self._iter.next()
    return elem, self._bck[ elem ]

# automatic rc_ptr dereferencing
def proxygetattr( self, attr ):
  try:
    return self.__getattribute__( attr )
  except:
    if self.__getattribute__( 'isNull' )():
      raise RuntimeError( 'Null object called' )
    return getattr( self._get(), attr )

def proxylen( self ):
  if self.isNull():
    raise TypeError( "object of type 'Object' has no len()" )
  return self._get().__len__()

def proxygetitem( self, attr ):
  if self.isNull():
    raise RuntimeError( 'Null object called' )
  return self._get().__getitem__( attr )

def proxysetitem( self, attr, value ):
  try:
    return __setitem__( self, attr, value )
  except:
    return self._get().__setitem__( attr, value )

def proxydelitem( self, attr ):
  try:
    return self.__delitem__( attr )
  except:
    if self.isNull():
      raise RuntimeError( 'Null object called' )
    return self._get().__delitem__( attr )

def proxynonzero( self ):
  if self.isNull():
    return False
  try:
    return self._get().__nonzero__()
  except:
    try:
      return len( self._get() )
    except:
      return True

def proxystr( self ):
  if self.isNull():
    return 'None'
  return self._get().__str__()

def rcptr_getAttributeNames( self ):
  '''IPython completion feature...'''
  m = []
  l = [ self._get(), self ]
  done = set()
  while l:
    c = l.pop()
    done.add( c )
    m += filter( lambda x: not x.startswith( '_' ) and x not in m,
      c.__dict__.keys() )
    cl = getattr( c, '__bases__', None )
    if not cl:
      cl = getattr( c, '__class__', None )
      if cl is None:
        continue
      else:
        cl = [ cl ]
    l += filter( lambda x: x not in done, cl )
  return m

def __getitem_vec__( self, s ):
  if isinstance( s, slice ):
    start, stop, step = s.indices(len(self))
    return [ self.__oldgetitem__(x) for x in xrange(start, stop, step) ]
  else:
    return self.__oldgetitem__( s )

def __setitem_vec__( self, s, val ):
  if isinstance( s, slice ):
    start, stop, step = s.indices(len(self))
    return [ self.__oldsetitem__(x, v) for x, v in
             zip(xrange(start, stop, step), val) ]
  else:
    return self.__oldsetitem__( s, val )


# scan classes and modify some of them
def __fixsipclasses__( classes ):
  '''Fix some classes methods which Sip doesn't correctly bind'''
  for x, y in classes:
    try:
      if y.__name__.startswith( 'rc_ptr_' ) \
        or y.__name__.startswith( 'weak_shared_ptr_' ) \
        or y.__name__.startswith( 'weak_ptr_' ):
        # add __getattr__ method
        y.__len__ = __fixsipclasses__.proxylen
        y.__getattr__ = __fixsipclasses__.proxygetattr
        y.__getitem__ = __fixsipclasses__.proxygetitem
        # y.__setitem__ = __fixsipclasses__.proxysetitem
        y.__delitem__ = __fixsipclasses__.proxydelitem
        y.__str__ = __fixsipclasses__.proxystr
        y.__nonzero__ = __fixsipclasses__.proxynonzero
        y._getAttributeNames = __fixsipclasses__.getAttributeNames
        y.get = lambda self: self._get() # to maintain compatibiity with pyaims 3.x
      elif y.__name__.startswith( 'ShallowConverter_' ):
        y.__oldcall__ = y.__call__
        y.__call__ = lambda self, obj: self.__oldcall__( obj )._get()
      else:
        if hasattr( y, '__objiter__' ):
          y.__iter__ = __fixsipclasses__.newiter
        if hasattr( y, '__objnext__' ):
          y.next = __fixsipclasses__.newnext
        elif y.__name__.startswith( 'AimsVector_' ) \
          or y.__name__.startswith( 'Texture_' ):
          y.__iterclass__ = VecIter
          y.__iter__ = lambda self: self.__iterclass__( self )
        if y.__name__.startswith( 'vector_' ) \
          or y.__name__.startswith( 'AimsVector_' ):
          y.__oldgetitem__ = y.__getitem__
          y.__getitem__ = __fixsipclasses__.__getitem_vec__
          y.__oldsetitem__ = y.__setitem__
          y.__setitem__ = __fixsipclasses__.__setitem_vec__
        if y.__name__.startswith( 'BucketMap_' ):
          y.Bucket.__iterclass__ = BckIter
          y.Bucket.__iteritemclass__ = BckIterItem
          y.Bucket.__iter__ = lambda self: self.__iterclass__( self )
          y.Bucket.iteritems = lambda self: self.__iteritemclass__( self )
    except:
      pass

__fixsipclasses__.newiter = newiter
__fixsipclasses__.newnext = newnext
__fixsipclasses__.objiter = objiter
__fixsipclasses__.objnext = objnext
__fixsipclasses__.proxygetattr = proxygetattr
__fixsipclasses__.proxylen = proxylen
__fixsipclasses__.proxygetitem = proxygetitem
__fixsipclasses__.proxysetitem = proxysetitem
__fixsipclasses__.proxydelitem = proxydelitem
__fixsipclasses__.proxystr = proxystr
__fixsipclasses__.proxynonzero = proxynonzero
__fixsipclasses__.getAttributeNames = rcptr_getAttributeNames
__fixsipclasses__.__getitem_vec__ = __getitem_vec__
__fixsipclasses__.__setitem_vec__ = __setitem_vec__
del newiter, newnext, objiter, objnext, proxygetattr, proxylen
del proxygetitem, proxysetitem, proxystr, proxynonzero
del rcptr_getAttributeNames
del __getitem_vec__, __setitem_vec__

__fixsipclasses__( globals().items() + carto.__dict__.items() )

Object.__iter__ = __fixsipclasses__.objiter
Object.next = __fixsipclasses__.objnext
Object.__delitem__ = __fixsipclasses__.proxydelitem
Object._getAttributeNames = __fixsipclasses__.getAttributeNames

del BckIter, BckIterItem, VecIter

__fixsipclasses__.fakerepr = lambda a : "<%s.%s object at 0x%x>" % (a.__class__.__module__, a.__class__.__name__, id(a))

# added aimssip.Point3df attributes/methods
Point4d = AimsVector_S16_4
Point4du = AimsVector_U32_4
Point4df = AimsVector_FLOAT_4
Point4dd = AimsVector_DOUBLE_4
Point3d = AimsVector_S16_3
Point3du = AimsVector_U32_3
Point3df = AimsVector_FLOAT_3
Point3dd = AimsVector_DOUBLE_3
Point2d = AimsVector_S16_2
Point2du = AimsVector_U32_2
Point2df = AimsVector_FLOAT_2
Point2dd = AimsVector_DOUBLE_2
Point3df.__doc__ = """This class wraps an aims Point3df aka AimsVector<3, float>
use the method items() to get a tuple out of it
use setItems(Point3df) to affect new values"""

Point4d.__repr__ = Point4du.__repr__ = Point4df.__repr__ = Point4dd.__repr__ \
= Point3d.__repr__ = Point3du.__repr__ = Point3df.__repr__ = Point3dd.__repr__\
= Point2d.__repr__ = Point2du.__repr__ = Point2df.__repr__ = Point2dd.__repr__\
= lambda self: __fixsipclasses__.fakerepr(self) + "\n" + str(self.items())


import numpy
def __toMatrix(s):
    """ This function return a copy of the transformation matrix """
    m = numpy.identity(4)
    t, r = s.translation(), s.rotation()
    m[0:3,0:3].transpose().flat = [r.value(x) for x in xrange(9)]
    m[0:3,3].flat = t.items()
    return m
def __AffineTransformation3dFromMatrix(self, value):
  self.rotation().volume().arraydata().reshape(3,3).transpose()[:,:] \
    = value[ 0:3, 0:3 ]
  self.translation().arraydata()[:] = value[ 0:3, 3 ].flatten()
def __AffineTransformation3d__init__( self, *args ):
  if len( args ) != 0 and isinstance( args[0], numpy.ndarray ) \
    and args[0].shape == ( 4, 4 ):
    self.__oldinit__()
    self.fromMatrix( args[0] )
  else:
    self.__oldinit__( *args )
def __AffineTransformation3d__header(self):
  h = AffineTransformation3d.__oldheader__(self)
  h.__motion=self
  return h
AffineTransformation3d.toMatrix = __toMatrix
AffineTransformation3d.fromMatrix = __AffineTransformation3dFromMatrix
AffineTransformation3d.__oldinit__ = AffineTransformation3d.__init__
AffineTransformation3d.__init__ = __AffineTransformation3d__init__
AffineTransformation3d.__oldheader__ = AffineTransformation3d.header
AffineTransformation3d.header = __AffineTransformation3d__header
del __toMatrix, __AffineTransformation3dFromMatrix, \
  __AffineTransformation3d__init__, __AffineTransformation3d__header
# backward compatibility
Motion = AffineTransformation3d

AffineTransformation3d.__repr__ = lambda self : __fixsipclasses__.fakerepr(self) + "\n"+str(self.toMatrix())
AffineTransformation3d.__str__ = lambda self: self.toMatrix().__str__()

# This one add support for += in 3D mesh
AimsTimeSurface_3.__iadd__ = lambda s, o : SurfaceManip.meshMerge(s, o) or s

# conversions from GenericObject
convertersObjectToPython = {
  'PyObject' : carto.PyObjectfromObject,
  'rc_ptr of bucket of VOID' : BucketMap_VOID.fromObject,
  'rc_ptr of bucket of S16' : BucketMap_S16.fromObject,
  'rc_ptr of bucket of U16' : BucketMap_U16.fromObject,
  'rc_ptr of bucket of S32' : BucketMap_S32.fromObject,
  'rc_ptr of bucket of U32' : BucketMap_U32.fromObject,
  'rc_ptr of bucket of FLOAT' : BucketMap_FLOAT.fromObject,
  'rc_ptr of bucket of DOUBLE' : BucketMap_DOUBLE.fromObject,
  'rc_ptr of mesh of VOID' : AimsSurfaceTriangle.fromObject,
  'rc_ptr of Mesh4 of VOID' : AimsTimeSurface_4.fromObject,
  'rc_ptr of Segments of VOID' : AimsTimeSurface_2.fromObject,
  'rc_ptr of texture of S16' : TimeTexture_S16.fromObject,
  'rc_ptr of texture of S32' : TimeTexture_S32.fromObject,
  'rc_ptr of texture of U32' : TimeTexture_U32.fromObject,
  'rc_ptr of texture of FLOAT' : TimeTexture_FLOAT.fromObject,
  'rc_ptr of texture of POINT2DF' : TimeTexture_POINT2DF.fromObject,
  'POINT2DF' : Point2df.fromObject,
  'POINT3DF' : Point3df.fromObject,
  'POINT4DF' : Point4df.fromObject,
  'S16' : carto.NumericGenericObjectConverter.asInt,
  'U16' : carto.NumericGenericObjectConverter.asInt,
  'S32' : carto.NumericGenericObjectConverter.asInt,
  'U32' : carto.NumericGenericObjectConverter.asInt,
  'boolean' : carto.NumericGenericObjectConverter.asBool,
  'string' : lambda x: x.getString(),
}

def getPython( self ):
  """
  Conversion to python types: extracts what is in the Object (when
  possible). The global dictionary
  :py:data:`soma.aims.convertersObjectToPython` stores
  converters
  """
  t = self.type()
  cv = convertersObjectToPython.get( t )
  res = None
  gen = self._get()
  if cv is not None:
    res = cv( gen )
    try:
      res.__genericobject__ = gen
    except:
      # here we lose the GenericObject because we cannot store it
      # so the object will be copied each time it is put in a generic
      # object again
      pass
    return res
  try:
    return self.getScalar()
  except:
    pass
  try:
    return self.getString()
  except:
    pass
  if t.find( 'volume' ) >= 0:
    dt = t.split()[-1]
    try:
      res = eval( 'AimsData_' + dt ).fromObject( gen )
      res.__genericobject__ = gen
      return res
    except:
      pass
  if t.startswith( 'vector of' ):
    dt = t.split()[-1]
    try:
      vectype = eval( 'vector_' + dt )
      res = vectype.fromObject( gen )
      res.__genericobject__ = gen
      return res
    except:
      pass
  if t.startswith( 'VECTOR_OF_' ):
    try:
      dt = t[ 10: ]
      vectype = eval( 'AimsVector_' + dt )
      res = vectype.fromObject( gen )
      res.__genericobject__ = gen
      return res
    except:
      pass

  raise RuntimeError( 'No conversion to python object possible from type ' \
    + t )

def toObject( x ):
  if hasattr( x, 'toObject' ):
    return x.toObject()
  return Object( x )

def ptrToObject( x ):
  if hasattr( x, 'ptrToObject' ):
    return x.ptrToObject()
  return Object( x )

def rcToObject( x ):
  if hasattr( x, 'rcToObject' ):
    return x.rcToObject()
  return Object( x )



Object.__len__ = __fixsipclasses__.proxylen
Object.__getattr__ = __fixsipclasses__.proxygetattr
Object.__getitem__ = __fixsipclasses__.proxygetitem
Object.__setitem__ = __fixsipclasses__.proxysetitem
Object.__str__ = __fixsipclasses__.proxystr
Object.__repr__ = Object.__str__
Object.getPython = getPython
Object.toObject = staticmethod( toObject )
Object.ptrToObject = staticmethod( ptrToObject )
Object.rcToObject = staticmethod( rcToObject )

del getPython
del toObject, ptrToObject, rcToObject


# customize GenericObject to get automatic conversions between
# Objects and concrete types
class _proxy:
  def retvalue( x ):
    if callable( x ):
      if isinstance( x, carto.GenericObject._proxy ):
        return x
      return carto.GenericObject._proxy(x)
    if isinstance( x, Object ):
      try:
        return x.getPython()
      except:
        return x
    elif isinstance( x, carto.GenericObject ):
      try:
        return Object.getPython( x )
      except:
        return Object( x )
    return x
  retvalue = staticmethod( retvalue )

  def __init__(self,x):
    if isinstance( x, carto.GenericObject._proxy ):
      self._x = x._x
    else:
      self._x = x
  def __call__( self, *args, **kwargs ):
    r = self._x
    obj = getattr( r, '__self__', None )
    if obj is not None:
      # determine if we were called recursively.
      # We need to detect this because SIP methods may call the proxy and
      # keep no other access to the C method
      import traceback
      sf = traceback.extract_stack()[-2]
      if sf[-2] == '__call__' and \
        sf[-1] == 'return carto.GenericObject._proxy.retvalue( r( *args, ' \
          '**kwargs ) )':
        # compare sf[0] and __file__, after removing .py/.pyc extension
        # which may make the comparison not to match
        f1 = sf[0]
        if f1.endswith( '.py' ):
          f1 = f1[:-3]
        elif f1.endswith( '.pyc' ):
          f1 = f1[:-4]
        f2 = __file__
        if f2.endswith( '.py' ):
          f2 = f2[:-3]
        elif f2.endswith( '.pyc' ):
          f2 = f2[:-4]
        if f1 == f2:
          # try to call with selfWasArg=True (class unound method)
          cl = object.__getattribute__( obj, '__class__' )
          return getattr( cl, r.__name__ )( obj, *args, **kwargs )
    return carto.GenericObject._proxy.retvalue( r( *args, **kwargs ) )

def genobj__getattribute__( self, attr ):
  ga = object.__getattribute__( self, '__oldgetattribute__' )
  x = ga( attr )
  if( attr.startswith( '__' ) ) or attr == '_proxy':
    return x
  return ga( '_proxy' ).retvalue( x )

def genobj__getitem__( self, item ):
  try:
    x = Object.getPython( self )
    return x.__getitem__( item )
  except:
    return carto.GenericObject._proxy.retvalue( \
      self.__oldgetitem__( item ) )

def genobj__new__( cls, *args, **kwargs ):
  if cls != carto.GenericObject \
    and cls.__getitem__ != carto.GenericObject.__getitem__:
    # modify __getitem__ on subclasses of GenericObject
    cls.__oldgetitem__ = cls.__getitem__
    cls.__getitem__ = carto.GenericObject.__getitem__
    if cls.__getattribute__ != carto.GenericObject.__getattribute__:
      cls.__oldgetattribute__ = cls.__getattribute__
      cls.__getattribute__ = carto.GenericObject.__getattribute__
  return carto.GenericObject.__oldnew__( cls, *args, **kwargs )

def genobj__update__( self, x ):
  if not self.isDictionary():
    raise ValueError( 'Generic object is not a dictionary-compatible object' )
  for x,y in x.iteritems():
    self[x] = y

def genobj__iadd__( self, x ):
  if not self.isDynArray():
    raise ValueError( 'Generic object is not a list-compatible object' )
  for y in x:
    self.append( y )
  return self

def obj__iadd__( self, x ):
  self._get().__iadd__( x )
  return self

carto.GenericObject.__repr__ = carto.GenericObject.__str__
carto.GenericObject._proxy = _proxy
del _proxy
carto.GenericObject.__oldgetitem__ = carto.GenericObject.__getitem__
carto.GenericObject.__getitem__ = genobj__getitem__
carto.GenericObject.__oldnew__ = carto.GenericObject.__new__
carto.GenericObject.__new__ = staticmethod( genobj__new__ )
carto.GenericObject.__oldgetattribute__ = carto.GenericObject.__getattribute__
carto.GenericObject.__getattribute__ = genobj__getattribute__
carto.GenericObject.update = genobj__update__
carto.GenericObject.__iadd__ = genobj__iadd__
carto.GenericObject.iteritems = objiteritems
carto.GenericObject.itervalues = objitervalues
carto.GenericObject.iterkeys = carto.GenericObject.__iter__
Object.__iadd__ = obj__iadd__
del genobj__getitem__, genobj__new__, genobj__update__, genobj__iadd__
del genobj__getattribute__
del objiteritems, objitervalues
del obj__iadd__


# trap every access to Point3df's instance methods to check for the
# weakref on the surface-object
# KNOWN BUG: using */+- in python skips __getattribute__, thus may sig11 :(
from new import instancemethod
import weakref
Point3df.__refParent = weakref.ref(Point3df)
Point3df.__oldgetattr__ = Point3df.__getattribute__
def __getattribute__(self, name):
  g = object.__getattribute__(self, "__oldgetattr__")
  if g("__refParent")():
    return g(name)
  else:
    raise "Underlying C++ object has been deleted"
Point3df.__getattribute__ = __getattribute__
del __getattribute__

# complement Quaternion
Quaternion.compose = Quaternion.__mul__

# templates / types helpers

def typeCode( data ):
  '''returns the AIMS type code for the given input data. data may be a string
  code, a python/numpy numeric type, an AIMS type, or an instance of such a
  type.
'''
  dmap = { 'int' : 'S32', 'int32' : 'S32', 'uint' : 'U32', 'uint32' : 'U32',
    'int64' : 'S64', 'uint64' : 'U64', 'int16' : 'S16', 'uint16' : 'U16',
    'char' : 'S8', 'uchar' : 'U8', 'float' : 'DOUBLE', 'float32' : 'FLOAT',
    'float64' : 'DOUBLE',
    numpy.int8 : 'S8', numpy.uint8 : 'U8',
    numpy.int16 : 'S16', numpy.uint16 : 'U16',
    numpy.int32 : 'S32', numpy.uint32 : 'U32',
    numpy.int64 : 'S64', numpy.uint64 : 'U64',
    numpy.float32 : 'FLOAT', numpy.float64 : 'DOUBLE',
    complex : 'CDOUBLE', numpy.complex64 : 'CFLOAT',
    numpy.complex128 : 'CDOUBLE', 'complex' : 'CDOUBLE',
  }
  dt = dmap.get( data, None )
  if dt is not None:
    return dt
  if type( data ) in types.StringTypes:
    return data
  dtn = getattr( data, '__name__', None )
  if dtn is not None: # if data is a type class
    dt = dmap.get( dtn, None )
    if dt is not None:
      return dt
  dtn2 = type( data ).__name__ # instance
  dt = dmap.get( dtn2, None )
  if dt is not None:
    return dt
  if dtn is not None:
    return dtn
  return dtn2


def _parseTypeInArgs( *args, **kwargs ):
  dtype = kwargs.get( 'dtype', None )
  # print '_parseTypeInArgs:', dtype
  if dtype is not None:
    # kwargs = dict( kwargs )
    del kwargs[ 'dtype' ]
  else:
    y = [ i for i, x in enumerate( args ) \
      if type(x) in types.StringTypes or hasattr( x, '__name__' ) ]
    if len( y ) != 0:
      i = y[0]
      dtype = args[ y[0] ]
      args = args[:i] + args[i+1:]
  if dtype is None:
    dtype = kwargs.get( 'default_dtype', None )
  if dtype is None:
    raise KeyError( 'data type not specified' )
  if 'default_dtype' in kwargs:
    del kwargs[ 'default_dtype' ]
  dtype = typeCode( dtype )
  return ( dtype, args, kwargs )


def _parse2TypesInArgs( *args, **kwargs ):
  dtype1 = kwargs.get( 'intype' )
  dtype2 = kwargs.get( 'outtype' )
  dtypeg = kwargs.get( 'dtype' )
  if dtype1 is not None:
    kwargs[ 'dtype' ] = dtype1
    del kwargs[ 'intype' ]
  intype, args2, kwargs2 = _parseTypeInArgs( *args, **kwargs )
  if dtype2 is not None:
    kwargs[ 'dtype' ] = dtype2
    del kwargs[ 'outtype' ]
  try:
    outtype, args3, kwargs3 = _parseTypeInArgs( *args2, **kwargs )
  except:
    outtype, args3, kwargs3 = _parseTypeInArgs( *args, **kwargs )
  return ( intype, outtype, args3, kwargs3 )


def _createObject( objtype, *args, **kwargs ):
  dtype, args, kwargs = _parseTypeInArgs( *args, **kwargs )
  return getattr( aimssip, objtype + '_' + dtype )( *args, **kwargs )


def Volume( *args, **kwargs ):
  '''Create an instance of Aims Volume (Volume_<type>) from a type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.
  The default type is 'FLOAT'.
  Type definitions should match those accepted by typeCode().
  Volume may also use a numpy array, or another Volume or AimsData_* as unique argument.
  Note that Volume( Volume_* ) or Volume( AimsData_* ) actually performs a copy of the data, whereas AimsData( Volume_* ) or AimsData( AimsData_* ) share the input data.
  '''
  if len( args ) == 1 and len( kwargs ) == 0:
    arg = args[0]
    if isinstance( arg, numpy.ndarray ):
      return _createObject( 'Volume', arg, dtype=arg.dtype.type )
    elif type( arg ).__name__.startswith( 'Volume_' ):
      return type( arg )( arg )
    elif type( arg ).__name__.startswith( 'AimsData_' ):
      return type( arg )( arg.volume() )
  return _createObject( 'Volume', default_dtype='FLOAT', *args, **kwargs )


def AimsData( *args, **kwargs ):
  '''Create an instance of the older Aims volumes (AimsData_<type>) from a type
  parameter, which may be specified as the dtype keyword argument, or as one of
  the arguments if one is identitied as a type.
  The default type is 'FLOAT'.
  Type definitions should match those accepted by typeCode().
  AimsData may also use a numpy array, or another Volume or AimsData_* as
  unique argument.
  Note that Volume( Volume_* ) or Volume( AimsData_* ) actually performs a copy
  of the data, whereas AimsData( Volume_* ) or AimsData( AimsData_* ) share the
  input data.
  '''
  if len( args ) == 1 and len( kwargs ) == 0:
    arg = args[0]
    if isinstance( arg, numpy.ndarray ):
      vol = _createObject( 'Volume', arg, dtype=arg.dtype.type )
      return AimsData( vol )
    elif type( arg ).__name__.startswith( 'Volume_' ):
      return getattr( aimssip, 'AimsData_' + type( arg ).__name__[ 7:] )( arg )
    elif type( arg ).__name__.startswith( 'AimsData_' ):
      return type( arg )( arg.volume() )
  return _createObject( 'AimsData', default_dtype='FLOAT', *args, **kwargs )


def TimeTexture( *args, **kwargs ):
  '''Create an instance of Aims texture (TimeTexture_<type>) from a type
  parameter, which may be specified as the dtype keyword argument, or as one of
  the arguments if one is identitied as a type.
  The default type is 'FLOAT'.
  Type definitions should match those accepted by typeCode().
  TimeTexture may also use a numpy array, or another TimeTexture_* or Textrue_*
  as unique argument.
  Building from a numpy arrays uses the 1st dimension as vertex, the 2nd as
  time (if any).
  '''
  if len( args ) == 1 and len( kwargs ) == 0:
    arg = args[0]
    if isinstance( args[0], numpy.ndarray ):
      tex = _createObject( 'TimeTexture', dtype=arg.dtype.type )
      if len( arg.shape ) == 1:
        tex[0].assign( arg )
      else:
        for i in xrange( arg.shape[1] ):
          tex[i].assign( arg[:,i] )
      return tex
    if type( arg ).__name__.startswith( 'TimeTexture_' ):
      return type( arg )( arg )
    elif type( arg ).__name__.startswith( 'Texture_' ):
      tex = getattr( aimssip, 'TimeTexture_' + type( arg ).__name__[ 8:] )()
      tex[0] = arg
      return tex
  return _createObject( 'TimeTexture', default_dtype='FLOAT', *args, **kwargs )


def Texture( *args, **kwargs ):
  '''Create an instance of Aims low-level texture (Texture_<type>) from a type
  parameter, which may be specified as the dtype keyword argument, or as one of
  the arguments if one is identitied as a type.
  The default type is 'FLOAT'.
  Type definitions should match those accepted by typeCode().
  Texture may also use a numpy array, or another Textrue_* as unique argument.
  '''
  if len( args ) == 1 and len( kwargs ) == 0:
    arg = args[0]
    if isinstance( args[0], numpy.ndarray ):
      return _createObject( 'Texture', arg, dtype=arg.dtype.type )
    if type( arg ).__name__.startswith( 'Texture_' ):
      return type( arg )( arg )
  return _createObject( 'Texture', default_dtype='FLOAT', *args, **kwargs )


def BucketMap( *args, **kwargs ):
  '''Create an instance of Aims bucket (BucketMap_<type>) from a type
  parameter, which may be specified as the dtype keyword argument, or as one of
  the arguments if one is identitied as a type.
  The default type is 'VOID'.
  Type definitions should match those accepted by typeCode().
  '''
  return _createObject( 'BucketMap', default_dtype='VOID', *args, **kwargs )


def Converter( *args, **kwargs ):
  '''Create a Converter instance from input and output types. Types may be
  passed as keyword arguments intype and outtype, or dtype if both are the same
  (not very useful for a converter). Otherwise the arguments are parsed to find
  types arguments.
  Types may be specified as allowed by typeCode().
  '''
  intype, outtype, args, kwargs = _parse2TypesInArgs( *args, **kwargs )
  return getattr( aimssip, 'Converter_' + intype + '_' + outtype )( *args )


def ShallowConverter( *args, **kwargs ):
  '''Create a ShallowConverter instance from input and output types. Types may
  be passed as keyword arguments intype and outtype, or dtype if both are the
  same (not very useful for a converter). Otherwise the arguments are parsed to
  find types arguments.
  Types may be specified as allowed by typeCode().
  '''
  intype, outtype, args, kwargs = _parse2TypesInArgs( *args, **kwargs )
  return getattr( aimssip, 'ShallowConverter_' + intype + '_' + outtype ) \
    ( *args )


def TimeSurface( dim=3 ):
  '''same as AimsTimeSurface( dim )'''
  return AimsTimeSurface( dim )


def AimsTimeSurface( dim=3 ):
  '''Create an instance of Aims mesh (AimsTimeSurface_<dim>) from a dimension
  parameter'''
  return getattr( aimssip, 'AimsTimeSurface_' + str( dim ) )()


def AimsThreshold( *args, **kwargs ):
  '''Create a AimsThreshold instance from input and output types. Types may be
  passed as keyword arguments intype and outtype, or dtype if both are the
  same. Otherwise the arguments are parsed to find types arguments.
  Types may be specified as allowed by typeCode().
  '''
  intype, outtype, args, kwargs = _parse2TypesInArgs( *args, **kwargs )
  if intype.startswith( 'Volume_' ):
    intype = intype[ 7: ]
  elif intype.startswith( 'AimsData_' ):
    intype = intype[ 9: ]
  if outtype.startswith( 'Volume_' ):
    outtype = outtype[ 7: ]
  elif outtype.startswith( 'AimsData_' ):
    outtype = outtype[ 9: ]
  return getattr( aimssip, 'AimsThreshold_' + intype + '_' + outtype )( *args )


def AimsVector( *args, **kwargs ):
  '''Create an AimsVector instance from type and dimension arguments. Types may be passed as the keyword argument dtype. Otherwise the arguments are parsed to find types arguments. Dimension should be passed as the keyword argument dim, or is guessed from the input value(s).
  Types may be specified as allowed by typeCode().
  If unspecified, type is guessed from the 1st element of the vector data.
  '''
  dim = kwargs.get( 'dim', None )
  if dim is not None:
    del kwargs[ 'dim' ]
  elif len( args ) == 1 and hasattr( args[0], '__len__' ):
      dim = len( args[0] )
  else:
    dim = len( args )
  try:
    dtype, args, kwargs = _parseTypeInArgs( *args, **kwargs )
  except KeyError, e:
    try:
      if hasattr( args[0], '__len__' ) and len( args[0] ) != 0:
        dtype = _parseTypeInArgs( dtype=args[0][0] )[0]
      else:
        dtype = _parseTypeInArgs( dtype=args[0] )[0]
    except:
      raise e
  return getattr( aimssip, 'AimsVector_' + dtype + '_' + str(dim) )( *args )


def stdVector( *args, **kwargs ):
  '''Create an instance of STL C++ vector (vector_<type>) from a type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.
  Type definitions should match those accepted by typeCode().
  '''
  return _createObject( 'vector', *args, **kwargs )


def stdSet( *args, **kwargs ):
  '''Create an instance of STL C++ set (set_<type>) from a type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.
  Type definitions should match those accepted by typeCode().
  '''
  return _createObject( 'set', *args, **kwargs )


def stdList( *args, **kwargs ):
  '''Create an instance of STL C++ list (list_<type>) from a type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.
  Type definitions should match those accepted by typeCode().
  '''
  return _createObject( 'list', *args, **kwargs )


def rc_ptr( *args, **kwargs ):
  '''Create an instance of aims reference-counting object (rc_ptr_<type>) from a type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.
  Type definitions should match those accepted by typeCode().
  '''
  return _createObject( 'rc_ptr', *args, **kwargs )


# documentation

carto.GenericObject.__doc__ = '''
Generic dynamic polymorphic object.

GenericObject should not be used directly, but rather through the
:py:class:`soma.aims.Object` proxy.

See :py:class:`soma.aims.Object` for a complete documentation.
'''

Object.__doc__ = '''
Generic dynamic polymorphic object proxy.

Object is a proxy and a reference counter to a GenericObject. In most
cases, Object and GenericObject are interchangeable and play the same role,
there are two objects for technical reasons in the C++ layer, but in Python
it whould make no difference.

The C++ generic object is an implementation of a mutable dynamic container
which can hold any type of concrete data, with a generic access API which
abstracts the concrete type of the object stored within it.

According to the underlying object type, Object can behave like a number,
a string, or a container: sequence or dictionary. The python bindings can
therefore support the sequence protocol, or the dictionary protocol, like
a 'real' python object.

The only part which makes it visibly different from a python object is
the conversions with stored objects, which are generally C++ objects
(but can also be any python objects). Transparent conversions are done when
possible.

We will take an example to show how to use it: a volume header. Suppose the
files volume.img/volume.hdr[/volume.img.minf] represent a 3D volume (a MRI
volume, typically, here in Analyze/SPM or Nifti format). We will read it
using aims, and access its header data, which comes as a generic object.

  >>> from soma import aims
  >>> vol = aims.read( 'volume.img' )
  >>> hdr = vol.header()

Now ``vol`` is the volume, and ``hdr`` its header, of type ``Object``.

  >>> type( hdr )
  <class 'soma.aims.Object'>
  >>> print hdr
  { 'voxel_size' : [ 1.875, 1.875, 4, 1 ], 'file_type' : 'SPM',
  'byte_swapping' : 0, 'volume_dimension' : [ 128, 128, 16, 1 ],
  'vox_units' : 'mm  ', 'cal_units' : '', 'data_type' : 'S16',
  'disk_data_type' : 'S16', 'bits_allocated' : 2, 'tr' : 1,
  'minimum' : 0, 'maximum' : 13645, 'scale_factor' : 1,
  'scale_factor_applied' : 0, 'db_name' : '',
  'aux_file' : '', 'orient' : 3, 'generated' : '', 'scannum' : '',
  'patient_id' : '', 'exp_date' : '', 'exp_time' : '', 'views' : 0,
  'start_field' : 32768, 'field_skip' : 8192, 'omax' : 0,
  'omin' : 0, 'smax' : 32, 'smin' : 0, 'SPM_data_type' : '',
  'possible_data_types' : [ 'S16' ], 'spm_radio_convention' : 1,
  'spm_origin' : [ 65, 65, 9 ], 'origin' : [ 64, 63, 7 ] }

``hdr`` prints like a dictionary, but is not really a python dictionary object.
However it behaves like a dictionary:

  >>> print hdr[ 'voxel_size' ]
  [ 1.875, 1.875, 4, 1 ]
  >>> print hdr[ 'data_type' ]
  S16
  >>> print hdr[ 'voxel_size' ][0]
  1.875
  >>> for x in hdr:
  >>>     print x
  >>>
  voxel_size
  file_type
  byte_swapping
  volume_dimension
  vox_units
  cal_units
  data_type
  disk_data_type
  bits_allocated
  tr
  minimum
  maximum
  scale_factor
  scale_factor_applied
  db_name
  aux_file
  orient
  generated
  scannum
  patient_id
  exp_date
  exp_time
  views
  start_field
  field_skip
  omax
  omin
  smax
  smin
  SPM_data_type
  possible_data_types
  spm_radio_convention
  spm_origin
  origin
  >>> for x,y in hdr.items():
  >>>     print x, ':', y
  >>>
  voxel_size : [ 1.875, 1.875, 4, 1 ]
  file_type : 'SPM'
  byte_swapping : 0
  volume_dimension : [ 128, 128, 16, 1 ]
  vox_units : 'mm  '
  cal_units : ''
  data_type : 'S16'
  disk_data_type : 'S16'
  bits_allocated : 2
  tr : 1
  minimum : 0
  maximum : 13645
  scale_factor : 1
  scale_factor_applied : 0
  db_name : ''
  aux_file : ''
  orient : 3
  generated : ''
  scannum : ''
  patient_id : ''
  exp_date : ''
  exp_time : ''
  views : 0
  start_field : 32768
  field_skip : 8192
  omax : 0
  omin : 0
  smax : 32
  smin : 0
  SPM_data_type : ''
  possible_data_types : [ 'S16' ]
  spm_radio_convention : 1
  spm_origin : [ 65, 65, 9 ]
  origin : [ 64, 63, 7 ]

Elements returned by the dictionary queries can have various types, althrough
they are all stored internally in a C++ structure through
:py:class:`soma.aims.Object` wrappers.
But when the underlying type of the element is known and can be retreived as
a python object (either a standard python type or a python binding of a C++
class), it is done automatically. Such conversion is not possible when the
underlying object has no python binding and is a pure C++ object, or when there
is no conversion function. In this case, an Object is returned and contains the
selected data.

Conversions are done using conversion methods that you generally do not
need to call by hand: getScalar(), getString(), or the more general
conversion method :py:meth:`soma.aims.Object.getPython`.
Elements types that cannot be converted to python concrete types are retreived
in their Object container.

The generic conversion function in ``Object``: ``getPython``, is a static
method and can be extended.

Elements in ``Object`` containers can generally be read and written:

  >>> hdr[ 'data_type' ] = 'FLOAT'
  >>> hdr[ 'data_type' ]
  'FLOAT'

This generally suits your needs. But in a few cases there will be a problem in
internal types handling in the C++ layer.

Here, the underlying C++ generic object which did hold a C++ string
(``std::string``) ``'S16'`` is now replaced by another generic object built
from the python string ``'FLOAT'``, and which now wraps an element of type
``PyObject`` (or ``PyString``). It just behaves the same way, so this
operation is perfectly valid, but if C++ programs expect it to be a C++
``std::string``, they may fail.

Moreover, when writing back to existing concrete objects, some additional
conversions may take place: for instance ``hdr['voxel_size']`` is a C++
object of type ``std::vector<float>``, so writing to
``hdr['voxel_size'][0]`` needs to enure we are actually writing a ``float``,
and if not, convert to it if possible.

You can query a type identifier for a generic object via the ``type()``
method:

  >>> hdr.type()
  'PropertySet'

Objects that can be converted from C++ generic objects to python are not necessarily known in advance. A conversion table is kept in the global variable :py:data:`soma.aims.convertersObjectToPython` (in the aims module) and can be extended. Therefore other python modules using aims (such as sigraph) can extend this conversion table.

**Details and tricks**

There are some limitations and "unexpected behaviours" caused by the
underlying C++ implementation of generic objects, and general behaviour
differences between Python and C++. The following is expert-level details,
so read it only if you have problems or you are a C++ expert with good
knowledge of the cartobase C++ library...

**Generic object - specialized object conversions**

Putting a specific object in a generic Object makes a copy of it the
first time it is done, because C++ generic objects contain the
specialized elements. Once this has been done once, generic objects
are shared and not copied anymore, which is consistent with the normal
python behaviour. The non-pythonic thing is the first insertion in a
generic object:

>>> a = aims.vector_FLOAT( [ 12.6, -5.7 ] )
>>> print a
[ 12.6, -5.7 ]
>>> hdr['foo'] = a # here a is copied into hdr['foo']
>>> a.append( 6.8 ) # a is changed but not hdr
>>> print a
[ 12.6, -5.7, 6.8 ]
>>> print hdr['foo']
[ 12.6, -5.7 ]
>>> hdr['dummy'] = hdr['foo']
>>> hdr['foo'].append( 4.2 )
>>> print hdr['dummy'] # this time hdr['dummy'] is changed
[ 12.6, -5.7, 4.2 ]

To overcome the first copy problem, you may have to reassign the initial
variable to the copy instance:

>>> a = aims.vector_FLOAT( [ 1.2, 2.3, 3.4 ] )
>>> hdr['foo'] = a
>>> a = hdr['foo']
>>> print hdr['foo']
[ 1.2, 2.3, 3.4 ]
>>> a[1] = 12.8
>>> print a
[ 1.2, 12.8, 3.4 ]
>>> print hdr['foo']
[ 1.2, 12.8, 3.4 ]

There are exceptions to this behaviour:

- pure python objects, like lists or dictionaries are never copied
  since it is only a pointer to them (the C `PyObject *` pointer) which
  is stored.
- small builtin types: numbers and strings are always copied since they
  are always converted and copied, not wrapped, when passed from python
  to C++ and vice versa.
'''

_volumedoc = '''
The various Volume_<type> classes are bindings to the C++ template classes
carto::Volume<T>. It represents a 4D volume (1D to 4D, actually) storing a
specific type of data: for instance S16 is signed 16 bit short ints, FLOAT is
32 bit floats,
etc.

Volumes are read and written using the :py:class:`soma.aims.Reader` and
:py:class:`soma.aims.Writer` classes, which are in turn used by the simple
:py:func:`soma.aims.read` and :py:func:`soma.aims.write` functions.

A volume of a given type can be built either using its specialized class constructor, or the general Volume() function which can take a voxel type in its arguments: the following are equivalent:

  >>> v = aims.Volume_S16( 100, 100, 10 )
  >>> v = aims.Volume( 'S16', 100, 100, 10 )
  >>> v = aims.Volume( 100, 100, 10, dtype='S16' )
  >>> import numpy
  >>> v = aims.Volume( numpy.int16, 100, 100, 10 )

A volume is an array of voxels, which can be accessed via the ``at()``
method.
For standard numeric types, it is also posisble to get the voxels array as a
numpy_ array, using the ``arraydata()`` method, or more conveniently,
``numpy.array( volume, copy=False )``.
The array returned is a reference to the actual data block, so any
modification to its contents also affect the Volume contents, so it is
generally an easy way of manipulating volume voxels because all the power of
the numpy module can be used on Volumes.
The ``arraydata()`` method returns a numpy array just like it is in memory,
that is a 4D array indexed by ``[t][z][y][x]``, which is generally not what you like and is not consistent with AIMS indexing. Contrarily, using
``numpy.array( volume, copy=False )`` sets strides in the returned numpy
array, so that indexing is in the "normal" order ``[x][y][z][t]``, while still sharing the same memory block.

Volumes also store a header which can contain various information (including
sizes and voxel sizes). The header is a dictionary-like generic object
(:py:class:`soma.aims.Object`) which can be accessed by the ``header()``
method.
This header object also has a read-write access with some restrictions
inherent to the :py:class:`soma.aims.Object` mechanism. It will be saved with
the volume contents when the volume is saved on disk.

Volumes support a number of arithmetic operators like +, - etc. when the
operands types and sizes match: for instance:

  >>> # V1 and V2 are two volumes
  >>> V3 = V1 + V2  # adds two volumes
  >>> V2 -= V1
  >>> V3 = V1 + 3   # adds 3 to aceh voxel
  >>> V3 = V1 * V2  # itemwise multiplication

Some type conversions can be performed on volumes, for istance to
convert a Volume_S16 to a Volume_FLOAT: see the converter classes
``Converter_<type1>_<type2>`` and ``ShallowConverter_<type1>_<type2>``
with <type1> and <type2> being volume types: for instance
:py:class:`soma.aims.Converter_Volume_S16_Volume_FLOAT`.
The converter can also be called using type arguments:

  >>> vol1 = aims.Volume( 'S16', 100, 100, 10 )
  >>> c = aims.Converter( intype=vol1, outtype='Volume_DOUBLE' )
  >>> vol2 = c( vol1 )

.. _numpy: http://numpy.scipy.org/
'''

_aimsdatadoc = '''
``AimsData_<type>`` classes correspond to the python bindings of C++
template classes ``AimsData<T>``. They are planned to be obsolete, and
replaced with the Volume_<type> classes. Try avoiding using them unless
using functions that work on ``AimsData_<type>``.

``AimsData`` actually contains a ``Volume``: you can retreive it using the
``volume()`` method.

Inversely, you can also build a ``Volume`` from a ``AimsData`` by passing it
to the ``Volume`` constructor.

In all cases the voxels memory block is shared.
'''

for x,y in locals().items():
  if x.startswith('Volume_'):
    y.__doc__ = _volumedoc
  elif x.startswith('AimsData_'):
    y.__doc__ = _aimsdatadoc
del x, y

del _volumedoc, _aimsdatadoc

SurfaceManip.__doc__ = """
Surface Manipulation Object. Available Methods are :

- meshMerge(AimsTimeSurface_3, AimsTimeSurface_3) : concatenate two 3D
  meshes into the first. Also the += operator of meshes

- meshTransform(aims.AimsTimeSurface_3 mesh,
  aims.AffineTransformation3d trans) : apply a
  transformation *in-place* to a mesh.

- meshPlanarPolygon

This doc might not be up-to-date, check sources for more info.
"""

SurfaceGenerator.__doc__ = """
Surface Generator Object. Available Methods are :

- cube(center_p, radius_f, smoothnormal_b=false)
- cylinder(p1_p, p2_p, radius1_f, radius2_f, nfacets_i, closed_b, smooth_b=false)
- cone(arrow_p, base_p, radius_f, nfacets_i, closed_b, smooth_b=false)
- arrow(arrow_p, base_p, radius_f, arrowradius_f, nfacets_i, arrowlengthfract_f)
- icosahedron(center_p, radius_f)
- sphere(center_p, radius_f, nfacets_i)
- icosphere(center_p, radius_f, nfacets_i)

With arguments suffix _f:float, _i:int, _b:bool, _p: Point3df or 3-tuple

Alternatively, all those may be called with a GenericObject as only parameter.

This doc might not be up-to-date, use the :py:meth:`printDescription` method
for more info.
"""

private = [ 'private', 'aimssip', 'aimsguisip', 'numpy', 'sip', 'types',
  'weakref' ]
__all__ = []
for x in dir():
  if not x.startswith( '_' ) and x not in private:
    __all__.append(x)
del x, private

