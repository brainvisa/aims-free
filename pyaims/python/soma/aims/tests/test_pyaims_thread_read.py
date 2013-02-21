#!/usr/bin/env python

import threading
from soma import aims
import os, sys
from optparse import OptionParser
import threading
import tempfile
import shutil


def aims_test_thread_read( filenames, verbose=True ):

    class Loadfile( object ):
        def __init__( self, filename, lock, objlist, verbose ):
            self._filename = filename
            self.lock = lock
            self.objlist = objlist
            self.verbose = verbose
        def __call__( self ):
            if self.verbose:
                print 'reading %s...' % self._filename
            obj = aims.read( self._filename )
            if self.verbose:
                print 'read %s: %s' % ( self._filename, str(type(obj)) )
            self.lock.acquire()
            self.objlist.append( obj )
            self.lock.release()

    aims.carto.PluginLoader.load() # do this once in main thread

    threads = []
    lock = threading.RLock()
    objlist = []

    for fname in filenames:
        thread = threading.Thread(
            target=Loadfile( fname, lock, objlist, verbose ) )
        thread.start()
        threads.append( thread )

    for thread in threads:
        thread.join()

    print 'finished. Read %d / %d objects.' % \
        ( len( objlist ), len( filenames ) )
    nmissing = len( filenames ) - len( objlist )
    if nmissing != 0:
        print 'Not all objects were loaded, %d missing.' % nmissing
        raise RuntimeError( 'Not all objects were loaded, %d missing.' \
            % nmissing )


def _convertFileFormat( aimsobj, directory, prefix, format ):
    exts = aims.Finder.extensions( format )
    if len( exts ) == 0:
        return None
    exts2 = [ x for x in exts if x != '' ]
    if len( exts ) != len( exts2 ):
        exts2.append( '' )
    exts = exts2
    del exts2
    formatok = False
    for ext in exts:
        if ext == '':
            newfilename = os.path.join( directory, prefix )
        else:
            newfilename = os.path.join( directory,
                '.'.join( ( prefix, ext ) ) )
        try:
            aims.write( aimsobj, newfilename, format=format )
            if not os.path.exists( newfilename ):
                for f in os.listdir( directory ):
                    if not f.endswith( '.minf' ) \
                            and ( ext == '' or f.endswith( '.' + ext ) ):
                        newfilename = os.path.join( directory, f )
                        break
                else:
                    shutil.rmtree( directory )
                    os.mkdir( directory )
                    continue
            f = aims.Finder()
            if f.check( newfilename ) and f.format() == format:
                formatok = True
                break
            else:
                #print 'could not read', newfilename
                shutil.rmtree( directory )
                os.mkdir( directory )
        except:
            shutil.rmtree( directory )
            os.mkdir( directory )
            continue
    if formatok:
        return newfilename
    return None


def test_all_formats( filename, number=30 ):
    f = aims.Finder()
    if not f.check( filename ):
        raise IOError( '%f is not readable' % filename )
    ot = f.objectType(), f.dataType()
    aimsobj = aims.read( filename )
    formats = aims.IOObjectTypesDictionary.formats( *ot )
    success = True
    unsafe_formats = []
    safe_formats = []
    for format in formats:
        #if format in ( 'DICOM' ): continue
        print 'testing: %s / %s, format: %s' % ( ot[0], ot[1], format )
        try:
            directory = tempfile.mkdtemp( prefix='aims_thread_test' )
            newfilename = _convertFileFormat( aimsobj, directory, 'aims_test',
                format )
            if not newfilename:
                print 'could not generate format', format
                #shutil.rmtree( directory )
                continue
            print 'testing read on %s...' % newfilename
            try:
                aims_test_thread_read( [ newfilename ] * number,
                    verbose=False )
                print 'Passed.'
                safe_formats.append( format )
                #shutil.rmtree( directory )
            except:
                print 'format %s is unsafe.' % format
                success = False
                unsafe_formats.append( format )
        finally:
            shutil.rmtree( directory )
    print 'All done for %s / %s. Success =' % ot, success
    if not success:
        return { ot : unsafe_formats }, { ot : safe_formats }
    return {}, { ot : safe_formats }


if __name__ == '__main__':

    parser = OptionParser( description='Perform tests of threaded concurrent loading of aims objects in pyaims' )
    parser.add_option( '-i', '--input', dest='infiles',
        help='files to be read concurrently', action='append', default=[] )
    parser.add_option( '-n', '--number', dest='number',
        help='number of times each file should be read at the same time. Default: 30 if one input filename, 1 otherwise', default=0 )
    parser.add_option( '-a', '--all', dest='all', action='store_true',
        default=False,
        help='test all possible formats for each input file (convert to all of them and test)' )

    options, args = parser.parse_args()

    filenames = options.infiles + args
    if len( filenames ) == 0:
        print 'no input files.'
        parser.parse_args( [ '-h' ] )
    if options.number == 0:
        if len( filenames ) == 1 or options.all:
            num = 30
        else:
            num = 1

    #import libxml2
    #libxml2.newTextReaderFilename( '/tmp/ra_head.gii.minf' )
    #import xml.parsers.expat
    #open( '/tmp/xml.xml', 'w' ).write( '<?xml version="1.0" encoding="utf-8" ?><grop></grop>' )
    #p = xml.parsers.expat.ParserCreate()
    #p.ParseFile( open( '/tmp/xml.xml' ) )

    if options.all:
        unsafe_formats = {}
        safe_formats = {}
        for filename in filenames:
            tested_formats = test_all_formats( filename, num )
            unsafe_formats.update( tested_formats[0] )
            safe_formats.update( tested_formats[1] )
        if len( unsafe_formats ) != 0:
            print 'Results:'
            print 'unsafe formats:'
            print unsafe_formats
            print 'safe formats:'
            print safe_formats
            raise RuntimeError( 'Some tests failed.' )
        else:
            print 'OK.'
            print 'safe formats:'
            print safe_formats
    else:
        filenames = filenames * num
        aims_test_thread_read( filenames )

