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

from __future__ import absolute_import
import sys
import os
import types
import stat
import shutil
import filecmp
import glob
import re
from optparse import OptionParser
import platform
import subprocess
import six
from six.moves import filter
from six.moves import range


parser = OptionParser(description='Preprocess a template file to generate '
                      'typed SIP inpuyt files')
parser.add_option('-s', '--sourcepath', dest='sourcepath',
                  help='source path to pyaims SIP directory containing '
                  'maketemplate.py [default: same as this program]',
                  default=os.path.dirname(sys.argv[0]))
parser.add_option('-i', '--input', dest='input',
                  help='input generated types file '
                  '[default: generatedtypes.py]',
                  default='generatedtypes.py')
parser.add_option('-p', '--tpl', dest='tpldir',
                  help='directory where .tpl files are', default='')
parser.add_option('-t', '--types', dest='typessub', action='append',
                  help='types substitution file [default: typessub.py in '
                  'the current directory and/or in sourcepath]')
parser.add_option('-o', '--output', dest='output',
                  help='output directory for generated .sip files '
                  '[default:same as sources]', default=None)
parser.add_option('-c', '--cleanup', dest='cleanup', action='store_true',
                  help='cleanup obsolete .sip files in the output directory '
                  '[default:false]', default=False)
parser.add_option('-l', '--listing', dest='listFilesOnly',
                  action='store_true',
                  help='change the display : show the generated file list'
                  '[default:false]', default=False)
parser.add_option('-m', '--moc', dest='moc',
                  help='Path to the moc executable.',
                  default=None)
parser.add_option('--no-preprocess', dest='preprocess',
                  action='store_false', help='use C preprocessor '
                  '[default:true]', default=True)
parser.add_option("-P", "--preprocessor", dest='preprocessor',
                  help="C preprocessor command [default: 'cpp -C']")
parser.add_option("-T", "--target-platform", dest='target_platform',
                  help="Target platform [default: %s]" % 
                  '-'.join([platform.system().lower(), 
                            platform.architecture()[0][:2]]))

(options, args) = parser.parse_args()
if args:
    parser.parse_args(['-h'])

cpp = options.preprocess
cppc = options.preprocessor
if cpp and not cppc:
    cppc = 'cpp -C'
elif not cpp:
    cppc = None

if not options.typessub:
    p = [os.path.join(options.sourcepath, 'typessub.py'), 'typessub.py']
    options.typessub = list(filter(os.path.exists, p))

pyaimssip = options.sourcepath
sys.path.insert(0, '.')
sys.path.insert(1, pyaimssip)
from maketemplate import makeTemplate
import maketemplate


def convert_string_to_int(s):
    '''
    Allow to convert string with digit followed by non digits
    Useful to buil Qt version such as 3.3.8b
    '''
    for i in range(len(s)):
        if not s[i].isdigit():
            s = s[:i]
            break
    return int(s)

# determine Qt version
try:
    qtdir = os.getenv('QTDIR')
    if options.moc:
        moc = options.moc
    else:
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
    qt_version = [convert_string_to_int(k) for k in x.split('.')]
except Exception as e:
    if not options.listFilesOnly:
        print(e)
    qt_version = [4, 0, 0]  # Qt not available ?

# read generatedtypes file
# expected to fill in the 'todo' dictionary variable
if sys.version_info[0] >= 3:
    with open(options.input) as f:
        code = compile(f.read(), options.input, 'exec')
    exec(code, globals(), globals())
else:
    exec(compile(open(options.input, "rb").read(), options.input, 'exec'), globals(), globals())

if options.tpldir == '':
    dir_name = os.path.dirname(options.input)
else:
    dir_name = options.tpldir

# read typessub files
typesmtime = 0
for x in options.typessub:
    typesmtime = max(typesmtime, os.stat(x)[stat.ST_MTIME])
    if sys.version_info[0] >= 3:
        with open(x) as f:
            code = compile(f.read(), x, 'exec')
        exec(code, globals(), globals())
    else:
        exec(compile(open(x, "rb").read(), x, 'exec'), globals(), globals())

typesmtime = max(typesmtime,
                 os.stat(maketemplate.__file__)[stat.ST_MTIME])
if os.path.exists(sys.argv[0]):
    typesmtime = max(typesmtime, os.stat(sys.argv[0])[stat.ST_MTIME])

outfiles = []
allok = True

for file, tps in todo.items():
    # print(file, ':', tps)
    infile = os.path.join(dir_name, file + '.tpl')
    if not os.path.exists(infile):
        infile = os.path.join(pyaimssip, infile)
        if not os.path.exists(infile):
            infile = os.path.join(pyaimssip, file + '.tpl')
    if options.output is not None:
        ofilebase = os.path.join(options.output, os.path.basename(file))
    else:
        ofilebase = file
    for x in tps:
        if isinstance(x, six.string_types):
            templates = {'Template1': x}
            ts = typessub[x].get('typecode')
            if not ts:
                ts = x
            ofile = ofilebase + '_' + ts + '.sip'
        else:
            i = 1
            ofile = ofilebase
            templates = {}
            for y in x:
                templates['Template' + str(i)] = y
                i += 1
                ts = typessub[y].get('typecode')
                if not ts:
                    ts = y
                ofile += '_' + ts
            ofile += '.sip'
        outfiles.append(ofile)
        try:
            # print('templates:', templates)
            done = False
            if os.path.exists(ofile):
                otmpfile = ofile + '.tmp'
                s1 = os.stat(infile)[stat.ST_MTIME]
                s2 = os.stat(ofile)[stat.ST_MTIME]
                if s1 <= s2 and typesmtime < s2:
                    done = True
                    if not options.listFilesOnly:
                        print('skipping', ofile, '- up to date',
                              file=sys.stderr)
            else:
                otmpfile = ofile
            if not done:
                if not options.listFilesOnly:
                    sys.stdout.write('generating ' + ofile)
                makeTemplate(
                    infile, otmpfile, typessub, templates, cpp=cppc,
                    moc=options.moc, quiet=options.listFilesOnly)
                if ofile != otmpfile:
                    if not filecmp.cmp(ofile, otmpfile):
                        shutil.copyfile(otmpfile, ofile)
                        if not options.listFilesOnly:
                            print(' - differs')
                    else:
                        if not options.listFilesOnly:
                            print(' - unchanged')
                        # copy it anyway because sip.py will take care of it
                        shutil.copyfile(otmpfile, ofile)
                    os.unlink(otmpfile)
                else:
                    if not options.listFilesOnly:
                        print()
        except Exception as e:
            print('error in generation of', ofile, ':', file=sys.stderr)
            print(e, file=sys.stderr)
            allok = False

if options.listFilesOnly:
    print(";".join(outfiles).replace('\\', '/'))

if allok and options.cleanup and options.output:
    if not options.listFilesOnly:
        print('cleanup obsolete files...')
    files = glob.glob(os.path.join(options.output, '*.sip'))
    for f in files:
        if f not in outfiles \
                and stat.S_IMODE(os.stat(f)[stat.ST_MODE]) & 0o200:
            if not options.listFilesOnly:
                print('deleting', f)
            os.unlink(f)

if not allok:
    sys.exit(1)
