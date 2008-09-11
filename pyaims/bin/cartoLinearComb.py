#!/usr/bin/env python

# Copyright CEA and IFR 49 (2000-2005)
#
#  This software and supporting documentation were developed by
#      CEA/DSV/SHFJ and IFR 49
#      4 place du General Leclerc
#      91401 Orsay cedex
#      France
#
# This software is governed by the CeCILL license version 2 under 
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the 
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
# knowledge of the CeCILL license version 2 and that you accept its terms.

from soma import aims
import sys, re
from optparse import OptionParser

parser = OptionParser( description = 'Apply a formula to a set of homogeneous '
                       'images (homogeneous means all of the same data type)' )
parser.add_option( '-o', '--output', dest='output', help='output volume' )
parser.add_option( "-f", "--formula",
                   dest="formula", 
                   help="image formula, ex: ( I1 * 2 + I2 * I3 ) / 1.2" )
parser.add_option( '-i', '--input', dest='filename', action='append', 
                   help='input volume(s)' )

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
    parser.parse_args( [ '-h' ] )

formula = re.sub( 'I([0-9]+)', 'image[\\1]', options.formula )
# print formula

# read images
image = [ None ]
typesord = ( 'RGBA', 'RGB', 'DOUBLE', 'FLOAT', 'S32', 'U32', 'S16', 
             'U16', 'U8', 'S8' )
restype = len( typesord ) - 1

r = aims.Reader( {} )
for x in options.filename:
    vol = r.read( x )
    t = vol.header()[ 'data_type' ]
    for i in xrange( restype ):
      if typesord[i] == t:
        restype = i
    image.append( vol )

# convert all volumes to result type
print 'output type:', typesord[ restype ]
for i in xrange( 1, len( image ) ):
  vol = image[i]
  c = aims.__getattribute__( 'ShallowConverter_' + type( vol ).__name__ + \
                             '_Volume_' + typesord[ restype ] )
  image[i] = c()( vol )

# print image

result = eval( formula )
# print result

w = aims.Writer()
w.write( result, options.output )


