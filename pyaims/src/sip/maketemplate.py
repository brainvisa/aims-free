#!/usr/bin/env python

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

import sys
import os
import re
import sipconfig
from optparse import OptionParser
import subprocess
import platform

if sys.version_info[0] >= 3:
    def xreadlines(f):
        return f.readlines()
    xrange = range
else:
    def xreadlines(f):
        return f.xreadlines()


def convert_string_to_int(s):
    '''
    Allow to convert string with digit followed by non digits
    Useful to buil Qt version such as 3.3.8b
    '''
    for i in xrange(len(s)):
        if not s[i].isdigit():
            s = s[:i]
            break
    return int(s)


def makeTemplate(
    infile, outfile, types, templates={}, cpp='cpp -C', moc=None,
        quiet=0):
    # print('input :', infile)
    # print('output:', outfile)
    # print('types :', types)

    fi = open(infile)
    fo = open(outfile, 'w')
    if cpp:
        c = sipconfig.Configuration()
        cppcmd = cpp.split() + ['-DSIP_VERSION=' + '0x%06x' % c.sip_version]
        # determine Qt version
        try:
            qtdir = os.getenv('QTDIR')
            if not moc:
                if qtdir:
                    moc = os.path.join(qtdir, 'bin', 'moc')
                    if not os.path.exists(moc):
                        moc = 'moc'
                else:
                    moc = 'moc'
            moc_out = subprocess.Popen(
                [moc, '-v'], stdout=subprocess.PIPE,
                stderr=subprocess.PIPE).communicate()
            l = moc_out[1].decode()
            if l == '':
                l = moc_out[0].decode() # moc 5
                x = re.search('^.*moc ([0-9\.]+).*$', l).group(1)
            else:
                x = re.search('^.*\(Qt ([^\)]*)\).*$', l).group(1)
            qv = [convert_string_to_int(k) for k in x.split('.')]
            qver = qv[0] * 0x10000 + qv[1] * 0x100 + qv[2]
            cppcmd.append('-DQT_VERSION=' + hex(qver))
            # print >> sys.stderr, 'Qt version:', hex( qver )
        except Exception as e:
            if not quiet:
                print(e)
            pass  # Qt not available ?
        if not quiet:
            print(' '.join(cppcmd))
        if platform.system() == 'Windows':
            p = subprocess.Popen(cppcmd,
                                 stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        else:
            p = subprocess.Popen(cppcmd,
                                 stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
        fo2, cppout = (p.stdin, p.stdout)

    templatere = re.compile('(%(Template[0-9]+)([^%]*)%)')
    disableprere = re.compile('(^\s*)(#)(.*$)', re.M)
    enableprere = re.compile('(^\s*)(//!#!)(.*$)')
    preprocre = re.compile('(^\s*)(%#)(.*)%$')
    removeprere = re.compile('^\s*#.*$')

    for line in xreadlines(fi):
        lo = line
        templ = templatere.search(lo)
        while templ:
            tempn = templ.group(2)
            tempv = templ.group(3)
            # print('fount template:', tempn, tempv)
            t = templates.get(tempn)
            val = None
            if t:
                if not tempv:
                    val = t
                else:
                    tv = types.get(t)
                    if tv:
                        val = tv.get(tempv)
            if val is not None:
                # print(templ.group(0), '->', val)
                pos = templ.start(1) + len(val)
                # print(lo, 'pos:', pos)
                lo = templatere.sub(val, lo, 1)
            else:
                pos = templ.end(1)
            templ = templatere.search(lo, pos)

        lo = disableprere.sub('\\1//!#!\\3', lo)
        lo = preprocre.sub('\\1#\\3', lo)
        lo = lo.replace('%%', '')  # remove empty macros
        # feed the C preprocessor
        if not cpp:
            lo = enableprere.sub('\\1#\\3', lo)
            fo.write(lo)
        else:
            fo2.write(lo.encode())

    if cpp:
        # cppout = p.communicate()[0]
        fo2.close()
        for line in xreadlines(cppout):
            line = line.decode()
            # This is necessary to remove CR LF on windows
            if len(line) >= 2 and line[-2] == '\r':
                line = line[:-2] + '\n'
            lo = line
            if removeprere.match(lo):
                lo = ''
            lo = enableprere.sub('\\1#\\3', lo)
            fo.write(lo)

    fo.close()
    fi.close()


if __name__ == '__main__':
    parser = OptionParser(description='SIP template generation')
    parser.add_option('-i', '--input', dest='infile',
                      help='input .tpl SIP-like template')
    parser.add_option(
        '-o', '--output', dest='outfile', help='output SIP file')
    parser.add_option("-p", "--preprocess", dest='preprocess',
                      action="store_true", help="use C preprocessor")
    parser.add_option("-P", "--preprocessor", dest='preprocessor',
                      help="C preprocessor command (default: 'cpp -C')")
    parser.add_option("-t", "--templates", dest='templates',
                      action='append', help="templates list")
    parser.add_option("-s", "--subs", dest='subs',
                      help="substitutions file (python file)")
    parser.add_option('-m', '--moc', dest='moc',
                      help='Path to the moc executable.',
                      default=None)

    (options, args) = parser.parse_args()
    if not options.infile or not options.outfile or len(args) % 2 != 0:
        parser.parse_args(['-h'])

    infile = options.infile
    outfile = options.outfile
    cpp = options.preprocess
    cppc = options.preprocessor
    if cpp and not cppc:
        cppc = 'cpp -C'
    elif not cpp:
        cppc = None

    types = {}
    templates = {}
    if options.templates:
        if len(options.templates) % 2 != 0:
            print('template arguments go by pairs (key, value)')
            sys.exit(1)
        for i in xrange(int(len(options.templates) / 2)):
            templates[options.templates[i * 2]] = options.templates[i * 2 + 1]

    # print('templates:', options.templates)
    # print('subs:', options.subs)

    if options.subs:
        if sys.version_info[0] >= 3:
            code = compile(open(options.sub).read(), options.sub, 'exec')
            exec(code, globals(), globals())
        else:
            execfile(options.subs)
        types = typessub

    for i in xrange(int(len(args) / 2)):
        types[args[i * 2]] = args[i * 2 + 1]

    makeTemplate(infile, outfile, types, templates, cppc, moc=options.moc)
