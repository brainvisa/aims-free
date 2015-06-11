# -*- coding: utf-8 -*-
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
from soma import aims
import math

# Read regions of interests from sys.argv[ 1 ]
roiIterator = aims.getRoiIterator(sys.argv[1])


# Read image from sys.argv[ 2 ]
reader = aims.Reader({'Volume': 'AimsData'})
image = reader.read(sys.argv[2])
interpolator = aims.getLinearInterpolator(image)


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
        if interpolator.isValid(p):
            print '6'
            value = interpolator.value(p)
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
        mean = sum / float(valid)
        if valid >= 2:
            stddev = math.sqrt((sqsum - mean * mean) / (valid - 1))
        else:
            stddev = math.sqrt((sqsum - mean * mean) / valid)
    else:
        mean = 'N/A'
        stddev = 'N/A'
    print '  mean value:', mean
    print '  std dev   :', stddev
    roiIterator.next()
