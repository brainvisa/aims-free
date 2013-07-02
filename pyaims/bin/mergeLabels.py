from optparse import OptionParser
from soma.aims import texturetools
from soma import aims
import sys

def parseOpts(argv):
  description = "Merge labels from texture \
                 usage : python mergeLabels.py input.gii(.tex) old_label( integer ) new_label( integer ) "
  parser = OptionParser( description )
  parser.add_option( '-i', '--tex', dest = 'gyri_tex', metavar = 'FILE',
                    help = 'input gyri texture')
  parser.add_option( '-l', '--ilabel', dest = 'ilabel', action='append',
                    help = 'input old label' )
  parser.add_option( '-n', '--olabel', dest = 'olabel', type='int',
                    help = 'input new label (erased/replaced old label)' )
  parser.add_option( '-o', '--otex', dest = 'gyri_otex', metavar = 'FILE', action='store',
                    help = 'ouput gyri texture with merge regions')
  return parser, parser.parse_args(argv)

def main():
  parser, (options, args) = parseOpts(sys.argv)
  tex = aims.read( options.gyri_tex )
  print 'ilabel', options.ilabel
  print 'type: ', type(options.ilabel)
  otex = texturetools.mergeLabelsFromTexture( tex, options.ilabel, options.olabel )
  aims.write( otex, options.gyri_otex )

if __name__ == "__main__": main()