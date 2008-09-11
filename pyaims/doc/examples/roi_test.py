import sys
from soma import aims
import math

# Read regions of interests from sys.argv[ 1 ]
roiIterator = aims.getRoiIterator( sys.argv[ 1 ] ).get()


# Read image from sys.argv[ 2 ]
reader = aims.Reader( {'Volume' : 'AimsData'} )
image = reader.read( sys.argv[2] )
interpolator = aims.getLinearInterpolator( image )


# Iterate on each region
while roiIterator.isValid():
  print roiIterator.regionName() + ':'
  maskIterator = roiIterator.maskIterator()
  print 'maskIterator:', maskIterator
  valid = 0
  invalid = 0
  sum = None
  sqsum = 0
  # Iterate on each point of a region
  print 'iter on maskIterator'
  while maskIterator.isValid():
    print ' point'
    p = maskIterator.valueMillimeters()
    print p
    # Check if the point is in the image limit
    print '4:', interpolator
    if interpolator.isValid( p ):
      print '6'
      value = interpolator.value( p )
      print '7:', value
      if sum is None:
        sum = value
      else:
        sum += value
      valid += 1
      sqsum += value * value
    else:
      invalid += 1
    print '8'
    maskIterator.next()
  print '  valid points:', valid
  print '  invalid points:', invalid
  if valid:
    mean = sum / float( valid )
    stddev = math.sqrt( sqsum / ( valid + 1 ) - mean * mean )
  else:
    mean = 'N/A'
    stddev = 'N/A'
  print '  mean value:', mean
  print '  std dev   :', stddev
  roiIterator.next()

