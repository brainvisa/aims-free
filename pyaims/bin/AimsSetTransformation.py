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

from __future__ import absolute_import
import sys
import os
from optparse import OptionParser
from soma import aims

parser = OptionParser(
    description='set an affine transformation '
        'information in an object header from a transformation file.\n'
        'The command can append, remove, or replace an existing '
        'transformation in the input file. Note that some formats will force '
        'certain transformations to exist or to respect certain criteria, '
        'such as the qform in nifti files which should be rotation + '
        'translation only. In such a case it is possible that the output file '
        'will contain some additional transformations which are not "wanted" '
        'here (they are added by the IO layer upon writing).')
parser.add_option('-i', '--input', dest='input', metavar='FILE',
                  help='object to set transformation info on (volume, mesh, etc).')
parser.add_option("-t", "--transformation", dest="transformation",
                  action='store', metavar='TRMFILE',
                  help="transformation file (*.trm). If not specified "
                  "and -n option is used, then erase the n-th transformation in list.")
parser.add_option('-n', '--number', dest='transnum', action='store',
                  type='int',
                  help='set transformation information as the n-th transformation. '
                  'Default: append at end of list')
parser.add_option('-d', '--destination', dest='destination',
                  help='set destination referential name/UUID. Default: try to get it '
                  'from the transformation file header')
parser.add_option('-o', '--output', dest='output',
                  help='output object file name. Default: overwrite input')

(options, args) = parser.parse_args()

if args and not options.input and len(args) > 0:
    options.input = args[0]
    del args[0]
if options.transformation is None and len(args) > 0:
    options.transformation = args[0]
    del args[0]
if options.transnum is None and len(args) > 0:
    options.transnum = args[0]
    del args[0]
if options.destination is None and len(args) > 0:
    options.destination = args[0]
    del args[0]
if len(args) > 0:
    parser.parse_args(['-h'])
if options.transnum is None and options.transformation is None:
    print('If no transformation is specified (erase mode), '
          'the transformation number to erase sould be specified: -t and -n '
          'options cannot be both omitted.', file=sys.stderr)
    sys.exit(1)
if options.input is None:
    parser.parse_args(['-h'])

# print(options)
# print(args)

# look for correct file
input = options.input

obj = None
try:
    obj = aims.read(input)
    hdr = obj.header()
except:
    # object not readable, just read the .minf
    if not input.endswith('.minf') and os.path.exists(input + '.minf'):
        input += '.minf'
        try:
            obj = aims.read(input)
            hdr = obj
        except:
            pass
if obj is None:
    print('cannot read input object', options.input, file=sys.stderr)
    sys.exit(1)

if options.transformation is None:
    # erase mode
    if 'transformations' not in hdr:
        print('No transformation to erase. Nothing done.')
        sys.exit(0)
    trans = hdr['transformations']
    if ( options.transnum >= 0 and len( trans ) <= options.transnum ) \
            or (options.transnum < 0 and len(trans) < -options.transnum):
        print('No such transformation in header.', file=sys.stderr)
        sys.exit(1)
    if options.transnum < 0:
        num = len(trans) + options.transnum
    else:
        num = options.transnum
    del trans[num]
    if 'referentials' in hdr:
        refs = hdr['referentials']
        if len(refs) > num:
            del refs[num]

else:
    # insert / replace mode
    trans = aims.read(options.transformation)
    dest = options.destination
    if dest is None:
        th = trans.header()
        if 'destination_referential' in th:
            dest = th['destination_referential']
            if dest == aims.StandardReferentials.mniTemplateReferentialID():
                dest = aims.StandardReferentials.mniTemplateReferential()
    if 'transformations' in hdr:
        translist = hdr['transformations']
    else:
        translist = []
        hdr['transformations'] = translist
    if options.transnum is None:
        transnum = len(translist)
        translist.append(trans.toVector())
    else:
        if (options.transnum >= 0 and len(translist) <= options.transnum) \
            or (options.transnum < 0 and
                len(translist) < -options.transnum):
            print('No such transformation in header. Appending.',
                  file=sys.stderr)
            transnum = len(translist)
            translist.append(trans.toVector())
        else:
            transnum = options.transnum
            if transnum < 0:
                transnum = len(translist) + transnum
            translist[transnum] = trans.toVector()
    if 'referentials' in hdr:
        refs = hdr['referentials']
    else:
        refs = []
        hdr['referentials'] = refs
    while len(refs) <= transnum:
        refs.append('Coordinates aligned to another file or to anatomical '
                    'truth')
    if dest is not None:
        refs[transnum] = dest

# write back updated information
if 'format' in hdr:
    format = hdr['format']
elif 'file_type' in hdr:
    format = hdr['file_type']
else:
    format = None
output = options.output
if not output:
    output = input
aims.write(obj, output, format=format)
