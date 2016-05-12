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
from optparse import OptionParser
from soma import aims, uuid

parser = OptionParser(description='extract an affine transformation '
                      'information in an object header into a transformation file')
parser.add_option('-i', '--input', dest='input', metavar='FILE',
                  help='object to get transformation from (volume, mesh, etc).')
parser.add_option("-o", "--output", dest="transformation",
                  action='store', metavar='TRMFILE',
                  help="output transformation file (*.trm)")
parser.add_option('-n', '--number', dest='transnum', action='store',
                  type='int',
                  help='get transformation information as the n-th transformation. '
                  'Default: -1, last one in list')
parser.add_option('-d', '--destination', dest='destination',
                  help='set destination referential name/UUID. Default: try to get it '
                  'from the referentials info of the input file header')
parser.add_option('-s', '--source', dest='source',
                  help='set source referential name/UUID. Default: try to get it from the '
                  'referential info of the input file header')

parser.set_defaults(transnum=-1)
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
if options.input is None or options.transformation is None:
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

if 'transformations' not in hdr:
    print('Object header has no transformations', file=sys.stderr)
    sys.exit(1)

trans = hdr['transformations']
if options.transnum < 0:
    transnum = len(trans) + options.transnum
else:
    transnum = options.transnum
try:
    tr = trans[transnum]
except:
    print('No such transformation in header.', file=sys.stderr)
mot = aims.Motion(tr)
dest = None
if options.destination is not None:
    dest = options.destination
elif 'referentials' in hdr:
    refs = hdr['referentials']
    if transnum < len(refs):
        dest = refs[transnum]
if dest is not None:
    if dest == aims.StandardReferentials.mniTemplateReferential():
        dest = aims.StandardReferentials.mniTemplateReferentialID()
    elif dest == aims.StandardReferentials.acPcReferential():
        dest = aims.StandardReferentials.acPcReferentialID()
    try:
        uuid.Uuid(dest)
        mot.header()['destination_referential'] = dest
    except:
        print('destination referential', dest,
              'cannot be converted to a valid UUID - not setting it.',
              file=sys.stderr)
src = None
if options.source is not None:
    src = options.source
elif 'referential' in hdr:
    src = hdr['referential']
if src is not None:
    if src == aims.StandardReferentials.mniTemplateReferential():
        src = aims.StandardReferentials.mniTemplateReferentialID()
    elif src == aims.StandardReferentials.acPcReferential():
        src = aims.StandardReferentials.acPcReferentialID()
    try:
        uuid.Uuid(src)
        mot.header()['source_referential'] = src
    except:
        print('source referential', src,
              'cannot be converted to a valid UUID - not setting it.',
              file=sys.stderr)

aims.write(mot, options.transformation)
