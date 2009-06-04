# Copyright CEA (2009)
#
#  This software and supporting documentation were developed by
#      CEA/DSV/SHFJ
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

import scipy, scipy.io
import numpy, sys
from soma import uuid
aims = sys.modules[ 'soma.aims']

__docformat__ = "epytext en"

def readSpmNormalization( matfilename, source=None, destref=None, srcref=None ):
  '''
  Read a SPM *_sn.mat normalization file and converts it to an Aims Motion.
  The converted transformation has for source the AIMS referential of the source
  image, and for destination the template referential of the SPM .mat file. All
  coordinates are in millimeters.

  The source image information may be provided either as its filename, its
  header object, or the image itself. It should carry the needed
  information: source volume storage_to_memory transformation matrix,
  voxel_size, etc.
  If None is passed as source (the default), then the source image name will
  be built from the .mat filename and will be read if found.

  @type matfilename: string
  @param matfilename: file name of the *_sn.mat normalization file to reading
  @type source: filename (string), or Volume (Volume or AimsData), or volume
  header (MappingType)
  @param source: file name of the *_sn.mat normalization file to reading
  @type destref: string or UUID
  @param destref: destination referential for the transformation. If not
  specified, none will be set. If provided as a symbolic name ('Talairach-MNI
  template-SPM'), it will be converted to an UUID string.
  @type srcref: string or UUID
  @param srcref: source referential for the transformation. If not specified,
  an attempt will be made to take it from the source image, otherwise it will
  not be set. If provided as a symbolic name ('Talairach-MNI template-SPM'), it
  will be converted to an UUID string.
  @rtype: aims.Motion object
  @return: the converted transformation
  '''
  if source is None:
    if matfilename.endswith( '_sn3d.mat' ):
      # SPM 99
      source = matfilename[ :-9 ]
    elif matfilename.endswith( '_sn.mat' ):
      # SPM >= 2
      source = matfilename[ :-7 ]
    elif matfilename.endswith( '.mat' ):
      # unlikely to work...
      source = matfilename[ :-4 ]
    else:
      # unlikely to work either...
      source = matfilename
  st = type( source )
  if st is type( '' ) or st is type( u'' ):
    f = aims.Finder()
    if not f.check( source ):
      raise IOError( 'could not read source image header for ' + source )
    hdr = f.header()
    if hdr.has_key( 'object_type' ) and hdr[ 'object_type' ] == 'genericobject':
      hdr = aims.read( source )
  elif hasattr( source, 'header' ):
    hdr = source.header()
  else:
    hdr = source
  try:
    s2m = aims.Motion( hdr[ 'storage_to_memory' ] )
    vsA = aims.Motion( numpy.diag( hdr[ 'voxel_size' ][:3] + [ 1. ] ) )
  except:
    raise RuntimeError( 'Source image header information could not be ' \
      'accessed.' )
  sn3d = scipy.io.loadmat( matfilename )
  Affine = aims.Motion( sn3d[ 'Affine' ] )
  if sn3d.has_key( 'VG' ):
    # SPM >= 2
    VG = sn3d[ 'VG' ]
    if [ int(x) for x in scipy.version.version.split('.')[:2] ] >= [ 0, 7 ]:
      VG = VG[0,0]
    MT = aims.Motion( VG.mat )
  else: # spm99
    MT = aims.Motion( sn3d[ 'MG' ] )
  # At is the compensation for Affine being used between Matlab arrays
  # indices, starting at 1, not 0
  At = numpy.mat( numpy.diag( [1.,1.,1.,1.] ) )
  At[:3,3] = numpy.transpose( numpy.mat( [-1.,-1.,-1.] ) )
  At = aims.Motion( At )
  # AtoT is Aims (mm) to Template (SPM mm) and is the output if
  # no normalized_volume is specified
  AtoT = MT * ( vsA * s2m * At * Affine ).inverse()
  ahdr = AtoT.header()
  if srcref is None and hdr.has_key( 'referential' ):
    srcref = hdr[ 'referential' ]
  if srcref:
    if srcref == aims.StandardReferentials.mniTemplateReferential():
      srcref = aims.StandardReferentials.mniTemplateReferentialID()
    elif srcref == aims.StandardReferentials.acPcReferential():
      srcref = aims.StandardReferentials.acPcReferentialID()
    try:
      uuid.Uuid( srcref )
      ahdr[ 'source_referential' ] = srcref
    except:
      print >> sys.stderr, 'source referential', srcref, \
      'cannot be converted to a valid UUID - not setting it.'
  if destref:
    if destref == aims.StandardReferentials.mniTemplateReferential():
      destref = aims.StandardReferentials.mniTemplateReferentialID()
    elif destref == aims.StandardReferentials.acPcReferential():
      destref = aims.StandardReferentials.acPcReferentialID()
    try:
      uuid.Uuid( destref )
      ahdr[ 'destination_referential' ] = destref
    except:
      print >> sys.stderr, 'destination referential', destref, \
      'cannot be converted to a valid UUID - not setting it.'

  return AtoT

