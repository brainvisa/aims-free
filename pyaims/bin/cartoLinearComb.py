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
from soma import aims
from soma.aims import lazy_read_data
import sys
import re
from optparse import OptionParser
import numpy
np = numpy  # allow both names for numpy

parser = OptionParser(description='Apply a formula to a set of homogeneous '
                      'images or textures (homogeneous means all of the same '
                      'data type)')
parser.add_option('-o', '--output', dest='output', help='output volume')
parser.add_option("-f", "--formula",
                  dest="formula",
                  help="image formula, ex: ( I1 * 2 + I2 * I3 ) / 1.2 . Image "
                  "(or other objects) can be named I1, I2... or image[1], "
                  "image[2] etc. Indices normally start at 1 (kind of "
                  "matlab-style) but can start at 0 if the -z option is used. "
                  "A formula is basically a python expression, thus can use "
                  "anything supported in python expressions. ex: sum(images). "
                  "Numpy may be used (as np), and numpy results can be "
                  "converted to volumes: np.asarray(I1) ** 2, or np.sqrt(I1). "
                  "'image' is the list of images, with index starting at "
                  "either 1 or 0 (if -z is used). 'images' is also the list "
                  "of images, with index starting at 0. If -z is not used, "
                  "'image' contains an additional first element, which "
                  "is None: image[0] is None, thus sum(image) will fail, "
                  "whereas sum(images) is OK.")
parser.add_option('-i', '--input', dest='filename', action='append',
                  help='input volume(s)')
parser.add_option('-l', '--lazy', action='store_true',
                  help='use lazy reading and release of objects during the '
                  'formula evaluation. With this option, each image is loaded '
                  'when used, and released once one operation has been '
                  'performed with it. It allows to process large lists of '
                  'images without loading all of them in memory, but will '
                  'read them several times if they are used several times in '
                  'the formula. (see aims.lazy_read_data.LasyReadData '
                  'python class for details)')
parser.add_option('-t', '--threads', type='int', default=1,
                  help='use threaded preloading when iterating over the list of images (see soma.aims.lazy_read_data.PreloadIterator python class for details). Implies -l, and only useful when using an iteration over the list of volumes, such as in the formula "sum(image)". In such an iteration volumes in the later iterations can be preloaded using threads, making the process much faster. The number of threads / preloads is specified with this option. 0 means guess the number of CPUs of the current machine. Default is 1: no thraading/preloading')
parser.add_option('-z', '--zero', dest='zero', action='store_true',
                  help='start indexing images at index 0 (instead of 1 by default): I0, I1 etc and image[0], image[1] etc.')

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

if options.threads != 1:
    options.lazy = True

# print(options)
# print(args)

if not options.filename or not options.output or not options.formula:
    parser.parse_args(['-h'])

formula = re.sub('I([0-9]+)', 'image[\\1]', options.formula)
# print(formula)

# read images
image = []
if options.zero:
    i0 = 0
else:
    i0 = 1
objtype = None

r = aims.Reader({'Texture': 'TimeTexture'})
for x in options.filename:
    if options.lazy:
        vol = aims.lazy_read_data.LazyReadData(x, nops=1)
        f = aims.Finder()
        f.check(x)
        t = f.header()['data_type']
        ot = f.header()['object_type']
        if ot.startswith('Volume'):
            t_objtype = 'Volume'
        elif ot.startswith('TimeTexture'):
            t_objtype = 'TimeTexture'
            vol = np.asarray(vol)
        else:
            raise TypeError('unsupported data: %s' % type(vol).__name__)
    else:
        vol = r.read(x)
        t = vol.header()['data_type']
        ot = type(vol).__name__
    if ot.startswith('Volume'):
        t_objtype = 'Volume'
    elif ot.startswith('TimeTexture'):
        t_objtype = 'TimeTexture'
        vol = np.asarray(vol)
    else:
        raise TypeError('unsupported data: %s' % ot)
    if objtype is None:
        objtype = t_objtype
    elif objtype != t_objtype:
        raise TypeError('heterogeneous data: %s and %s'
                        % (objtype, t_objtype))
    image.append(vol)

if options.threads != 1:
    # list with preloading iterator
    image = aims.lazy_read_data.PreloadList(image, npreload=options.threads)

images = image  # same but more logical when using the list ("sum(images)")
if not options.zero:
    image = type(image)(image)
    image.insert(0, None)

# print(image)

result = eval(formula)
if isinstance(result, lazy_read_data.LazyReadData):
    result = result.data
# print(result)

if objtype in ('TimeTexture',):
    # reconvert numpy object to aims object
    # transpose is needed for textures
    result = getattr(aims, objtype)(np.transpose(result))

elif objtype in ('Volume',) and isinstance(result, np.ndarray):
    # volumes have been converted to numpy in the expression
    # convert it back to a volume, using the first image header
    result = aims.Volume(result)
    if len(image) > i0:
        result.copyHeaderFrom(image[i0].header())

print('output type:', type(result).__name__)
aims.write(result, options.output)
