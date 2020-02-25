from __future__ import absolute_import
from __future__ import print_function
from optparse import OptionParser
from soma.aims import texturetools
from soma import aims
import sys


parser = OptionParser('AimsExtractLabelsFromTexture.py -i input.gii(.tex) '
                      '-l old_label(integer) -n new_label(integer) -o output.gii(.tex)',
                      description='Extract labels from texture.')
parser.add_option('-i', '--tex', dest='gyri_tex', metavar='FILE',
                  help='input gyri texture')
parser.add_option('-l', '--ilabel', dest='ilabel', action='append',
                  help='input old label')
parser.add_option('-n', '--olabel', dest='olabel', type='int',
                  help='new label for old label extracted')
parser.add_option('-o', '--otex', dest='gyri_otex', metavar='FILE',
                  action='store', help='ouput gyri texture with extracted regions')

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


tex = aims.read(options.gyri_tex)
otex = texturetools.extractLabelsFromTexture(
    tex, options.ilabel, options.olabel)
aims.write(otex, options.gyri_otex)
