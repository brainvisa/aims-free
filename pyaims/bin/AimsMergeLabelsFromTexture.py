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
from optparse import OptionParser
from soma.aims import texturetools
from soma import aims
import sys


parser = OptionParser('AimsMergeLabelsFromTexture.py -i input.gii(.tex) '
                      '-l old_label(integer) -n new_label(integer) -o output.gii(.tex)',
                      description='Change/Merge labels from texture.')
parser.add_option('-i', '--tex', dest='input_tex', metavar='FILE',
                  help='input gyri texture')
parser.add_option('-l', '--ilabel', dest='ilabel', action='append',
                  help='input old label')
parser.add_option('-n', '--olabel', dest='olabel', type='int',
                  help='new label (erased/replaced old label)')
parser.add_option('-o', '--otex', dest='output_tex', metavar='FILE',
                  action='store', help='ouput gyri texture with new regions')

(options, args) = parser.parse_args()
    
if args and not options.input_tex and len(args) > 0:
    options.input_tex = args[0]
    del args[0]
if options.ilabel is None and len(args) > 0:
    options.ilabel = args[0]
    del args[0]
if options.olabel is None and len(args) > 0:
    options.olabel = args[0]
    del args[0]
if options.output_tex is None and len(args) > 0:
    options.output_tex = args[0]
    del args[0]
if len(args) > 0:
    parser.parse_args(['-h'])

if not options.input_tex or not options.output_tex \
    or not options.ilabel or options.olabel is None:
    print("Error: input texture or ouput texture or labels are missing")
    parser.parse_args(['-h'])


tex = aims.read(options.input_tex)
print('ilabel', options.ilabel)
#print('type: ', type(options.ilabel))
print('olabel', options.olabel)
otex = texturetools.mergeLabelsFromTexture(
    tex, options.ilabel, options.olabel)
aims.write(otex, options.output_tex)
