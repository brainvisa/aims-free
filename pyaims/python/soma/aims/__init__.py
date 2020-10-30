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

Since version 4.5.1 both Python 2 (2.6 and higher) and Python 3 (3.4 and higher) are supported.

- organization: `NeuroSpin <http://www.neurospin.org>`_ and former IFR 49

- license: `CeCILL-B <http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html>`_ (a free licence comparable to BSD)

Most of it is a set of direct bindings to the
:aimsdox:`AIMS C++ library <index.html>` API. But a few
classes have been customized or specially written to make easy links to the
python world. See also the small :aimsdata:`AIMS C++ developer doc <dev/index.html>`.

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
  - AimsTimeSurface_<polygons_size>_<texture_type=VOID>: meshes, and the factory
    function :py:func:`AimsTimeSurface`. See for instance
    :py:class:`AimsTimeSurface_3_VOID`.
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
from __future__ import print_function
from __future__ import absolute_import
from six.moves import range
__docformat__ = 'restructuredtext en'


import collections
import functools
import types
import sip
import os
import six
import sys
import numbers

# list of namespace objects that should not be patched to avoid a side effect
# in sip imported namespaces: we must not access their attributes during
# imports.
__namespaces__ = ['soma', 'aims', 'carto', 'anatomist']

# save the original IOError type, it will be replaced by aims bindings
IOError_orig = IOError

# check for share dir, and set the BRAINVISA_SHARE environment var if it is not
# already set
if 'BRAINVISA_SHARE' not in os.environ:
    sharepath = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.dirname(__file__)))), 'share')
    os.environ['BRAINVISA_SHARE'] = sharepath
    os.putenv('BRAINVISA_SHARE', sharepath)  # environ[] and putenv() seem to
    # have slightly different scopes...
del os

try:
    from soma.aims import aimssip
except ImportError as exc:
    if exc.args[0] == ('dynamic module does not define init function '
                       '(initaimssip)'):
        raise ImportError('version mismatch: you are running Python {0} '
                          'but the soma.aims module was likely compiled for '
                          'Python 3.'
                          .format(sys.version.split()[0], sys.api_version))
    elif exc.args[0] == ('dynamic module does not define module export '
                         'function (PyInit_aimssip)'):
        six.raise_from(
            ImportError('version mismatch: you are running Python {0} '
                        'but the soma.aims module was likely compiled for '
                        'Python 2.'
                        .format(sys.version.split()[0], sys.api_version)),
            exc
        )
    raise  # other import errors are re-raised

from soma.importer import ExtendedImporter, GenericHandlers

# Rename sip modules and reorganize namespaces using the Importer class
# this class manages changes that can occur during import of other modules
# for example the import of aimsalgo module modifies the aims module. The
# Importer class manages the rules to apply and applies all rules after each
# module import.
ExtendedImporter().importInModule('', globals(), locals(), 'aimssip')
ExtendedImporter().importInModule(
    '', globals(), locals(), 'aimssip', [],
    [GenericHandlers.removeChildren], ['Reader_', 'Writer_'])
ExtendedImporter().importInModule('', globals(), locals(), 'aimssip',
                                  ['aimssip.aims'])
# move some objects out of carto namespace
Object = carto.Object
Transformation = soma.Transformation
Transformation3d = soma.Transformation3d

del ExtendedImporter

# version string is more or less standard as __version__
__version__ = versionString()

from soma.aims import hierarchy
del hierarchy  # init has been done, get rid of it...
try:
    from soma.aims.spmnormalizationreader import *
except:
    pass  # probably cannot import scipy.io


# restore the original IOError
IOError = IOError_orig
del IOError_orig

# typedefs

try:
    AimsSurfaceTriangle = AimsTimeSurface_3_VOID
    AimsTimeSurface_2 = AimsTimeSurface_2_VOID
    AimsTimeSurface_3 = AimsTimeSurface_3_VOID
    AimsTimeSurface_4 = AimsTimeSurface_4_VOID
    rc_ptr_AimsTimeSurface_2 = rc_ptr_AimsTimeSurface_2_VOID
    rc_ptr_AimsTimeSurface_3 = rc_ptr_AimsTimeSurface_3_VOID
    rc_ptr_AimsTimeSurface_4 = rc_ptr_AimsTimeSurface_4_VOID
except:
    pass  # no surface support

# RCObject constructor / destructor


def RCObject_init(self, *args):
    # print('RCObject init:', self)
    carto.RCObject.__oldinit__(self, *args)
    carto.RCObject._setupRC(self)


def RCObject_del(self):
    # print('RCObject_del', self)
    # Prevent a corner case when an object is created in python side and
    # deleted within C++ side. Thus, the sip wrapper doesn't contain its
    # wrapped C++ object any more, so all deleting works are commited to C++.
    if sip.isdeleted(self):
        # print('already deleted')
        return
    carto.RCObject._releaseRC(self)
    if hasattr(self, '_dontdel'):
        del self._dontdel
    else:
        if getattr(carto.RCObject, '__olddel__', None):
            carto.RCObject.__olddel__(self)

carto.RCObject.__oldinit__ = carto.RCObject.__init__
carto.RCObject.__init__ = RCObject_init
if getattr(carto.RCObject, '__del__', None):
    carto.RCObject.__olddel__ = carto.RCObject.__del__
carto.RCObject.__del__ = RCObject_del

del RCObject_init, RCObject_del

def rgb_str(self):
    return '[%d, %d, %d]' % (self[0], self[1], self[2])

def rgb_repr(self):
    return '<soma.aims.%s> [%d, %d, %d]' % (self.__class__.__name__, self[0],
                                            self[1], self[2])

def rgba_str(self):
    return '[%d, %d, %d, %d]' % (self[0], self[1], self[2], self[3])

def rgba_repr(self):
    return '<soma.aims.%s> [%d, %d, %d, %d]' % (self.__class__.__name__,
                                                self[0], self[1], self[2],
                                                self[3])
AimsRGB.__str__ = rgb_str
AimsRGB.__repr__ = rgb_repr
AimsRGBA.__str__ = rgba_str
AimsRGBA.__repr__ = rgba_repr
del rgb_str, rgba_str, rgb_repr, rgba_repr


# generic Reader class

class Reader(object):

    '''
    Generic reader that can theorerically load any SIP-mapped AIMS or
    Cartograph object. A translation table can be provided to correctly map
    readers and objects.
    For quick and simple operations, you can also use the gloabl
    :py:func:`soma.aims.read` and :py:func:`soma.aims.write` functions, which
    use a Reader object internally.
    '''

    def __init__(self, typemap=None, allocmode=None, options=None):
        '''
        typemap can be provided to specify which reader may
        be used to load objects whose type has been read. A default internal map
        is present but can be replaced.

        The map has 2 modes:

        * object_type : object_type (ex: ``{'Volume' : 'AimsData'}`` )
        * object_type : dict( data_type : full_type )
          (ex: ``'Mesh' : { 'VOID' : 'AimsSurfaceTriangle' }`` )

          A default object_type can be specified if the data_type is not
          found:

          ::

            'Mesh' : { 'VOID' : 'AimsSurfaceTriangle',
                        'default_object_type' : 'AimsTimeSurface_3_VOID' }

          (and this example corresponds to the default internal map if none is
          specified)

        Parameters
        ----------
        typemap: dict (optional)
            described above
        allocmode: (optional)
            :py:class:`carto.AllocatorContext` or
            :py:class:`carto.AllocatorStrategy`.DataAccess constant:
            allocation specification.
        '''
        if typemap is None:
            self._typemap = {  # 'Volume' : 'AimsData',
                'Segments':
                {
                    'VOID': 'AimsTimeSurface_2_VOID',
                    'FLOAT': 'AimsTimeSurface_2_FLOAT',
                    'POINT2DF': 'AimsTimeSurface_2_POINT2DF',
                    'default_object_type': 'AimsTimeSurface_2_VOID',
                 },
                'Mesh':
                {
                    'VOID': 'AimsTimeSurface_3_VOID',
                    'FLOAT': 'AimsTimeSurface_3_FLOAT',
                    'POINT2DF': 'AimsTimeSurface_3_POINT2DF',
                    'default_object_type': 'AimsTimeSurface_3_VOID',
                 },
                'Mesh4':
                {
                    'VOID': 'AimsTimeSurface_4_VOID',
                    'FLOAT': 'AimsTimeSurface_4_FLOAT',
                    'POINT2DF': 'AimsTimeSurface_4_POINT2DF',
                    'default_object_type': 'AimsTimeSurface_4_VOID',
                 },
                'Graph': {'VOID': 'Graph', },
                'AffineTransformation3d':
                {'VOID': 'AffineTransformation3d', },
                'Bucket': 'BucketMap',
                'Texture': 'TimeTexture',
                'Tree': {'hierarchy': 'Hierarchy'},
                'genericobject': {'any': 'Object'},
                #'SparseMatrix': {'DOUBLE': 'SparseMatrix'},
                'SparseMatrix': {'DOUBLE': 'SparseOrDenseMatrix'},
                'SparseOrDenseMatrix': {'DOUBLE': 'SparseOrDenseMatrix'},
            }
        else:
            self._typemap = typemap
        if allocmode is None:
            self.allocmode = None
        elif isinstance(allocmode, carto.AllocatorContext):
            self.allocmode = allocmode
        elif isinstance(allocmode, carto.AllocatorStrategy.DataAccess):
            self.allocmode = carto.AllocatorContext(allocmode)
        else:
            raise TypeError(
                'allocmode argument (2) must be either a '
                'aims.carto.AllocatorContext or a '
                'carto.AllocatorStrategy.DataAccess')
        self.options = options

    def read(self, filename, border=0, frame=-1, dtype=None):
        '''Reads the object contained in the file <filename>, whatever the type
        of the contents of the file. All objects types supported by Aims IO
        system can be read. A border width and a frame number may be specified
        and will be only used by formats that support them.
        If <dtype> is specified, the corresponding object/data type is forced.
        It may be useful to force reading a volume with float voxels for
        instance. It is only supported by a few formats. <dtype> may contain a
        string or a type object, as accepted by :py:func:`soma.aims.typeCode`.
        The read function may follow other object/data types rules, allocators
        and options, as specified in the Reader constructor.
        '''
        f = Finder()
        if not f.check(filename):
            open(filename).close()  # "file not found"-case raising first
            raise IOError(
                'Unknown file format or missing meta-file(s): ' + filename)
        if dtype is not None:
            finaltype = typeCode(dtype)
        else:
            otype = f.objectType()
            dtype = f.dataType()
            otype2 = self._typemap.get(otype, otype)
            if isinstance(otype2, (bytes, str)):
                finaltype = otype2 + '_' + dtype
                otype = otype2
            else:
                finaltype = otype2.get(dtype)
                if finaltype is None:
                    otype2 = otype2.get('default_object_type')
                    if otype2 is not None:
                        finaltype = otype2 + '_' + dtype
                    else:
                        finaltype = otype + '_' + dtype
        rdr = 'Reader_' + finaltype
        r = getattr(aimssip, rdr, None)
        if r is None:
            raise IOError('Unsupported object type: ' + finaltype)
        r = r(filename)
        if self.allocmode is not None:
            r.setAllocatorContext(self.allocmode)
        format = f.format()
        if self.options:
            r.setOptions(self.options)
            # allow forcing format in options
            if 'format' in self.options:
                format = self.options['format']

        result = r.read(border, format, frame)
        return result

    def mapType(self, iotype, aimstype):
        ''' Get the translated type correspondance between identified data type
        (from IO system) and the data type that will actually be read. This is
        generally the identity, but a few IO/types allow some variations.
        '''
        self._typemap[iotype] = aimstype


# generic Writer class

class Writer(object):

    def __init__(self):
        self._objectType = None
        self._dataType = None
        self._fullType = None
        pass

    def write(self, obj, filename, format=None, options={}):
        '''Writes the object <obj> in a file named <filename>, whatever the
        type of <obj>, as format <format>.
        All objects types and formats supported by the Aims IO system can be
        used. <obj> may be a reference-counter to an object type supported by
        the IO system.
        Additional specific options may be passed to the underlying IO system
        in an optional <options> dictionary.
        '''
        c = obj.__class__.__name__.split('.')[-1]
        wr = 'Writer_' + c
        self._objectType = None
        self._dataType = None
        self._fullType = None
        try:
            W = getattr(aimssip, wr)
        except:
            if c.startswith('rc_ptr_'):
                obj = obj._get()
                # build a list of parent classes, so that we can try a more
                # generic writer if the exact type of the object does not have
                # a specific Writer in C++
                tryclass = [obj.__class__]
            else:
                tryclass = list(obj.__class__.__bases__)
            tried = set()
            W = None
            while len(tryclass) != 0:
                ocl = tryclass[0]
                del tryclass[0]
                wr = 'Writer_' + ocl.__name__.split('.')[-1]
                W = getattr(aimssip, wr, None)
                if W is None:
                    tried.add(ocl)
                    tryclass += [x for x in ocl.__bases__ if x not in tried]
                else:
                    break
            if W is None:
                raise AttributeError('no Writer for type ' +
                                     obj.__class__.__name__)
        w = W(filename, options)
        w.write(obj, False, format)
        try:
            self._objectType = w.writtenObjectType()
            self._dataType = w.writtenObjectDataType()
            self._fullType = w.writtenObjectFullType()
        except:
            raise  # 'Unsupported object type: ' + obj.__class__.__name__

    def writtenObjectType(self):
        ''' Data object type (Volume, Mesh...) written (available after writing)
        '''
        return self._objectType

    def writtenObjectDataType(self):
        ''' Data content type (voxel or texture type...) written (available
        after writing)
        '''
        return self._dataType

    def writtenObjectFullType(self):
        ''' Data full type written (available after writing)
        '''
        return self._fullType


# simple IO functions

def read(filename, border=0, frame=-1, dtype=None, allocmode=None,
         options=None):
    '''Equivalent to:

    .. code-block:: python

      r = Reader( allocmode=allocmode, options=options )
      return r.read( filename, border=border, frame=frame, dtype=dtype )
    '''
    r = Reader(allocmode=allocmode, options=options)
    return r.read(filename, border=border, frame=frame, dtype=dtype)


def write(obj, filename, format=None, options={}):
    '''Equivalent to:

    .. code-block:: python

      w = Writer()
      w.write( obj, filename, format=format, options=options )
    '''
    w = Writer()
    w.write(obj, filename, format=format, options=options)


# vector-like iterator

class VecIter(object):

    '''iterator class for some aims containers (AimsVector)'''

    def __init__(self, vec):
        self._vector = vec
        self._index = 0

    def __iter__(self):
        return self

    def __next__(self):
        if self._index >= len(self._vector):
            raise StopIteration('iterator outside bounds')
        val = self._vector[self._index]
        self._index += 1
        return val

    next = __next__  # Python 2 compatibility

# Iterator (doesn't work when implemented in SIP so far)


def newiter(self):
    return self.__objiter__()


def newnext(self):
    return self.__objnext__()


def objiter(self):
    return self._get().__iter__()


def objnext(self):
    return self._get().__next__()


def objiteritems(self):
    class iterator(object):

        def __init__(self, it):
            self.iterator = it

        def __iter__(self):
            return self

        def __next__(self):
            if not self.iterator.isValid():
                raise StopIteration("iterator outside bounds")
            try:
                key = self.iterator.key()
            except:
                try:
                    key = self.iterator.intKey()
                except:
                    key = self.iterator.keyObject()
            res = (key, self.iterator.currentValue())
            next(self.iterator)
            return res

        next = __next__  # Python 2 compatibility

    return iterator(self.objectIterator())


def objitervalues(self):
    class iterator(object):

        def __init__(self, it):
            self.iterator = it

        def __iter__(self):
            return self

        def __next__(self):
            if not self.iterator.isValid():
                raise StopIteration("iterator outside bounds")
            res = self.iterator.currentValue()
            next(self.iterator)
            return res

        next = __next__  # Python 2 compatibility

    return iterator(self.objectIterator())


class BckIter(object):

    '''iterator class for bucket containers'''

    def __init__(self, bucket):
        self._bck = bucket
        self._iter = None

    def __iter__(self):
        return self

    def __next__(self):
        if self._iter is None:
            self._iter = iter(self._bck.keys())
        elem = next(self._iter)
        return self._bck[elem]

    next = __next__  # Python 2 compatibility


class BckIterItem(object):

    '''item iterator class for bucket containers'''

    def __init__(self, bucket):
        self._bck = bucket
        self._iter = None

    def __iter__(self):
        return self

    def __next__(self):
        if self._iter is None:
            self._iter = iter(self._bck.keys())
        elem = next(self._iter)
        return elem, self._bck[elem]

    next = __next__  # Python 2 compatibility

# automatic rc_ptr dereferencing


def proxygetattr(self, attr):
    try:
        return self.__getattribute__(attr)
    except:
        if self.__getattribute__('isNull')():
            raise RuntimeError('Null object called')
        return getattr(self._get(), attr)


def proxylen(self):
    if self.isNull():
        raise TypeError("object of type 'Object' has no len()")
    return self._get().__len__()


def proxygetitem(self, attr):
    if self.isNull():
        raise RuntimeError('Null object called')
    return self._get().__getitem__(attr)


def proxysetitem(self, attr, value):
    try:
        return __setitem__(self, attr, value)
    except:
        return self._get().__setitem__(attr, value)


def proxydelitem(self, attr):
    try:
        return self.__delitem__(attr)
    except:
        if self.isNull():
            raise RuntimeError('Null object called')
        return self._get().__delitem__(attr)


def proxynonzero(self):
    if self.isNull():
        return False
    try:
        return self._get().__nonzero__()
    except Exception:
        try:
            return len(self._get()) != 0
        except Exception:
            try:
                if self == 0:
                    return False
            except Exception:
                return True


def proxystr(self):
    if self.isNull():
        return 'None'
    return self._get().__str__()


def proxyiter(self):
    if self.isNull():
        raise AttributeError("'%s' object has no attributes '__iter__'"
                             % self.__class__)
    return self._get().__iter__()


def rcptr_getAttributeNames(self):
    '''IPython completion feature...'''
    m = []
    l = [self._get(), self]
    done = set()
    while l:
        c = l.pop()
        done.add(c)
        m += [x for x in list(c.__dict__.keys()) if not x.startswith('_') and x not in m]
        cl = getattr(c, '__bases__', None)
        if not cl:
            cl = getattr(c, '__class__', None)
            if cl is None:
                continue
            else:
                cl = [cl]
        l += [x for x in cl if x not in done]
    return m


def __getitem_vec__(self, s):
    if isinstance(s, slice):
        start, stop, step = s.indices(len(self))
        return [self.__oldgetitem__(x) for x in range(start, stop, step)]
    else:
        return self.__oldgetitem__(s)


def __setitem_vec__(self, s, val):
    if isinstance(s, slice):
        start, stop, step = s.indices(len(self))
        return [self.__oldsetitem__(x, v) for x, v in
                zip(range(start, stop, step), val)]
    else:
        return self.__oldsetitem__(s, val)


if six.PY34:
    def __operator_overload__(self, op, other):
        try:
            return op(self, other)
        except TypeError:
            # this exception handling is here to follow the behaviour of normal
            # operators: if a.__add__(b) does not support the operand type of
            # b, it should return NotImplemented, and should not raise an
            # exception. In that case, python can call b.__radd__(a), which it
            # doesn't do if an exception is raised.
            return NotImplemented

else:
    def __operator_overload__(op, self, other):
        try:
            return op(self, other)
        except TypeError:
            # this exception handling is here to follow the behaviour of normal
            # operators: if a.__add__(b) does not support the operand type of
            # b, it should return NotImplemented, and should not raise an
            # exception. In that case, python can call b.__radd__(a), which it
            # doesn't do if an exception is raised.
            return NotImplemented


def __vol_pow__(self, y, z=None):
    ''' Power operation: x**y, pow(x, y [,z]) is performed using numpy.
    '''
    vol = aims.Volume(numpy.asarray(self).__pow__(y, z))
    vol.copyHeaderFrom(self.header())
    return vol


def __vol_ipow__(self, y):
    ''' Power operation: x **= y is performed using numpy.
    '''
    a = numpy.asarray(self)
    a **= y
    return self


def __vol_floordiv__(self, y):
    ''' Power operation: x // y is performed using numpy.
    '''
    vol = aims.Volume(numpy.asarray(self).__floordiv__(y))
    vol.copyHeaderFrom(self.header())
    return vol


def __vol_ifloordiv__(self, y):
    ''' Power operation: x //= y is performed using numpy.
    '''
    a = numpy.asarray(self)
    a //= y
    return self


def __Volume_astype__(self, dtype, copy=False):
    '''Easy conversion method for volumes. Works in a similar was as numpy
    arrays astype() methods, except that the "copy" parameter defaults to
    False.

    Parameters
    ----------
    dtype: string or type object
        may be a volume or voxel type, specified either as a type oject
        (int, aims.Volume_S32, numpy.int16), or as a string ("S16"...).
    copy: bool
        if False, a ShallowConverter will be used: if dtype matches the
        current volume type, a shared reference to self will be returned.
        Otherwise a new copy will be returned.

    Returns
    -------
    A volume of the converted type
    '''
    if isinstance(dtype, six.string_types):
        if dtype.startswith('Volume_'):
            dtype = typeCode(dtype[7:])
        else:
            dtype = typeCode(dtype)
    elif dtype.__name__.startswith('Volume_'):
        dtype = typeCode(dtype.__name__[7:])
    else:
        dtype = typeCode(dtype)
    outtype = 'Volume_%s' % dtype
    try:
        if not copy:
            conv = ShallowConverter(intype=self, outtype=outtype)
        else:
            conv = Converter(intype=self, outtype=outtype)
    except AttributeError:
        raise TypeError('No conversion to type %s' % outtype)
    return conv(self)


def proxyget(self, *args, **kwargs):
    '''get() is ambiguous:
    rc_ptr / Object has the get() method, providing access to the underlying
    object.
    GenericObject has the method get(key, default=None) as a dictionary-like
    object.
    This proxy calls the right one depending on arguments.
    '''
    if len(args) == 0 and len(kwargs) == 0:
        # no arguments, it's really the rc_ptr.get() method
        return self._get()
    return self._get().get(*args, **kwargs)


def _vol_getstate(self):
    return (eval(repr(self.header())), numpy.asarray(self))


def _vol_setstate(self, state):
    self.__init__(state[1].shape)
    self.copyHeaderFrom(state[0])
    numpy.asarray(self)[:] = state[1][:]


def _aimsdata_getstate(self):
    return self.volume().__getstate__()


def _aimsdata_setstate(self, state):
    vol = Volume(state[1].shape)
    vol.copyHeaderFrom(state[0])
    numpy.asarray(vol)[:] = state[1][:]
    self.__init__(vol)


# scan classes and modify some of them
def __fixsipclasses__(classes):
    '''Fix some classes methods which Sip doesn't correctly bind'''
    import sip
    for x, y in classes:
        if not isinstance(y, six.class_types):
            continue
        try:
            try:
                name = object.__getattribute__(y, '__name__')
            except AttributeError:
                # not a named class
                continue

            # we have to filter out namespace objects. I don't know how to
            # detect them.
            if name in __namespaces__:
                continue

            if name.startswith('rc_ptr_') \
                    or name.startswith('weak_shared_ptr_') \
                        or name.startswith('weak_ptr_'):
                # add __getattr__ method
                sip.wrappertype.__setattr__(y, '__len__',
                                            __fixsipclasses__.proxylen)
                sip.wrappertype.__setattr__(y, '__getattr__',
                                            __fixsipclasses__.proxygetattr)
                sip.wrappertype.__setattr__(y, '__getitem__',
                                            __fixsipclasses__.proxygetitem)
                # y.__setitem__ = __fixsipclasses__.proxysetitem
                sip.wrappertype.__setattr__(y, '__delitem__',
                                            __fixsipclasses__.proxydelitem)
                #y.__str__ = __fixsipclasses__.proxystr
                sip.wrappertype.__setattr__(y, '__nonzero__',
                                            __fixsipclasses__.proxynonzero)
                sip.wrappertype.__setattr__(y, '__bool__',
                                            __fixsipclasses__.proxynonzero)
                sip.wrappertype.__setattr__(
                    y, '_getAttributeNames',
                    __fixsipclasses__.getAttributeNames)
                # to maintain compatibiity with pyaims 3.x
                sip.wrappertype.__setattr__(y, 'get',
                                            __fixsipclasses__.proxyget)
                sip.wrappertype.__setattr__(y, '__iter__',
                                            __fixsipclasses__.proxyiter)

            elif name.startswith('ShallowConverter_'):
                sip.wrappertype.__setattr__(y, '__oldcall__', y.__call__)
                sip.wrappertype.__setattr__(
                    y, '__call__',
                    lambda self, obj: self.__oldcall__(obj)._get())

            elif name.startswith('Volume_'):
                for op in ('add', 'iadd', 'radd', 'sub', 'isub', 'rsub',
                           'mul', 'imul', 'rmul', 'div', 'idiv', 'rdiv',
                           'mod', 'imod', 'rmod', 'and', 'iand', 'rand',
                           'or', 'ior', 'ror', 'xor', 'ixor' 'rxor'):
                    try:
                        add = object.__getattribute__(
                            aimssip, '__%s_%s__' % (op, y.__name__))
                    except AttributeError:
                        add = None
                    if add is not None:
                        if six.PY34:
                            sip.wrappertype.__setattr__(
                                y, '__%s__' % op, functools.partialmethod(
                                    __fixsipclasses__.__operator_overload__,
                                    add))
                        else:
                            sip.wrappertype.__setattr__(
                                y, '__%s__' % op,
                                types.MethodType(
                                    functools.partial(
                                      __fixsipclasses__.__operator_overload__,
                                        add),
                                    None, y))
                # pow, ipow, floordiv, ifloordiv
                sip.wrappertype.__setattr__(y, '__pow__',
                                            __fixsipclasses__.__vol_pow__)
                sip.wrappertype.__setattr__(y, '__ipow__',
                                            __fixsipclasses__.__vol_ipow__)
                sip.wrappertype.__setattr__(y, '__floordiv__',
                                            __fixsipclasses__.__vol_floordiv__)
                sip.wrappertype.__setattr__(
                    y, '__ifloordiv__', __fixsipclasses__.__vol_ifloordiv__)
                # volume item access
                sip.wrappertype.__setattr__(
                    y, '__getitem__',
                    lambda self, *args, **kwargs:
                        numpy.asarray(self).__getitem__(*args, **kwargs))
                sip.wrappertype.__setattr__(
                    y, '__setitem__',
                    lambda self, *args, **kwargs:
                        numpy.asarray(self).__setitem__(*args, **kwargs))
                if not six.PY2:
                    sip.wrappertype.__setattr__(
                        y, 'astype', __fixsipclasses__.__Volume_astype__)
                else:
                    sip.wrappertype.__setattr__(
                        y, 'astype',
                        types.MethodType(
                            __fixsipclasses__.__Volume_astype__, None, y))
                # volume pickling
                sip.wrappertype.__setattr__(y, '__getstate__',
                                            __fixsipclasses__._vol_getstate)
                sip.wrappertype.__setattr__(y, '__setstate__',
                                            __fixsipclasses__._vol_setstate)
                sip.wrappertype.__setattr__(
                    y, 'shape',
                    property(lambda self: numpy.asarray(self).shape, None,
                             None))
            else:
                if hasattr(y, '__objiter__'):
                    y.__iter__ = __fixsipclasses__.newiter
                try:
                    z = object.__getattribute__(y, '__objnext__')
                    sip.wrappertype.__setattr__(y, '__next__',
                                                __fixsipclasses__.newnext)
                    if six.PY2:
                        sip.wrappertype.__setattr__(y, 'next',
                                                    __fixsipclasses__.newnext)
                except AttributeError:
                    pass

                if name.startswith('AimsVector_') \
                        or name.startswith('Texture_') \
                        or name in ('AimsRGB', 'AimsRGBA', 'AimsHSV'):
                    sip.wrappertype.__setattr__(y, '__iterclass__',
                                                VecIter)
                    sip.wrappertype.__setattr__(
                        y, '__iter__',
                        lambda self: self.__iterclass__(self))

                if (name.startswith('vector_') \
                        or name.startswith('AimsVector_')) \
                        and 'iterator' not in name:
                    sip.wrappertype.__setattr__(
                        y, '__oldgetitem__',
                        object.__getattribute__(y, '__getitem__'))
                    sip.wrappertype.__setattr__(
                        y, '__getitem__', __fixsipclasses__.__getitem_vec__)
                    sip.wrappertype.__setattr__(
                        y, '__oldsetitem__',
                        object.__getattribute__(y, '__setitem__'))
                    sip.wrappertype.__setattr__(
                        y, '__setitem__', __fixsipclasses__.__setitem_vec__)
                continue
                if name.startswith('BucketMap_') \
                        or name.startswith('TimeTexture_'):
                    sip.wrappertype.__setattr__(y, '__iterclass__', BckIter)
                    sip.wrappertype.__setattr__(y, '__iteritemclass__',
                                                BckIterItem)
                    sip.wrappertype.__setattr__(
                        y, '__iter__', lambda self: self.__iterclass__(self))
                    sip.wrappertype.__setattr__(
                        y, 'iteritems',
                        lambda self: self.__iteritemclass__(self))
                    if not six.PY2:
                        sip.wrappertype.__setattr__(
                            y, 'items',
                            lambda self: self.__iteritemclass__(self))
                    if name.startswith('BucketMap_'):
                        sip.wrappertype.__setattr__(
                            object.__getattribute__(y, 'Bucket'),
                            '__iterclass__', BckIter)
                        sip.wrappertype.__setattr__(
                            object.__getattribute__(y, 'Bucket'),
                            '__iteritemclass__', BckIterItem)
                        sip.wrappertype.__setattr__(
                            object.__getattribute__(y, 'Bucket'), '__iter__',
                            lambda self: self.__iterclass__(self))
                        sip.wrappertype.__setattr__(
                            object.__getattribute__(y, 'Bucket'), 'iteritems',
                            lambda self: self.__iteritemclass__(self))
                        if not six.PY2:
                            sip.wrappertype.__setattr__(
                                object.__getattribute__(y, 'Bucket'), 'items',
                                lambda self: self.__iteritemclass__(self))
            if name.startswith('AimsData_'):
                # volume item access
                sip.wrappertype.__setattr__(
                    y, '__getitem__',
                    lambda self, *args, **kwargs: \
                        numpy.asarray(self.volume()).__getitem__(*args,
                                                                 **kwargs))
                sip.wrappertype.__setattr__(
                    y, '__setitem__',
                    lambda self, *args, **kwargs: \
                        numpy.asarray(self.volume()).__setitem__(*args,
                                                                 **kwargs))
                sip.wrappertype.__setattr__(
                    y, '__getstate__', __fixsipclasses__._aimsdata_getstate)
                sip.wrappertype.__setattr__(
                    y, '__setstate__',  __fixsipclasses__._aimsdata_setstate)
            try:
                z = object.__getattribute__(y, 'next')
                z = object.__getattribute__(y, '__iter__')
                try:
                    z = object.__getattribute__(y, '__next__')
                    import warnings
                    warnings.warn('{0!r} looks like an iterator implementing '
                                  'the Python 2 next() method, it will not '
                                  'work as an iterator under '
                                  'Python 3'.format(y),
                                  DeprecationWarning)
                except AttributeError:
                    pass
            except AttributeError:
                pass
        except Exception as e:
            print('warning: exception during classes patching:', e, ' for:', y)
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
__fixsipclasses__.proxyiter = proxyiter
__fixsipclasses__.__operator_overload__ = __operator_overload__
__fixsipclasses__.__Volume_astype__ = __Volume_astype__
__fixsipclasses__.proxyget = proxyget
__fixsipclasses__._vol_getstate = _vol_getstate
__fixsipclasses__._vol_setstate = _vol_setstate
__fixsipclasses__._aimsdata_getstate = _aimsdata_getstate
__fixsipclasses__._aimsdata_setstate = _aimsdata_setstate
__fixsipclasses__.__vol_pow__ = __vol_pow__
__fixsipclasses__.__vol_ipow__ = __vol_ipow__
__fixsipclasses__.__vol_floordiv__ = __vol_floordiv__
__fixsipclasses__.__vol_ifloordiv__ = __vol_ifloordiv__
del newiter, newnext, objiter, objnext, proxygetattr, proxylen
del proxygetitem, proxysetitem, proxystr, proxynonzero
del rcptr_getAttributeNames
del __getitem_vec__, __setitem_vec__, __operator_overload__, __Volume_astype__
del proxyget
del _vol_getstate, _vol_setstate, _aimsdata_getstate, _aimsdata_setstate
del __vol_pow__, __vol_ipow__, __vol_floordiv__, __vol_ifloordiv__

__fixsipclasses__(list(globals().items()) + list(carto.__dict__.items()))

Object.__iter__ = __fixsipclasses__.objiter
Object.__next__ = __fixsipclasses__.objnext
if six.PY2:
    Object.next = __fixsipclasses__.objnext
Object.__delitem__ = __fixsipclasses__.proxydelitem
Object._getAttributeNames = __fixsipclasses__.getAttributeNames

del BckIter, BckIterItem, VecIter

__fixsipclasses__.fakerepr = lambda a: "<%s.%s object at 0x%x>" % (
    a.__class__.__module__, a.__class__.__name__, id(a))

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
    = Point3d.__repr__ = Point3du.__repr__ = Point3df.__repr__ \
    = Point3dd.__repr__ = Point2d.__repr__ = Point2du.__repr__ \
    = Point2df.__repr__ = Point2dd.__repr__ \
    = lambda self: __fixsipclasses__.fakerepr(self) + "\n" + str(list(self.items()))


import numpy


def __toMatrix(s):
    """ This function return a copy of the transformation matrix """
    return numpy.asarray(s.affine())[:, :, 0, 0]


def __AffineTransformation3dFromMatrix(self, value):
    if value.shape[0] == 3:
        numpy.asarray(self.affine())[:3, :, 0, 0] = value
    else:
        numpy.asarray(self.affine())[:, :, 0, 0] = value
    #self.rotation().volume().arraydata().reshape(3, 3).transpose()[:, :] \
        #= value[0:3, 0:3]
    #self.translation().arraydata()[:] = value[0:3, 3].flatten()


def __AffineTransformation3d__init__(self, *args):
    if len( args ) != 0 and isinstance( args[0], numpy.ndarray ) \
            and args[0].shape == (4, 4):
        self.__oldinit__()
        self.fromMatrix(args[0])
    else:
        self.__oldinit__(*args)


def __AffineTransformation3d__header(self):
    h = AffineTransformation3d.__oldheader__(self)
    h.__motion = self
    return h


def __AffineTransformation3d__affine(self):
    a = self.__oldaffine__(self)._get()
    # get a ref to self into a to prevent deletion of self
    # warning: setting it in the refvolume is not working since the python part
    # of the Volume object is not preserved when only manipulating rc_ptrs
    a._trans = self
    return a


AffineTransformation3d.toMatrix = __toMatrix
AffineTransformation3d.fromMatrix = __AffineTransformation3dFromMatrix
AffineTransformation3d.__oldinit__ = AffineTransformation3d.__init__
AffineTransformation3d.__init__ = __AffineTransformation3d__init__
AffineTransformation3d.__oldheader__ = AffineTransformation3d.header
AffineTransformation3d.header = __AffineTransformation3d__header
AffineTransformation3d.__oldaffine__ = AffineTransformation3d.affine
AffineTransformation3d.affine = __AffineTransformation3d__affine
del __toMatrix, __AffineTransformation3dFromMatrix, \
    __AffineTransformation3d__init__, __AffineTransformation3d__header, \
    __AffineTransformation3d__affine
# backward compatibility
Motion = AffineTransformation3d

AffineTransformation3d.__repr__ = lambda self: __fixsipclasses__.fakerepr(
    self) + "\n" + str(self.toMatrix())
AffineTransformation3d.__str__ = lambda self: self.toMatrix().__str__()

# This one add support for += in 3D mesh
AimsTimeSurface_2_VOID.__iadd__ = lambda s, o: SurfaceManip.meshMerge(s, o) or s
AimsTimeSurface_3_VOID.__iadd__ = lambda s, o: SurfaceManip.meshMerge(s, o) or s
AimsTimeSurface_3_VOID.__iadd__ = lambda s, o: SurfaceManip.meshMerge(s, o) or s

# conversions from GenericObject
convertersObjectToPython = {
    'PyObject': carto.PyObjectfromObject,
    'rc_ptr of bucket of VOID': BucketMap_VOID.fromObject,
    'rc_ptr of bucket of S16': BucketMap_S16.fromObject,
    'rc_ptr of bucket of U16': BucketMap_U16.fromObject,
    'rc_ptr of bucket of S32': BucketMap_S32.fromObject,
    'rc_ptr of bucket of U32': BucketMap_U32.fromObject,
    'rc_ptr of bucket of FLOAT': BucketMap_FLOAT.fromObject,
    'rc_ptr of bucket of DOUBLE': BucketMap_DOUBLE.fromObject,
    'rc_ptr of Mesh of VOID': AimsSurfaceTriangle.fromObject,
    'rc_ptr of mesh of VOID': AimsSurfaceTriangle.fromObject, # obsolete
    'rc_ptr of Mesh4 of VOID': AimsTimeSurface_4_VOID.fromObject,
    'rc_ptr of Segments of VOID': AimsTimeSurface_2_VOID.fromObject,
    'rc_ptr of Mesh of FLOAT': AimsTimeSurface_3_FLOAT.fromObject,
    'rc_ptr of Mesh4 of FLOAT': AimsTimeSurface_4_FLOAT.fromObject,
    'rc_ptr of Segments of FLOAT': AimsTimeSurface_2_FLOAT.fromObject,
    'rc_ptr of Mesh of POINT2DF': AimsTimeSurface_3_POINT2DF.fromObject,
    'rc_ptr of Mesh4 of POINT2DF': AimsTimeSurface_4_POINT2DF.fromObject,
    'rc_ptr of Segments of POINT2DF': AimsTimeSurface_2_POINT2DF.fromObject,
    'rc_ptr of texture of S16': TimeTexture_S16.fromObject,
    'rc_ptr of texture of S32': TimeTexture_S32.fromObject,
    'rc_ptr of texture of U32': TimeTexture_U32.fromObject,
    'rc_ptr of texture of FLOAT': TimeTexture_FLOAT.fromObject,
    'rc_ptr of texture of POINT2DF': TimeTexture_POINT2DF.fromObject,
    'POINT2DF': Point2df.fromObject,
    'POINT3DF': Point3df.fromObject,
    'POINT4DF': Point4df.fromObject,
    'S16': carto.NumericGenericObjectConverter.asInt,
    'U16': carto.NumericGenericObjectConverter.asInt,
    'S32': carto.NumericGenericObjectConverter.asInt,
    'U32': carto.NumericGenericObjectConverter.asInt,
    'boolean': carto.NumericGenericObjectConverter.asBool,
    'string': lambda x: x.getString(),
}
''' Conversion function map. These converters are used to convert between a C++ object inside a generic object to their Python equivalent. They are typically used when accessing a sub-object:

>>> from soma import aims
>>> v = aims.Object(aims.vector_STRING())
>>> v.append('toto')
>>> v.append('bubu')
>>> type(v)
soma.aims.Object
>>> v.type()
'vector of string'
>>> v[0]
'toto'
>>> type(v[0])
str

In this example, v contains a C++ object of type std::vector<std::string>. Its elements are accessed via a wrapping which is using a C++ generic object (carto::Object). But in python we obtain a str (pyton string): the string has been taken out of the generic object using this conversion map (and then converted automatically to str by the SIP tool).
'''


def getPython(self):
    """
    Conversion to python types: extracts what is in the Object (when
    possible). The global dictionary
    :py:data:`soma.aims.convertersObjectToPython` stores
    converters
    """
    if self.isNone():
        return None
    t = self.type()
    cv = convertersObjectToPython.get(t)
    res = None
    gen = self._get()
    if cv is not None:
        res = cv(gen)
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
    if t.find('volume') >= 0:
        dt = t.split()[-1]
        try:
            res = eval('AimsData_' + dt).fromObject(gen)
            res.__genericobject__ = gen
            return res
        except:
            pass
    t2 = str(t)
    dtv = ''
    while t2.startswith('vector of '):
        dtv = dtv + 'vector_'
        t2 = t2[10:]
    if dtv != '':
        try:
            vectype = eval(dtv + t2)
            res = vectype.fromObject(gen)
            res.__genericobject__ = gen
            return res
        except:
            try:
                vectype = eval(dtv + t2.upper())
                res = vectype.fromObject(gen)
                res.__genericobject__ = gen
                return res
            except:
                pass

    if t.startswith('VECTOR_OF_'):
        try:
            dt = t[10:]
            vectype = eval('AimsVector_' + dt)
            res = vectype.fromObject(gen)
            res.__genericobject__ = gen
            return res
        except:
            pass

    raise RuntimeError('No conversion to python object possible from type '
                       + t)


def toObject(x):
    if hasattr(x, 'toObject'):
        return x.toObject()
    return Object(x)


def ptrToObject(x):
    if hasattr(x, 'ptrToObject'):
        return x.ptrToObject()
    return Object(x)


def rcToObject(x):
    if hasattr(x, 'rcToObject'):
        return x.rcToObject()
    return Object(x)


Object.__len__ = __fixsipclasses__.proxylen
Object.__getattr__ = __fixsipclasses__.proxygetattr
Object.__getitem__ = __fixsipclasses__.proxygetitem
Object.__setitem__ = __fixsipclasses__.proxysetitem
Object.__str__ = __fixsipclasses__.proxystr
Object.__repr__ = Object.__str__
Object.getPython = getPython
Object.toObject = staticmethod(toObject)
Object.ptrToObject = staticmethod(ptrToObject)
Object.rcToObject = staticmethod(rcToObject)

del getPython
del toObject, ptrToObject, rcToObject


# customize GenericObject to get automatic conversions between
# Objects and concrete types
class _proxy(object):

    def retvalue(x):
        if callable(x):
            if isinstance(x, carto.GenericObject._proxy):
                return x
            return carto.GenericObject._proxy(x)
        if isinstance(x, Object):
            try:
                return x.getPython()
            except:
                return x
        elif isinstance(x, carto.GenericObject):
            try:
                return Object.getPython(x)
            except:
                return Object(x)
        return x
    retvalue = staticmethod(retvalue)

    def __init__(self, x):
        if isinstance(x, carto.GenericObject._proxy):
            self._x = x._x
        else:
            self._x = x

    def __call__(self, *args, **kwargs):
        r = self._x
        obj = getattr(r, '__self__', None)
        if obj is not None:
            # determine if we were called recursively.
            # We need to detect this because SIP methods may call the proxy and
            # keep no other access to the C method
            import traceback
            sf = traceback.extract_stack()[-2]
            if sf[-2] == '__call__' and \
                sf[-1] == 'return carto.GenericObject._proxy.retvalue(' \
                    'r(*args, **kwargs))':
                # compare sf[0] and __file__, after removing .py/.pyc extension
                # which may make the comparison not to match
                f1 = sf[0]
                if f1.endswith('.py'):
                    f1 = f1[:-3]
                elif f1.endswith('.pyc'):
                    f1 = f1[:-4]
                f2 = __file__
                if f2.endswith('.py'):
                    f2 = f2[:-3]
                elif f2.endswith('.pyc'):
                    f2 = f2[:-4]
                if f1 == f2:
                    # try to call with selfWasArg=True (class unound method)
                    cl = object.__getattribute__(obj, '__class__')
                    return getattr(cl, r.__name__)(obj, *args, **kwargs)
        return carto.GenericObject._proxy.retvalue(r(*args, **kwargs))


def genobj__getattribute__(self, attr):
    ga = object.__getattribute__(self, '__oldgetattribute__')
    x = ga(attr)
    if(attr.startswith('__') or attr == '_proxy') and attr != '__next__':
        return x
    return ga('_proxy').retvalue(x)


def genobj__getitem__(self, item):
    try:
        x = Object.getPython(self)
        return x.__getitem__(item)
    except:
        return carto.GenericObject._proxy.retvalue(
            self.__oldgetitem__(item))


def genobj__new__(cls, *args, **kwargs):
    if cls != carto.GenericObject \
            and cls.__getitem__ != carto.GenericObject.__getitem__:
        # modify __getitem__ on subclasses of GenericObject
        cls.__oldgetitem__ = cls.__getitem__
        cls.__getitem__ = carto.GenericObject.__getitem__
        if cls.__getattribute__ != carto.GenericObject.__getattribute__:
            cls.__oldgetattribute__ = cls.__getattribute__
            cls.__getattribute__ = carto.GenericObject.__getattribute__
    return carto.GenericObject.__oldnew__(cls, *args, **kwargs)


def genobj__update__(self, x):
    if not self.isDictionary():
        raise ValueError(
            'Generic object is not a dictionary-compatible object')
    for x, y in six.iteritems(x):
        self[x] = y


def genobj__iadd__(self, x):
    if not self.isDynArray():
        raise ValueError('Generic object is not a list-compatible object')
    for y in x:
        self.append(y)
    return self


def obj__iadd__(self, x):
    self._get().__iadd__(x)
    return self

carto.GenericObject.__repr__ = carto.GenericObject.__str__
carto.GenericObject._proxy = _proxy
del _proxy
carto.GenericObject.__oldgetitem__ = carto.GenericObject.__getitem__
carto.GenericObject.__getitem__ = genobj__getitem__
carto.GenericObject.__oldnew__ = carto.GenericObject.__new__
carto.GenericObject.__new__ = staticmethod(genobj__new__)
carto.GenericObject.__oldgetattribute__ = carto.GenericObject.__getattribute__
carto.GenericObject.__getattribute__ = genobj__getattribute__
carto.GenericObject.update = genobj__update__
carto.GenericObject.__iadd__ = genobj__iadd__
carto.GenericObject.iteritems = objiteritems
if not six.PY2:
    carto.GenericObject.items = objiteritems
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
import weakref
Point3df.__refParent = weakref.ref(Point3df)
Point3df.__oldgetattr__ = Point3df.__getattribute__


def __getattribute__(self, name):
    g = object.__getattribute__(self, "__oldgetattr__")
    if g("__refParent")():
        return g(name)
    else:
        raise RuntimeError("Underlying C++ object has been deleted")
Point3df.__getattribute__ = __getattribute__
del __getattribute__

# complement Quaternion
Quaternion.compose = Quaternion.__mul__

# templates / types helpers


def typeCode(data):
    '''returns the AIMS type code for the given input data. data may be a string
    code, a python/numpy numeric type, an AIMS type, or an instance of such a
    type.
    '''
    dmap = {'int': 'S32', 'int32': 'S32', 'uint': 'U32', 'uint32': 'U32',
            'int64': 'S64', 'uint64': 'U64', 'int16': 'S16', 'uint16': 'U16',
            'char': 'S8', 'uchar': 'U8', 'float': 'DOUBLE', 'float32': 'FLOAT',
            'float64': 'DOUBLE',
            numpy.int8: 'S8', numpy.uint8: 'U8',
            numpy.int16: 'S16', numpy.uint16: 'U16',
            numpy.int32: 'S32', numpy.uint32: 'U32',
            numpy.int64: 'S64', numpy.uint64: 'U64',
            numpy.float32: 'FLOAT', numpy.float64: 'DOUBLE',
            complex: 'CDOUBLE', numpy.complex64: 'CFLOAT',
            numpy.complex128: 'CDOUBLE', 'complex': 'CDOUBLE',
            }
    if isinstance(data, numpy.dtype):
        data = str(data)
    if type(data) in (str, six.text_type):
        return dmap.get(data, data)
    if not isinstance(data, type):
        # we use the type, not an instance (which may not be hashable)
        dtn2 = type(data).__name__
        if dtn2.startswith('rc_ptr_'):
            # special case of rc_ptr: get the object in it
            try:
                data = type(data.get())
            except AttributeError:
                data = type(data)
        else:
            data = type(data)
    dt = dmap.get(data, None)
    if dt is not None:
        return dt
    dtn = getattr(data, '__name__', None)
    if dtn is not None:  # if data is a type class
        dt = dmap.get(dtn, None)
        if dt is not None:
            return dt
    return dtn


def voxelTypeCode(data):
    '''returns the AIMS type code for the given input data voxel type.
    For instance, for a volume of 16 bit ints, it will be 'S16', whereas
    typeCode() would return 'Volume_S16'.

    Input data may be a type or an instance of a container type.
    '''
    if not hasattr(data, '__name__'):
        tdata = type(data)
    else:
        tdata = data
    if tdata is numpy.ndarray:
        return typeCode(data.dtype)
    name = tdata.__name__
    if name.startswith('rc_ptr_'):
        name = name[7:]
    container_types = ['Volume_', 'AimsData_', 'BucketMap_', 'vector_', 'set_',
                       'list_', 'TimeTexture_', 'Texture_',
                       'ResamplerFactory_', 'RegularBinnedHistogram_',
                       'Gaussian2DSmoothing_',
                       'Gaussian3DSmoothing_', 'GeometricMoment_',
                       'Histogram_', 'LinearResampler_',
                       'MaskedDiffusionSmoother_', 'MaskLinearResampler_',
                       'MedianSmoothing_', 'MomentInvariant_', 'Moment_',
                       'MorphoGreyLevel_', 'NearestNeighborResampler_',
                       'QuarticResampler_', 'QuinticResampler_', 'Resampler_',
                       'SeventhOrderResampler_', 'SixthOrderResampler_',
                       'SplineResampler_']
    for container in container_types:
        if name.startswith(container):
            return name[len(container):]
    if name.startswith('AimsVector_'):
        return '_'.join(name[11:].split('_')[:-1])
    return None


def somaio_typeCode(data):
    '''returns the Soma-IO type code (as in
    soma.carto.IOObjectTypesDictionary.readTypes() dict keys) for the given
    input type.

    Parameters
    ----------
    data: string or type or instance, or 2-tuple
        If type or instance, get the type code for the given object.
        If string, try to translate AIMS IO to soma-IO codes.
        If 2-tuple, translate AIMS object type / data type couple
    '''
    def _somaio_objecttype(data):
        if data in ('Volume', 'CartoVolume'):
            return 'carto_volume'
        elif data == 'GenericObject':
            return 'genericobject'
        else:
            return data
    if isinstance(data, tuple):
        return '%s of %s' % (_somaio_objecttype(data[0]), data[1])
    if not isinstance(data, str) and not isinstance(data, six.text_type):
        if hasattr(data, '__name__'):
            data = data.__name__
        else:
            data = data.__class__.__name__
    objtype, dtype = data.split('_')
    return '%s of %s' % (_somaio_objecttype(objtype), dtype)


def _parseTypeInArgs(*args, **kwargs):
    dtype = kwargs.get('dtype', None)
    # print('_parseTypeInArgs:', dtype)
    if dtype is not None:
        # kwargs = dict( kwargs )
        del kwargs['dtype']
    else:
        y = [i for i, x in enumerate(args)
             if type(x) in (str, six.text_type) or hasattr(x, '__name__')]
        if len(y) != 0:
            i = y[0]
            dtype = args[y[0]]
            args = args[:i] + args[i + 1:]
    if dtype is None:
        dtype = kwargs.get('default_dtype', None)
    if dtype is None:
        raise KeyError('data type not specified')
    if 'default_dtype' in kwargs:
        del kwargs['default_dtype']
    dtype = typeCode(dtype)
    return (dtype, args, kwargs)


def _parse2TypesInArgs(*args, **kwargs):
    dtype1 = kwargs.get('intype')
    dtype2 = kwargs.get('outtype')
    dtypeg = kwargs.get('dtype')
    if dtype1 is not None:
        kwargs['dtype'] = dtype1
        del kwargs['intype']
    intype, args2, kwargs2 = _parseTypeInArgs(*args, **kwargs)
    if dtype2 is not None:
        kwargs['dtype'] = dtype2
        del kwargs['outtype']
    try:
        outtype, args3, kwargs3 = _parseTypeInArgs(*args2, **kwargs)
    except:
        outtype, args3, kwargs3 = _parseTypeInArgs(*args, **kwargs)
    return (intype, outtype, args3, kwargs3)


def _createObject(objtype, *args, **kwargs):
    dtype, args, kwargs = _parseTypeInArgs(*args, **kwargs)
    return getattr(aimssip, objtype + '_' + dtype)(*args, **kwargs)


def Volume(*args, **kwargs):
    '''Create an instance of Aims Volume (Volume_<type>) from a type parameter,
    which may be specified as the dtype keyword argument, or as one of the
    arguments if one is identitied as a type.
    The default type is 'FLOAT'.
    Type definitions should match those accepted by typeCode().
    Volume may also use a numpy array, or another Volume or AimsData_* as unique
    argument.

    Note that Volume( Volume_* ) or Volume( AimsData_* ) actually performs a
    copy of the data, whereas AimsData( Volume_* ) or AimsData( AimsData_* )
    share the input data.
    '''
    if len(args) == 1 and len(kwargs) == 0:
        arg = args[0]
        if isinstance(arg, numpy.ndarray):
            return _createObject('Volume', arg, dtype=arg.dtype.type)
        elif type(arg).__name__.startswith('Volume_'):
            return type(arg)(arg)
        elif type(arg).__name__.startswith('AimsData_'):
            return type(arg)(arg.volume())
    return _createObject('Volume', default_dtype='FLOAT', *args, **kwargs)


def VolumeView(volume, position, size):
    '''Create a view in an existing volume. The returned volume is of the same
    type and shares data with its "parent".
    '''
    volclass = type(volume)
    if volclass.__name__.startswith('Volume_'):
        vol = volume
    elif volclass.__name__.startswith('AimsData_'):
        vol = volume.volume()
        volclass = type(volume.volume())
    elif volclass.__name__.startswith('rc_ptr_'):
        volclass = type(volume.get())
        vol = volume
    else:
        raise TypeError('incompatible or wrong volume type: %s'
            % volclass.__name__)

        if len(size) < 4:
            size = list(size) + [1] * (4 - len(size))
        for i in range(4):
            if size[i] == 0:
                size[i] = 1
    return volclass(vol, position, size)


def AimsData(*args, **kwargs):
    '''Create an instance of the older Aims volumes (AimsData_<type>) from a
    type parameter, which may be specified as the dtype keyword argument, or as one of the arguments if one is identitied as a type.

    The default type is 'FLOAT'.

    Type definitions should match those accepted by typeCode().
    AimsData may also use a numpy array, or another Volume or AimsData_* as
    unique argument.

    Note that Volume( Volume_* ) or Volume( AimsData_* ) actually performs a
    copy of the data, whereas AimsData( Volume_* ) or AimsData( AimsData_* )
    share the input data.
    '''
    if len(args) == 1 and len(kwargs) == 0:
        arg = args[0]
        if isinstance(arg, numpy.ndarray):
            vol = _createObject('Volume', arg, dtype=arg.dtype.type)
            return AimsData(vol)
        elif type(arg).__name__.startswith('Volume_'):
            return getattr(aimssip, 'AimsData_' + type(arg).__name__[7:])(arg)
        elif type(arg).__name__.startswith('AimsData_'):
            return type(arg)(arg.volume())
    return _createObject('AimsData', default_dtype='FLOAT', *args, **kwargs)


def TimeTexture(*args, **kwargs):
    '''Create an instance of Aims texture (TimeTexture_<type>) from a type
    parameter, which may be specified as the dtype keyword argument, or as one
    of the arguments if one is identitied as a type.

    The default type is 'FLOAT'.

    Type definitions should match those accepted by typeCode().
    TimeTexture may also use a numpy array, or another TimeTexture_* or
    Textrue_* as unique argument.

    Building from a numpy arrays uses the 1st dimension as vertex, the 2nd as
    time (if any).
    '''
    if len(args) == 1 and len(kwargs) == 0:
        arg = args[0]
        if isinstance(args[0], numpy.ndarray):
            tex = _createObject('TimeTexture', dtype=arg.dtype.type)
            if len(arg.shape) == 1:
                tex[0].assign(arg)
            else:
                for i in range(arg.shape[1]):
                    tex[i].assign(arg[:, i])
            return tex
        if type(arg).__name__.startswith('TimeTexture_'):
            return type(arg)(arg)
        elif type(arg).__name__.startswith('Texture_'):
            tex = getattr(aimssip, 'TimeTexture_' + type(arg).__name__[8:])()
            tex[0] = arg
            return tex
    return _createObject('TimeTexture', default_dtype='FLOAT', *args, **kwargs)


def Texture(*args, **kwargs):
    '''Create an instance of Aims low-level texture (Texture_<type>) from a type
    parameter, which may be specified as the dtype keyword argument, or as one
    of the arguments if one is identitied as a type.

    The default type is 'FLOAT'.

    Type definitions should match those accepted by typeCode().

    Texture may also use a numpy array, or another Textrue_* as unique argument.
    '''
    if len(args) == 1 and len(kwargs) == 0:
        arg = args[0]
        if isinstance(args[0], numpy.ndarray):
            return _createObject('Texture', arg, dtype=arg.dtype.type)
        if type(arg).__name__.startswith('Texture_'):
            return type(arg)(arg)
    return _createObject('Texture', default_dtype='FLOAT', *args, **kwargs)


def BucketMap(*args, **kwargs):
    '''Create an instance of Aims bucket (BucketMap_<type>) from a type
    parameter, which may be specified as the dtype keyword argument, or as one
    of the arguments if one is identitied as a type.

    The default type is 'VOID'.

    Type definitions should match those accepted by typeCode().
    '''
    return _createObject('BucketMap', default_dtype='VOID', *args, **kwargs)


def Converter(*args, **kwargs):
    '''Create a Converter instance from input and output types. Types may be
    passed as keyword arguments intype and outtype, or dtype if both are the
    same (not very useful for a converter). Otherwise the arguments are parsed
    to find types arguments.

    Types may be specified as allowed by typeCode().
    '''
    intype, outtype, args, kwargs = _parse2TypesInArgs(*args, **kwargs)
    return getattr(aimssip, 'Converter_' + intype + '_' + outtype)(*args)


def ShallowConverter(*args, **kwargs):
    '''Create a ShallowConverter instance from input and output types. Types may
    be passed as keyword arguments intype and outtype, or dtype if both are the
    same (not very useful for a converter). Otherwise the arguments are parsed
    to find types arguments.

    Types may be specified as allowed by typeCode().
    '''
    intype, outtype, args, kwargs = _parse2TypesInArgs(*args, **kwargs)
    return getattr( aimssip, 'ShallowConverter_' + intype + '_' + outtype ) \
        (*args)


def TimeSurface(*args, **kwargs):
    '''same as AimsTimeSurface(dim, dtype)'''
    return AimsTimeSurface(*args, **kwargs)


def AimsTimeSurface(*args, **kwargs):
    '''Create an instance of Aims mesh (AimsTimeSurface_<dim>_<dtype>) from a
    dimension parameter, or copies a mesh, or build it from arrays.

    See for instance :py:class:`AimsTimeSurface_3_VOID`

    Usages:

    ::

        mesh1 = AimsTimeSurface()
        mesh2 = AimsTimeSurface(3)
        mesh3 = AimsTimeSurface(3, 'VOID')
        mesh4 = AimsTimeSurface(3, dtype='VOID')
        mesh5 = AimsTimeSurface(dim=3, dtype='VOID')

    ::

        mesh6 = AimsTimeSurface(mesh)

    ::

        mesh7 = AimsTimeSurface(vertices=[[1,2,3], [3,4,5.6]],
                                polygons=[[0,1]])
        mesh8 = AimsTimeSurface([[1,2,3], [3,4,5.6]], polygons=[[0,1]])
        mesh9 = AimsTimeSurface([[1,2,3], [3,4,5.6]], None, [[0,1]])
        mesh10 = AimsTimeSurface([[1,2,3], [3,4,5.6]], [[0,1]], normals=[])
    '''
    dtype = kwargs.get('dtype', None)
    dim = kwargs.get('dim', None)
    vert = kwargs.get('vertices', None)
    norm = kwargs.get('normals', None)
    poly = kwargs.get('polygons', None)
    has_arrays = not (vert is None and norm is None and poly is None)

    if not has_arrays and len(args) == 0:
        if dim is None:
            dim = 3
        if dtype is None:
            dtype = 'VOID'
        return getattr(aimssip, 'AimsTimeSurface_%d_%s' % (dim, dtype))()

    if not has_arrays and len(args) == 1 and dim is None \
            and isinstance(args[0], numbers.Number):
        dim = args[0]
    elif not has_arrays and len(args) == 1 \
            and type(args[0]).__name__.startswith('AimsTimeSurface_'):
        # copy
        return type(args[0])(args[0])
    elif not has_arrays and len(args) == 2 \
            and isinstance(args[0], numbers.Number) \
            and dtype is None and dim is None:
        dim = args[0]
        dtype = args[1]
    elif dim is None:
        nargs = 0
        if vert is None and len(args) > nargs:
            vert = args[nargs]
            nargs += 1
        if norm is None and len(args) > nargs:
            norm = args[nargs]
            nargs += 1
        if poly is None and len(args) > nargs:
            poly = args[nargs]
            nargs += 1
        if dtype is None and len(args) > nargs:
            dtype = args[nargs]
            nargs += 1
        if len(args) > nargs + 1 or vert is None or poly is None:
            raise ValueError('Wrong parameters for AimsTimeSurface')
        if not poly:
            if dim is None:
                dim = 3
        else:
            if isinstance(poly[0], AimsVector_U32_2):
                dim = 2
            elif isinstance(poly[0], AimsVector_U32_3):
                dim = 3
            elif isinstance(poly[0], AimsVector_U32_4):
                dim = 4
            else:
                dim = len(poly[0])
        if dtype is None:
            dtype = 'VOID'
        mesh = getattr(aimssip, 'AimsTimeSurface_%d_%s' % (dim, dtype))()
        if not isinstance(vert[0], Point3df):
            vert = [Point3df(x) for x in vert]
        poly_type = getattr(aimssip, 'AimsVector_U32_%d' % dim)
        if not isinstance(poly[0], poly_type):
            poly = [poly_type(x) for x in poly]
        mesh.vertex().assign(vert)
        mesh.polygon().assign(poly)
        if norm:
            if not isinstance(norm[0], Point3df):
                norm = [Point3df(x) for x in norm]
            mesh.normal().assign(norm)
        return mesh
    if dtype is None:
        dtype = 'VOID'
    return getattr(aimssip, 'AimsTimeSurface_%d_%s' % (dim, dtype))()


def AimsThreshold(*args, **kwargs):
    '''Create a AimsThreshold instance from input and output types. Types may
    be passed as keyword arguments intype and outtype, or dtype if both are the
    same. Otherwise the arguments are parsed to find types arguments.
    Types may be specified as allowed by typeCode().

    Other arguments are otherwise passed to the constructor of the underlying
    threshold object. See for instance:
    :class:`soma.aims.AimsThreshold_S16_S16`.
    '''
    intype, outtype, args, kwargs = _parse2TypesInArgs(*args, **kwargs)
    if intype.startswith('Volume_'):
        intype = intype[7:]
    elif intype.startswith('AimsData_'):
        intype = intype[9:]
    if outtype.startswith('Volume_'):
        outtype = outtype[7:]
    elif outtype.startswith('AimsData_'):
        outtype = outtype[9:]
    return getattr(aimssip, 'AimsThreshold_' + intype + '_' + outtype)(*args)


def AimsVector(*args, **kwargs):
    '''Create an AimsVector instance from type and dimension arguments. Types
    may be passed as the keyword argument dtype. Otherwise the arguments are
    parsed to find types arguments. Dimension should be passed as the keyword
    argument dim, or is guessed from the input value(s).

    Types may be specified as allowed by typeCode().

    If unspecified, type is guessed from the 1st element of the vector data.
    '''
    dim = kwargs.get('dim', None)
    if dim is not None:
        del kwargs['dim']
    elif len(args) == 1 and hasattr(args[0], '__len__'):
        dim = len(args[0])
    else:
        dim = len(args)
    try:
        dtype, args, kwargs = _parseTypeInArgs(*args, **kwargs)
    except KeyError as e:
        try:
            if hasattr(args[0], '__len__') and len(args[0]) != 0:
                dtype = _parseTypeInArgs(dtype=args[0][0])[0]
            else:
                dtype = _parseTypeInArgs(dtype=args[0])[0]
        except:
            raise e
    return getattr(aimssip, 'AimsVector_' + dtype + '_' + str(dim))(*args)


def stdVector(*args, **kwargs):
    '''Create an instance of STL C++ vector (vector_<type>) from a type
    parameter, which may be specified as the dtype keyword argument, or as one
    of the arguments if one is identitied as a type.

    Type definitions should match those accepted by typeCode().
    '''
    return _createObject('vector', *args, **kwargs)


def stdSet(*args, **kwargs):
    '''Create an instance of STL C++ set (set_<type>) from a type parameter,
    which may be specified as the dtype keyword argument, or as one of the
    arguments if one is identitied as a type.

    Type definitions should match those accepted by typeCode().
    '''
    return _createObject('set', *args, **kwargs)


def stdList(*args, **kwargs):
    '''Create an instance of STL C++ list (list_<type>) from a type parameter,
    which may be specified as the dtype keyword argument, or as one of the
    arguments if one is identitied as a type.

    Type definitions should match those accepted by typeCode().
    '''
    return _createObject('list', *args, **kwargs)


def rc_ptr(*args, **kwargs):
    '''Create an instance of aims reference-counting object (rc_ptr_<type>) from
    a type parameter, which may be specified as the dtype keyword argument, or
    as one of the arguments if one is identitied as a type.

    Type definitions should match those accepted by typeCode().
    '''
    return _createObject('rc_ptr', *args, **kwargs)


# callback to fix sys.stderr / sys.stdout after use of carto::fdinhibitor
class StdOutInhibitorFix(carto.fdinhibitor.ResetCallback):
    def __call__(self, fd):
        if fd == sys.stdout.fileno():
            try:
                sys.stdout.write('')
            except: # IOError:
                pass
        elif fd == sys.stderr.fileno():
            try:
                sys.stderr.write('')
            except: # IOError:
                pass

#carto.fdinhibitor.registerResetCallback('sys.stderr', StdOutInhibitorFix())


def supported_io_formats(otypes=None, access=''):
    '''
    Returns a list of formats supported to read or write the given object
    types. Both AIMS and Soma-IO support are queried.

    Parameters
    ----------
    otypes: dict, list, str, or None
        object types. May be given in several shapes: 1. None: will query all
        possible object types. 2. str: will get all data types (voxel, texture)
        for the given container object type (Volume, Mesh etc.). 3. list of
        str: a list of container object types, as in the str case. 4. dict:
        allows to specify precisely object type / data types mapping: for each
        object type, a list of data types will be used. When a data type is
        None, then all possible ones will be used.
    access: str
        read / write capabilities: restricts soma-io IO queries to the
        specified access modes. aims IO will not be affected by this parameter
        since it does not separate reading and writing capabilities. Possible
        values are: '': any, 'r': read, 'w': write, 'rw': both needed.

    Returns
    -------
    formats: set of str
        set of formats names
    '''
    # make otypes a dict object_type: data_types (data_types may be None)
    iotypes = IOObjectTypesDictionary.objectsTypes()
    if otypes is None:
        otypes = iotypes
    elif isinstance(otypes, six.string_types):
        otypes = {otypes: None}
    elif isinstance(otypes, (list, tuple)):
        otypes = dict([(x, None) for x in otypes])
    iosread = carto.IOObjectTypesDictionary.readTypes()
    ioswrite = carto.IOObjectTypesDictionary.writeTypes()
    formats = set()
    for otype, dtypes in six.iteritems(otypes):
        if otype not in iotypes:
            continue
        if dtypes is None:
            dtypes = iotypes[otype]
        for dtype in dtypes:
            aiof = IOObjectTypesDictionary.formats(otype, dtype)
            # filter "real" formats (with actual extensions)
            aiof = [f for f in aiof if len(Finder.extensions(f)) != 0]
            formats.update(aiof)
            stype = somaio_typeCode((otype, dtype))
            siof = set()
            if access in ('', 'r'):
                siof = set(iosread.get(stype, []))
            if access in ('', 'w'):
                siof.update(ioswrite.get(stype, []))
            elif access == 'rw':
                # need both r and w
                fr = iosread.get(stype, [])
                fw = ioswrite.get(stype, [])
                siof = set(fr).intersection(fw)
            # filter "real" formats (with actual extensions)
            siof = [f for f in siof
                    if len(soma.DataSourceInfoLoader.extensions(f)) != 0]
            formats.update(siof)
    return formats


def get_transform_to(aims_obj, ref):
    ''' Get a transformation object in an AIMS object header to the given referential identifier. Objects need to have a header() method following the aims conventions (``referentials`` and ``transformations`` elements beingn lists).

    Parameters
    ----------
    aims_obj: Aims object with a header
        object we want to get the transformation from
    ref: str
        target refetential identifier. May be an UUID string, or a known
        identifier such as "Talairach-MNI template-SPM" (see
        aims.StandardReferentials attributes)

    Returns
    -------
    transform: aims.AffineTransformation3d
        the transformation object, or None if no matching one is found
    '''
    h = aims_obj.header()
    refs = h.get('referentials', [])
    if len(refs) == 0:
        return None
    if ref not in refs:
        if ref == StandardReferentials.acPcReferential():
            if StandardReferentials.acPcReferentialID() in refs:
                ref = StandardReferentials.acPcReferentialID()
        elif ref == StandardReferentials.acPcReferentialID():
            if StandardReferentials.acPcReferential() in refs:
                ref = StandardReferentials.acPcReferential()
        elif ref == aims.StandardReferentials.mniTemplateReferential():
            if aims.StandardReferentials.mniTemplateReferentialID() in refs:
                ref = aims.StandardReferentials.mniTemplateReferentialID()
        elif ref \
                == aims.StandardReferentials.commonScannerBasedReferentialID():
            if aims.StandardReferentials.commonScannerBasedReferential() \
                    in refs:
                ref = aims.StandardReferentials.commonScannerBasedReferential()
    print('ref:', ref)
    if ref not in refs:
        return None
    index = refs.index(ref)
    print('index:', index)
    try:
        tr = h.get('transformations', [])[index]
    except IndexError:
        return None
    return aims.AffineTransformation3d(tr)


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

    >>> from __future__ import print_function # work using python 2 or 3
    >>> from soma import aims
    >>> vol = aims.read('volume.img')
    >>> hdr = vol.header()

Now ``vol`` is the volume, and ``hdr`` its header, of type ``Object``.

    >>> type(hdr)
    <class 'soma.aims.Object'>
    >>> print(hdr)
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

    >>> print(hdr['voxel_size'])
    [ 1.875, 1.875, 4, 1 ]
    >>> print(hdr['data_type'])
    S16
    >>> print(hdr['voxel_size'][0])
    1.875
    >>> for x in hdr:
    >>>     print(x)
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
    >>>     print(x, ':', y)
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

    >>> hdr['data_type'] = 'FLOAT'
    >>> hdr['data_type']
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

>>> a = aims.vector_FLOAT([12.6, -5.7])
>>> print(a)
[ 12.6, -5.7 ]
>>> hdr['foo'] = a # here a is copied into hdr['foo']
>>> a.append( 6.8 ) # a is changed but not hdr
>>> print(a)
[ 12.6, -5.7, 6.8 ]
>>> print(hdr['foo'])
[ 12.6, -5.7 ]
>>> hdr['dummy'] = hdr['foo']
>>> hdr['foo'].append(4.2)
>>> print(hdr['dummy']) # this time hdr['dummy'] is changed
[ 12.6, -5.7, 4.2 ]

To overcome the first copy problem, you may have to reassign the initial
variable to the copy instance:

>>> a = aims.vector_FLOAT([1.2, 2.3, 3.4])
>>> hdr['foo'] = a
>>> a = hdr['foo']
>>> print(hdr['foo'])
[ 1.2, 2.3, 3.4 ]
>>> a[1] = 12.8
>>> print(a)
[ 1.2, 12.8, 3.4 ]
>>> print(hdr['foo'])
[ 1.2, 12.8, 3.4 ]

There are exceptions to this behaviour:

- pure python objects, like lists or dictionaries are never copied
  since it is only a pointer to them (the C `PyObject *` pointer) which
  is stored.
- small builtin types: numbers and strings are always copied since they
  are always converted and copied, not wrapped, when passed from python
  to C++ and vice versa.

**The get() method**

On :class:`Object` the ``get`` method is ambiguous and has 2 meanings:

* get() without arguments is a wrapping to the C++ ``rc_ptr::get()`` which
  returns the underlying wrapped object (here, a :class:`GenericObject`)
* get(key, default=None) is the dict-like method that is available on
  :class:`GenericObject` since aims 4.6.1.

Depending on the arguments the Object method will redirect to the right one, thus:

::

    myobject.get()            # gets the GenericObject
    myobject.get('key')       # gets the dict item under keyt 'key'
    myobject.get().get('key') # does both, but this is what the previous line
                              # already does, so the result is the same

To avoid this ambiguity, ``Object.get()`` and ``rc_ptr.get()`` have been
renamed ``_get()`` years ago (in aims 4.0 I think) but ``get()`` is still
present for compatibility. This use (without argument) is obsolete and may be
removed in the future.
'''

_volumedoc = '''
**Generalities**

The various Volume_<type> classes are bindings to the :cartoddox:`C++ template classes carto::Volume <classcarto_1_1Volume.html>`. It represents a 4D volume (1D to 4D, actually) storing a
specific type of data: for instance S16 is signed 16 bit short ints, FLOAT is
32 bit floats,
etc.

Volumes are read and written using the :py:class:`soma.aims.Reader` and
:py:class:`soma.aims.Writer` classes, which are in turn used by the simple
:py:func:`soma.aims.read` and :py:func:`soma.aims.write` functions.

A volume of a given type can be built either using its specialized class constructor, or the general Volume() function which can take a voxel type in its arguments: the following are equivalent:

    >>> v = aims.Volume_S16(100, 100, 10)
    >>> v = aims.Volume('S16', 100, 100, 10)
    >>> v = aims.Volume(100, 100, 10, dtype='S16')
    >>> v = aims.Volume([100, 100, 10], dtype='S16')
    >>> import numpy
    >>> v = aims.Volume(numpy.int16, 100, 100, 10)

**Numpy arrays and volumes**

A volume is an array of voxels, which can be accessed via the ``at()``
method.
For standard numeric types, it is also possible to get the voxels array as a
numpy_ array, using the ``__array__()`` method, or more conveniently,
``numpy.asarray(volume)`` or ``numpy.array(volume, copy=False)``.
The array returned is a reference to the actual data block, so any
modification to its contents also affect the Volume contents, so it is
generally an easy way of manipulating volume voxels because all the power of
the numpy module can be used on Volumes.
The ``arraydata()`` method returns a numpy array just like it is in memory,
that is a 4D (or more) array indexed by ``[t][z][y][x]``, which is generally not what you like and is not consistent with AIMS indexing. Contrarily, using
``numpy.asarray(volume)`` sets strides in the returned numpy
array, so that indexing is in the "normal" order ``[x][y][z][t]``, while still sharing the same memory block.
The Volume object now also wraps the numpy accessors to the volume object itself, so that ``volume[x, y, z, t]`` is the same as
``nupmy.asarray(volume)[x, y, z, t]``.

*new in PyAims 4.7*

Since PyAims 4.7 the numpy arrays bindings have notably improved, and are now able to bind arrays to voxels types which are not scalar numeric types. Volumes of RGB, RGBA, HSV, or Point3df now have numpy bindings. The bound object have generally a "numpy struct" binding, that is not the usual C++/python object binding, but instead a structure managed by numpy which also supports indexing. For most objects we are using, they also have an array stucture (a RGB is an array with 3 int8 items), and are bound under a sturcture with a unique field, named "v" (for "vector"):

    >>> v = aims.Volume('RGB', 100, 100, 10)
    >>> numpy.asarray(v)[0, 0, 0, 0]
    ([0, 0, 0],)

Such an array may be indexed by the field name, which returns another array with scalar values and additional dimensions:

    >>> numpy.asarray(v)['v'].dtype
    dtype('uint8')
    >>> numpy.asarray(v)['v'].shape
    (100, 100, 10, 1, 3)

Both arrays share their memory with the aims volume.

.. seealso:: :ref:`numpy_bindings`

**Header**

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

    >>> vol1 = aims.Volume('S16', 100, 100, 10)
    >>> c = aims.Converter(intype=vol1, outtype='Volume_DOUBLE')
    >>> vol2 = c(vol1)

**Volume from a numpy array**

It is also possible to build a Volume from a numpy array:

    >>> v = aims.Volume(numpy.zeros((100, 100, 10)))

Note that passing a 1D numpy array of ints will build a volume mapping the array contents, whereas passing a python list or tuple of ints will interpret the list as a shape for the volume:

    >>> v = aims.Volume(numpy.array([100, 100, 10]).astype('int32'))
    >>> print(v.getSize())
    [ 3, 1, 1, 1 ]
    >>> print(np.asarray(v))
    [100 100  10]
    >>> v = aims.Volume([100, 100, 10])
    >>> print(v.getSize())
    [ 100, 100, 10, 1 ]

.. _numpy: http://numpy.scipy.org/
'''

_aimsdatadoc = '''
``AimsData_<type>`` classes correspond to the python bindings of C++
template classes :aimsdox:`AimsData <classAimsData.html>`. They are planned to be obsolete, and
replaced with the Volume_<type> classes. Try avoiding using them unless
using functions that work on ``AimsData_<type>``.

``AimsData`` actually contains a ``Volume``: you can retreive it using the
``volume()`` method.

Inversely, you can also build a ``Volume`` from a ``AimsData`` by passing it
to the ``Volume`` constructor.

In all cases the voxels memory block is shared.
'''

for x, y in list(locals().items()):
    if x.startswith('Volume_'):
        y.__doc__ = _volumedoc
    elif x.startswith('AimsData_'):
        y.__doc__ = _aimsdatadoc
del x, y

del _volumedoc, _aimsdatadoc


carto.AllocatorStrategy.DataAccess.__doc__ = '''
Data access mode

InternalModif:
    random access in memory, disk data (memmapped) should not be overwritten.
ReadOnly:
    read only in memory. Accessing data in write mode may cause a crash.
ReadWrite:
    read/write both in memory and on disk, if memmapped.
NotOwner:
    data is unallocated, or its ownership belongs to a proxy object.
'''

carto.AllocatorStrategy.MappingMode.__doc__ = '''
Memory mapping modes

Memory, (MEM):
    in-memory allocation
CopyMap, (MAP, MAP_COPY):
    data is copied into a memmaped file (accessed read/write)
ReadOnlyMap, (MAP_RO):
    read-only memory mapping
ReadWriteMap, (MAP_RW):
    read/write memory mapping: modifying the memory also modifies the file on disk.
Unallocated:
    not allocated.
'''

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

private = ['private', 'aimssip', 'aimsguisip', 'numpy', 'sip', 'types',
           'weakref']
__all__ = []
for x in dir():
    if not x.startswith('_') and x not in private:
        __all__.append(x)
del x, private
