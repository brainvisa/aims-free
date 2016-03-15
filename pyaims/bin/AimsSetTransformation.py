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

import sys
import os
from optparse import OptionParser
from soma import aims

parser = OptionParser(description='set an affine transformation '
                      'information in an object header from a transformation file')
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
    print >> sys.stderr, 'If no transformation is specified (erase mode), ' \
        'the transformation number to erase sould be specified: -t and -n ' \
        'options cannot be both omitted.'
    sys.exit(1)
if options.input is None:
    parser.parse_args(['-h'])

# print options
# print args

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
    print >> sys.stderr, 'cannot read input object', options.input
    sys.exit(1)

if options.transformation is None:
    # erase mode
    if not hdr.has_key('transformations'):
        print 'No transformation to erase. Nothing done.'
        sys.exit(0)
    trans = hdr['transformations']
    if ( options.transnum >= 0 and len( trans ) <= options.transnum ) \
            or (options.transnum < 0 and len(trans) < -options.transnum):
        print >> sys.stderr, 'No such transformation in header.'
        sys.exit(1)
    if options.transnum < 0:
        num = len(trans) + options.transnum
    else:
        num = options.transnum
    del trans[num]
    if hdr.has_key('referentials'):
        refs = hdr['referentials']
        if len(refs) > num:
            del refs[num]

else:
    # insert / replace mode
    trans = aims.read(options.transformation)
    dest = options.destination
    if dest is None:
        th = trans.header()
        if th.has_key('destination_referential'):
            dest = th['destination_referential']
            if dest == aims.StandardReferentials.mniTemplateReferentialID():
                dest = aims.StandardReferentials.mniTemplateReferential()
    if hdr.has_key('transformations'):
        translist = hdr['transformations']
    else:
        translist = []
        hdr['transformations'] = translist
    if options.transnum is None:
        transnum = len(translist)
        translist.append(trans.toVector())
    else:
        if ( options.transnum >= 0 and len( translist ) <= options.transnum ) \
            or (options.transnum < 0 and
                len(translist) < -options.transnum):
            print >> sys.stderr, 'No such transformation in header. Appending.'
            transnum = len(translist)
            translist.append(trans.toVector())
        else:
            transnum = options.transnum
            if transnum < 0:
                transnum = len(translist) + transnum
            translist[transnum] = trans.toVector()
    if hdr.has_key('referentials'):
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
if hdr.has_key('file_type'):
    format = hdr['file_type']
else:
    format = None
aims.write(obj, options.input, format=format)
