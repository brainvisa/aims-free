#!/usr/bin/env python2

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
from __future__ import print_function
from soma import aims

_created = False
_deleted = False


class MyVol (aims.Volume_S16):

    def __init__(self, *args):
        print('creating MyVol')
        aims.Volume_S16.__init__(self, *args)
        global _created, _deleted
        _created = True
        _deleted = False

    def __del__(self):
        print('deleting MyVol, C++ refs:',  self.__refcount__())
        aims.Volume_S16.__del__(self)
        global _deleted
        _deleted = True


class MyVol2 (aims.pyaims.DebuggingVolume):

    def __init__(self, *args):
        print('creating MyVol2')
        aims.pyaims.DebuggingVolume.__init__(self, *args)
        global _created, _deleted
        _created = True
        _deleted = False

    def __del__(self):
        print('deleting MyVol2, C++ refs:', self.__refcount__())
        aims.pyaims.DebuggingVolume.__del__(self)
        global _deleted
        _deleted = True


class MyClass(object):

    def __init__(self):
        print('creating Myclass')
        object.__init__(self)
        global _created, _deleted
        _created = True
        _deleted = False

    def __del__(self):
        print('deleting Myclass')
        global _deleted
        _deleted = True


print(
    'testing rc_ptr<inherited Python instance> built from temporary instance')
r = aims.rc_ptr_Volume_S16(MyVol())
print('ref created')
if _deleted:
    print(
        'ERROR: Python instance deleted whereas a rc_ptr still points to it.')
    print('(this error is "normal" for sip <= 4.7.x)')
print('deleting ref')
del r
if not _deleted:
    print('ERROR: Python instance not deleted when last reference is deleted')
_created = False
_deleted = False

print('testing rc_ptr<inherited C++ instance> built from temporary instance')
r = aims.rc_ptr_Volume_S16(aims.pyaims.DebuggingVolume())
print('ref created')
if aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance deleted whereas a rc_ptr still points to it.')
print('deleting ref')
del r
if not aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance not deleted when last reference is deleted')
aims.pyaims.DebuggingVolume.created = False
aims.pyaims.DebuggingVolume.deleted = False

print('testing rc_ptr<inherited Python instance from inherited C++ class> '
      'built from temporary instance')
r = aims.rc_ptr_Volume_S16(MyVol2())
print('ref created')
if _deleted:
    print(
        'ERROR: Python instance deleted whereas a rc_ptr still points to it.')
    print('(this error is "normal" for sip <= 4.7.x)')
if aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance deleted whereas a rc_ptr still points to it.')
print('deleting ref')
del r
if not _deleted:
    print('ERROR: Python instance not deleted when last reference is deleted')
if not aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance not deleted when last reference is deleted')
_created = False
_deleted = False
aims.pyaims.DebuggingVolume.created = False
aims.pyaims.DebuggingVolume.deleted = False

print('testing Object->GenericObject on-the-fly temporary conversion')
o = aims.Object(aims.pyaims.DebuggingVolume())
if aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance deleted whereas a rc_ptr still points to it.')
try:
    aims.carto.NumericGenericObjectConverter.asBool(o)
except:
    pass  # it's normal it raises an exception
if aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance deleted after Object->GenricObject temporary '
          'conversion whereas a rc_ptr still points to it.')
del o
if not aims.pyaims.DebuggingVolume.deleted:
    print('ERROR: C++ instance not deleted when last reference is deleted')

print('testing python dict -> GenericObject on-the-fly temporary conversion')
try:
    aims.carto.NumericGenericObjectConverter.asBool({'toto': 12})
except:
    pass
print('no crash, good...')
print('testing leaks in same situation')
try:
    aims.carto.NumericGenericObjectConverter.asBool(MyClass())
except:
    pass
if not _deleted:
    print('ERROR: Python instance not deleted when last reference is deleted')
_created = False
_deleted = False

print('testing python dict -> GenericObject conversion + ref-counting')
x = MyClass()
o = aims.Object(x)
print('ref created. deleting dict.')
del x
if _deleted:
    print(
        'ERROR: Python instance deleted whereas a rc_ptr still points to it.')
print('deleting ref')
del o
if not _deleted:
    print('ERROR: Python instance not deleted when last reference is deleted')
_created = False
_deleted = False


print('ending.')
