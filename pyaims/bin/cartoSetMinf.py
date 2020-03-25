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
from six.moves import range


parser = OptionParser(description='set/create properties in a .minf '
                      'header')
parser.add_option('-i', '--inpput', dest='input', help='input .minf')
parser.add_option("-p", "--property", dest="property", action='append',
                  help="property to set")
parser.add_option('-v', '--value', dest='value', action='append',
                  help='property value (12, or "toto", or [ 1.2, 12 ]. '
                  'Be careful to put two quotes around numeric expressions '
                  'that are meant to be strings (one quote for the shell, '
                  'one for pyhton: "\'12.5\'") otherwise they will be taken '
                  'as numbers. '
                  'Multiple property/value pairs are allowed but must have '
                  'matching numbers')
parser.add_option('-e', '--expression', dest='expression',
                  help='expression to apply, ex: attributes.update( '
                  '{ "toto" : 12, "tutu" : [ 1.2, 12.5 ] } , or '
                  'del attributes[ "toto" ]')

(options, args) = parser.parse_args()

if args and not options.input:
    options.input = args[0]
    del args[0]
if options.value is None:
    options.value = []
if options.property is None:
    options.property = []
while args and len(options.value) < len(options.property):
    options.value.append(args[0])
    del args[0]
if len(args) % 2 == 0:
    while(args):
        options.property.append(args[0])
        del args[0]
        options.value.append(args[0])
        del args[0]

#print(options)
#print(args)

if args or not options.input \
        or len(options.property) != len(options.value):
    parser.parse_args(['-h'])

# look for correct file
input = options.input
if not input.endswith('.minf'):
    if os.path.exists(input + '.minf'):
        input += '.minf'

attributes = {}
try:
    exec(compile(open(input, "rb").read(), input, 'exec'))
except:
    if os.path.exists(input) and not input.endswith('.minf'):
        input += '.minf'
        try:
            exec(compile(open(input, "rb").read(), input, 'exec'))
        except:
            pass

for i in range(len(options.property)):
    try:
        a = eval(options.value[i])
    except:
        a = options.value[i]
    attributes[options.property[i]] = a
if options.expression:
    exec(options.expression)

print('MINF file:', input)
print(attributes)
f = open(input, 'w')
print('attributes =', repr(attributes), file=f)
f.close()
