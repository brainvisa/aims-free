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

from soma import aims
import sys
import re
from optparse import OptionParser
import numpy as np

parser = OptionParser(description='Apply a formula to a set of homogeneous '
                      'images or textures (homogeneous means all of the same '
                      'data type)')
parser.add_option('-o', '--output', dest='output', help='output volume')
parser.add_option("-f", "--formula",
                  dest="formula",
                  help="image formula, ex: ( I1 * 2 + I2 * I3 ) / 1.2")
parser.add_option('-i', '--input', dest='filename', action='append',
                  help='input volume(s)')

(options, args) = parser.parse_args()

if args and not options.output:
    options.output = args[0]
    del args[0]
if args and not options.formula:
    options.formula = args[0]
    del args[0]
if options.filename is None:
    options.filename = []
options.filename += args

# print options
# print args

if not options.filename or not options.output or not options.formula:
    parser.parse_args(['-h'])

formula = re.sub('I([0-9]+)', 'image[\\1]', options.formula)
# print formula

# read images
image = [None]
objtype = None

r = aims.Reader({'Texture': 'TimeTexture'})
for x in options.filename:
    vol = r.read(x)
    t = vol.header()['data_type']
    if type(vol).__name__.startswith('Volume'):
        t_objtype = 'Volume'
    elif type(vol).__name__.startswith('TimeTexture'):
        t_objtype = 'TimeTexture'
        vol = np.asarray(vol)
    else:
        raise TypeError('unsupported data: %s' % type(vol).__name__)
    if objtype is None:
        objtype = t_objtype
    elif objtype != t_objtype:
        raise TypeError('heterogeneous data: %s and %s' % (objtype, t_objtype))
    image.append(vol)

# print image

result = eval(formula)
# print result

if objtype in ('TimeTexture',):
    # reconvert numpy object to aims object
    # transpose is needed for textures
    result = getattr(aims, objtype)(np.transpose(result))

print 'output type:', type(result).__name__
aims.write(result, options.output)
