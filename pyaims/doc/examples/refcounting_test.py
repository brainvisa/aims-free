#!/usr/bin/env python

from soma import aims

_created = False
_deleted = False

class MyVol ( aims.Volume_S16 ):
  def __init__( self, *args ):
    print 'creating MyVol'
    aims.Volume_S16.__init__( self, *args )
    global _created, _deleted
    _created = True
    _deleted = False
  def __del__( self ):
    print 'deleting MyVol, C++ refs:',  self.__refcount__()
    aims.Volume_S16.__del__( self )
    global _deleted
    _deleted = True

class MyVol2 ( aims.pyaims.DebuggingVolume ):
  def __init__( self, *args ):
    print 'creating MyVol2'
    aims.pyaims.DebuggingVolume.__init__( self, *args )
    global _created, _deleted
    _created = True
    _deleted = False
  def __del__( self ):
    print 'deleting MyVol2, C++ refs:', self.__refcount__()
    aims.pyaims.DebuggingVolume.__del__( self )
    global _deleted
    _deleted = True

class MyClass( object ):
  def __init__( self ):
    print 'creating Myclass'
    object.__init__( self )
    global _created, _deleted
    _created = True
    _deleted = False
  def __del__( self ):
    print 'deleting Myclass'
    global _deleted
    _deleted = True


print 'testing rc_ptr<inherited Python instance> built from temporary instance'
r = aims.rc_ptr_Volume_S16( MyVol() )
print 'ref created'
if _deleted:
  print 'ERROR: Python instance deleted whereas a rc_ptr still points to it.'
  print '(this error is "normal" for sip <= 4.7.x)'
print 'deleting ref'
del r
if not _deleted:
  print 'ERROR: Python instance not deleted when last reference is deleted'
_created = False
_deleted = False

print 'testing rc_ptr<inherited C++ instance> built from temporary instance'
r = aims.rc_ptr_Volume_S16( aims.pyaims.DebuggingVolume() )
print 'ref created'
if aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance deleted whereas a rc_ptr still points to it.'
print 'deleting ref'
del r
if not aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance not deleted when last reference is deleted'
aims.pyaims.DebuggingVolume.created = False
aims.pyaims.DebuggingVolume.deleted = False

print 'testing rc_ptr<inherited Python instance from inherited C++ class> ' \
  'built from temporary instance'
r = aims.rc_ptr_Volume_S16( MyVol2() )
print 'ref created'
if _deleted:
  print 'ERROR: Python instance deleted whereas a rc_ptr still points to it.'
  print '(this error is "normal" for sip <= 4.7.x)'
if aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance deleted whereas a rc_ptr still points to it.'
print 'deleting ref'
del r
if not _deleted:
  print 'ERROR: Python instance not deleted when last reference is deleted'
if not aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance not deleted when last reference is deleted'
_created = False
_deleted = False
aims.pyaims.DebuggingVolume.created = False
aims.pyaims.DebuggingVolume.deleted = False

print 'testing Object->GenericObject on-the-fly temporary conversion'
o = aims.Object( aims.pyaims.DebuggingVolume() )
if aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance deleted whereas a rc_ptr still points to it.'
try:
  aims.carto.NumericGenericObjectConverter.asBool( o )
except:
  pass # it's normal it raises an exception
if aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance deleted after Object->GenricObject temporary ' \
    'conversion whereas a rc_ptr still points to it.'
del o
if not aims.pyaims.DebuggingVolume.deleted:
  print 'ERROR: C++ instance not deleted when last reference is deleted'

print 'testing python dict -> GenericObject on-the-fly temporary conversion'
try:
  aims.carto.NumericGenericObjectConverter.asBool( { 'toto': 12 } )
except:
  pass
print 'no crash, good...'
print 'testing leaks in same situation'
try:
  aims.carto.NumericGenericObjectConverter.asBool( MyClass() )
except:
  pass
if not _deleted:
  print 'ERROR: Python instance not deleted when last reference is deleted'
_created = False
_deleted = False

print 'testing python dict -> GenericObject conversion + ref-counting'
x = MyClass()
o = aims.Object( x )
print 'ref created. deleting dict.'
del x
if _deleted:
  print 'ERROR: Python instance deleted whereas a rc_ptr still points to it.'
print 'deleting ref'
del o
if not _deleted:
  print 'ERROR: Python instance not deleted when last reference is deleted'
_created = False
_deleted = False


print 'ending.'
