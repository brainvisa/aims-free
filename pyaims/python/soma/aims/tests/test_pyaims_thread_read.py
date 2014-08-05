#!/usr/bin/env python

import threading
from soma import aims
import os, sys
from optparse import OptionParser
import threading
import tempfile
import shutil
import subprocess
import time


def aims_test_thread_read( filenames, verbose=True ):

    class Loadfile( object ):
        def __init__( self, filename, lock, objnum, verbose ):
            self._filename = filename
            self.lock = lock
            self.objnum = objnum
            self.verbose = verbose
        def __call__( self ):
            if self.verbose:
                print 'reading %s...' % self._filename
            obj = aims.read( self._filename )
            if self.verbose:
                print 'read %s: %s' % ( self._filename, str(type(obj)) )
            self.lock.acquire()
            self.objnum[0] += 1
            self.lock.release()

    aims.carto.PluginLoader.load() # do this once in main thread

    threads = []
    lock = threading.RLock()
    # objnum is a list, not an int, because the counter has to be shared
    # between all threads: a list is, an int is not
    objnum = [ 0 ]

    starttime = time.time()
    for fname in filenames:
        thread = threading.Thread(
            target=Loadfile( fname, lock, objnum, verbose ) )
        thread.start()
        threads.append( thread )

    for thread in threads:
        thread.join()

    duration = time.time() - starttime
    print 'finished. Read %d / %d objects in %.3f seconds.' % \
        ( objnum[0], len( filenames ), duration )
    nmissing = len( filenames ) - objnum[0]
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


def test_all_formats( filename, number=30, separate_process=False ):
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
        # JP2 writer in Qt (4.8.1 at least) systematically crashes.
        if format in ( 'JP2' ): continue
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
                if separate_process:
                    subprocess.check_call( [ sys.executable, '-m',
                      'soma.aims.tests.test_pyaims_thread_read', '-i',
                      newfilename, '-n', str(number), '--silent' ] )
                else:
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
    parser.add_option( '-n', '--number', dest='number', type='int',
        help='number of times each file should be read at the same time. Default: 30 if one input filename, 1 otherwise', default=0 )
    parser.add_option( '-a', '--all', dest='all', action='store_true',
        default=False,
        help='test all possible formats for each input file (convert to all of them and test)' )
    parser.add_option( '-s', '--subprocess', dest='subprocess',
        action='store_true', default=False,
        help='use subprocesses to run formats tests (with -a option). By default, they run in a single process, so a thread-related crash will end all tests (but will be easier to trace with a debugger).' )
    parser.add_option( '--silent', dest='silent', action='store_true',
        default=False,
        help='be less verbose in per-file tests (no -a option)' )
    parser.add_option( '-l', '--loop', dest='loop',
        action='store_true', help='loop the execution endlessly (until it crashes). Useful for debugging rare crashes' )

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
    else:
        num = options.number

    #import libxml2
    #libxml2.newTextReaderFilename( '/tmp/ra_head.gii.minf' )
    #import xml.parsers.expat
    #open( '/tmp/xml.xml', 'w' ).write( '<?xml version="1.0" encoding="utf-8" ?><grop></grop>' )
    #p = xml.parsers.expat.ParserCreate()
    #p.ParseFile( open( '/tmp/xml.xml' ) )

    from PyQt4 import QtGui
    app = QtGui.QApplication(sys.argv)

    doit = True
    while doit:
        if options.all:
            unsafe_formats = {}
            safe_formats = {}
            for filename in filenames:
                tested_formats = test_all_formats( filename, num,
                    separate_process=options.subprocess )
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
            aims_test_thread_read( filenames, verbose=not options.silent )

        if not options.loop:
            doit = False

