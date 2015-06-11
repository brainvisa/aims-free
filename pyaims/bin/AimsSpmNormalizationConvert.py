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

import sys
import os
from optparse import OptionParser
from soma import aims

parser = OptionParser(description='Reads the affine part of a SPM '
                      'normalization file (_sn.mat) and converts it to an AIMS .trm file.')
parser.add_option('-i', '--input', dest='input', metavar='FILE',
                  help='SPM normalization file (*_sn.mat)')
parser.add_option("-s", "--source", dest="source",
                  action='store', metavar='SOUREFILE',
                  help="source image which has been used for normalization, but not "
                  "resampled in the template space")
parser.add_option('-o', '--output', dest='output', action='store',
                  metavar='TRMFILE', help='output transformationfile *.trm')
parser.add_option('-d', '--destref', dest='destref',
                  help='set destination referential name/UUID. Default: none')
parser.add_option('--srcref', dest='srcref',
                  help='set source referential name/UUID. Default: try to guess it from '
                  'source image')

(options, args) = parser.parse_args()

if args and not options.input and len(args) > 0:
    options.input = args[0]
    del args[0]
if options.source is None and len(args) > 0:
    options.source = args[0]
    del args[0]
if options.output is None and len(args) > 0:
    options.output = args[0]
    del args[0]
if options.destref is None and len(args) > 0:
    options.destref = args[0]
    del args[0]
if options.srcref is None and len(args) > 0:
    options.srcref = args[0]
    del args[0]
if len(args) > 0:
    parser.parse_args(['-h'])
if options.input is None or options.source is None or options.output is None:
    parser.parse_args(['-h'])


trans = aims.readSpmNormalization(options.input, options.source,
                                  destref=options.destref, srcref=options.srcref)
aims.write(trans, options.output)
