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

# FSL matrixes seem to transform from/to internal refs, like Aims but with
# a different convention:
# X: right -> left
# Y: back -> front
# Z: bottom -> top
# which appears to be Y and Z flipped compared to Aims

from soma import aims
import numpy, types

def fslMatToTrm( matfile, srcimage, dstimage ):
  if type( srcimage ) in types.StringTypes:
    f = aims.Finder()
    f.check( srcimage )
    im1 = f.header()
  else: # try srcimage as a volume
    try:
      im1 = srcimage.header()
    except: # otherwise it is considered to already be a header
      im1 = srcimage
  if type( dstimage ) in types.StringTypes:
    f = aims.Finder()
    f.check( dstimage )
    im2 = f.header()
  else: # try dstimage as a volume
    try:
      im2 = dstimage.header()
    except: # otherwise it is considered to already be a header
      im2 = dstimage

  height1 = im1['voxel_size'][1] * ( im1['volume_dimension'][1] - 1 )
  height2 = im2['voxel_size'][1] * ( im2['volume_dimension'][1] - 1 )
  size1 = im1['voxel_size'][2] * ( im1['volume_dimension'][2] - 1 )
  size2 = im2['voxel_size'][2] * ( im2['volume_dimension'][2] - 1 )
  flip1 = aims.Motion()
  flip1.rotation().setValue( -1., 1,1 )
  flip1.translation()[1] = height1
  flip1.rotation().setValue( -1., 2,2 )
  flip1.translation()[2] = size1
  flip2 = aims.Motion()
  flip2.rotation().setValue( -1., 1,1 )
  flip2.translation()[1] = height2
  flip2.rotation().setValue( -1., 2,2 )
  flip2.translation()[2] = size2

  mat = [ x.split() for x in open( matfile ).readlines() ]
  mot = aims.Motion()
  mot.fromMatrix( numpy.mat( mat ) )

  trm = flip2.inverse() * mot * flip1

  return trm
