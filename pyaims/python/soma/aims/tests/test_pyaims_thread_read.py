#!/usr/bin/env python

import threading
from soma import aims
import os
import sys
from optparse import OptionParser
import tempfile
import shutil
import soma.subprocess
import time


def aims_test_thread_read(filenames, verbose=True):

    class Loadfile(object):

        def __init__(self, filename, lock, objnum, verbose):
            self._filename = filename
            self.lock = lock
            self.objnum = objnum
            self.verbose = verbose

        def __call__(self):
            if self.verbose:
                print('reading %s...' % self._filename)
            obj = aims.read(self._filename)
            if self.verbose:
                print('read %s: %s' % (self._filename, str(type(obj))))
            self.lock.acquire()
            self.objnum[0] += 1
            self.lock.release()

    aims.carto.PluginLoader.load()  # do this once in main thread

    threads = []
    lock = threading.RLock()
    # objnum is a list, not an int, because the counter has to be shared
    # between all threads: a list is, an int is not
    objnum = [0]

    starttime = time.time()
    for fname in filenames:
        thread = threading.Thread(
            target=Loadfile(fname, lock, objnum, verbose))
        thread.start()
        threads.append(thread)

    for thread in threads:
        thread.join()

    duration = time.time() - starttime
    print('finished. Read %d / %d objects in %.3f seconds.'
          % (objnum[0], len(filenames), duration))
    nmissing = len(filenames) - objnum[0]
    if nmissing != 0:
        print('Not all objects were loaded, %d missing.' % nmissing)
        raise RuntimeError('Not all objects were loaded, %d missing.'
                           % nmissing)


def _convertFileFormat(aimsobj, directory, prefix, format, is_soma=False):
    if is_soma:
        exts = somaio_extensions(aimsobj, format)
    else:
        exts = aims.Finder.extensions(format)
    if len(exts) == 0:
        return None
    exts2 = [x for x in exts if x != '']
    if len(exts) != len(exts2):
        exts2.append('')
    exts = exts2
    del exts2
    formatok = False
    for ext in exts:
        if ext == '':
            newfilename = os.path.join(directory, prefix)
        else:
            newfilename = os.path.join(directory,
                                       '.'.join((prefix, ext)))
        try:
            aims.write(aimsobj, newfilename, format=format)
            if not os.path.exists(newfilename):
                for f in os.listdir(directory):
                    if not f.endswith('.minf') \
                            and (ext == '' or f.endswith('.' + ext)):
                        newfilename = os.path.join(directory, f)
                        break
                else:
                    shutil.rmtree(directory)
                    os.mkdir(directory)
                    continue
            f = aims.Finder()
            if f.check(newfilename) and f.format() == format:
                formatok = True
                break
            else:
                # print('could not read', newfilename)
                shutil.rmtree(directory)
                os.mkdir(directory)
        except Exception:
            shutil.rmtree(directory)
            os.mkdir(directory)
            continue
    if formatok:
        return newfilename
    return None


def somaio_formats(aimsobj):
    try:
        fclass = getattr(aims.carto,
                         'FormatDictionary_%s' % aims.typeCode(aimsobj))
    except Exception:
        if isinstance(aimsobj, aims.carto.GenericObject):
            fclass = aims.carto.FormatDictionary_Object
        else:
            return
    formats = fclass.writeFormats()
    read_formats = [f for f in fclass.readFormats() if f not in formats]
    exts = fclass.writeExtensions()
    rexts = fclass.readExtensions()
    ext_by_format = dict([(f, []) for f in formats])
    ext_non_written = dict([(f, []) for f in read_formats])
    for ext, flist in exts.items():
        for f in flist:
            ext_by_format[f].append(ext)
    for ext, flist in rexts.items():
        for f in flist:
            if f in read_formats:
                ext_non_written[f].append(ext)
    return ext_by_format, ext_non_written


def somaio_extensions(aimsobj, format):
    try:
        fclass = getattr(aims.carto,
                         'FormatDictionary_%s' % aims.typeCode(aimsobj))
    except Exception:
        if isinstance(aimsobj, aims.carto.GenericObject):
            fclass = aims.carto.FormatDictionary_Object
        else:
            return []
    exts = fclass.writeExtensions()
    exts_for_format = [ext for ext, formats in exts.items()
                       if format in formats]
    return exts_for_format


def test_all_formats(filename, number=30, separate_process=False):
    f = aims.Finder()
    if not f.check(filename):
        raise IOError('%f is not readable' % filename)
    otd = f.objectType()
    dt = f.dataType()
    ot_equiv = [{'Volume', 'CartoVolume'}, ]
    otgs = [g for g in ot_equiv if otd in g]
    otg = [otd]
    if otgs:
        otg = otgs[0]
    aimsobj = aims.read(filename)
    formats = set()
    for ot in otg:
        formats_l = aims.IOObjectTypesDictionary.formats(ot, dt)
        formats.update(formats_l)
    formats = sorted(formats)
    soma_io_formats, soma_io_not_tested_formats = somaio_formats(aimsobj)
    if f.format() not in formats and f.format() not in soma_io_formats:
        # the input format cannot be written, however we already have a file
        # to test it, so move it from non_tested to tested ones
        soma_io_formats[f.format()] = soma_io_not_tested_formats[f.format()]
        del soma_io_not_tested_formats[f.format()]
    success = True
    unsafe_formats = []
    safe_formats = []
    not_tested_formats = list(soma_io_not_tested_formats.keys())
    all_formats = list(zip(formats, [False] * len(formats))) \
        + [(f, True) for f in soma_io_formats]
    print('formats:', all_formats)
    for format, is_soma in all_formats:
        # JP2 writer in Qt (4.8.1 at least) systematically crashes.
        if format in ('JP2'):
            continue
        print('testing: %s / %s, format: %s' % (otd, dt, format))
        if format == f.format():
            directory = None
            newfilename = filename
        else:
            directory = tempfile.mkdtemp(prefix='aims_thread_test')
        try:
            if format == f.format():
                newfilename = filename
            else:
                newfilename = _convertFileFormat(aimsobj, directory,
                                                 'aims_test', format, is_soma)
                if not newfilename:
                    print('could not generate format', format)
                    not_tested_formats.append(format)
                    # shutil.rmtree( directory )
                    continue
            print('testing read on %s...' % newfilename)
            try:
                if separate_process:
                    soma.subprocess.check_call(
                        [sys.executable, '-m',
                         'soma.aims.tests.test_pyaims_thread_read',
                         '-i', newfilename, '-n', str(number), '--silent'])
                else:
                    aims_test_thread_read([newfilename] * number,
                                          verbose=False)
                print('Passed.')
                safe_formats.append(format)
            except Exception:
                print('format %s is unsafe.' % format)
                success = False
                unsafe_formats.append(format)
        finally:
            if directory is not None:
                shutil.rmtree(directory)
    print('All done for %s / %s. Success =' % (otd, dt), success)
    if not success:
        return ({otd: unsafe_formats}, {otd: safe_formats},
                {otd: not_tested_formats})
    return {}, {otd: safe_formats}, {otd: not_tested_formats}


if __name__ == '__main__':

    parser = OptionParser(
        description='Perform tests of threaded concurrent loading of aims '
        'objects in pyaims')
    parser.add_option('-i', '--input', dest='infiles',
                      help='files to be read concurrently', action='append',
                      default=[])
    parser.add_option('-n', '--number', dest='number', type='int',
                      help='number of times each file should be read at the '
                      'same time. Default: 30 if one input filename, '
                      '1 otherwise', default=0)
    parser.add_option('-a', '--all', dest='all', action='store_true',
                      default=False,
                      help='test all possible formats for each input file '
                      '(convert to all of them and test)')
    parser.add_option('-s', '--subprocess', dest='subprocess',
                      action='store_true', default=False,
                      help='use subprocesses to run formats tests (with -a '
                      'option). By default, they run in a single process, so '
                      'a thread-related crash will end all tests (but will be '
                      'easier to trace with a debugger).')
    parser.add_option('--silent', dest='silent', action='store_true',
                      default=False,
                      help='be less verbose in per-file tests (no -a option)')
    parser.add_option('-l', '--loop', dest='loop',
                      action='store_true', help='loop the execution endlessly '
                      '(until it crashes). Useful for debugging rare crashes')

    options, args = parser.parse_args()

    filenames = options.infiles + args
    if len(filenames) == 0:
        print('no input files.')
        parser.parse_args(['-h'])
    if options.number == 0:
        if len(filenames) == 1 or options.all:
            num = 30
        else:
            num = 1
    else:
        num = options.number

    from soma.qt_gui.qt_backend import QtWidgets
    app = QtWidgets.QApplication(sys.argv)

    doit = True
    while doit:
        if options.all:
            unsafe_formats = {}
            safe_formats = {}
            not_tested_formats = {}
            for filename in filenames:
                tested_formats = test_all_formats(
                    filename, num, separate_process=options.subprocess)
                unsafe_formats.update(tested_formats[0])
                safe_formats.update(tested_formats[1])
                not_tested_formats.update(tested_formats[2])
            if len(not_tested_formats) != 0:
                print('The following format cound not be tested:')
                print(not_tested_formats)
            if len(unsafe_formats) != 0:
                print('Results:')
                print('unsafe formats:')
                print(unsafe_formats)
                print('safe formats:')
                print(safe_formats)
                raise RuntimeError('Some tests failed.')
            else:
                print('OK.')
                print('safe formats:')
                print(safe_formats)
        else:
            filenames = filenames * num
            aims_test_thread_read(filenames, verbose=not options.silent)

        if not options.loop:
            doit = False
