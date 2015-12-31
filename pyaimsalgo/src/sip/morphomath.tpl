

class MorphoGreyLevel_%Template1typecode%
{
%TypeHeaderCode
#include <aims/morphology/operatormorpho.h>

#ifndef PYAIMSALGOSIP_MORPHOGREYLEVEL_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_MORPHOGREYLEVEL_%Template1typecode%_DEFINED
        typedef aims::MorphoGreyLevel<%Template1% > MorphoGreyLevel_%Template1typecode%;
#endif
%End

%Docstring
  Grey-level mathematical morphology.

  when enabled, on binary images, the chamfer-based morphomath is
  used instead of the grey-level one. This is the default as it is
  way faster (see setChamferBinaryMorphoEnabled).

  In binary mode, the input data (volume) should contain a border of "sufficent" size. The border size will be checked, and a new volume with larger border will be temporarily allocated and used if needed, but it is more efficient (and consumes less memory) if this border is already allocated in the input image.

  Grey-level operations do not need a border in input images, the test is included in the algorithm (which makes it even slower).

  See the method neededBorderWidth().

  This class thus regroups all basic morphological operations, and makes obsolete direct calls to AimsMorphoChamferErosion(), AimsMorphoChamferDilation(), AimsMorphoChamferClosing(), AimsMorphoChamferOpening().
%End

public:
  MorphoGreyLevel_%Template1typecode%();
  virtual ~MorphoGreyLevel_%Template1typecode%();

  rc_ptr_Volume_%Template1typecode%
    doErosion( const rc_ptr_Volume_%Template1typecode% & dataIn, float radius );
%Docstring
  doErosion(dataIn, radius)

  Erosion operation

  Parameters
  ----------
  dataIn: rc_ptr_Volume_%Template1typecode%
      volume to perform the operation on
  radius: float
      radius of the morphological operation (in mm)

  Returns
  -------
  result: rc_ptr_Volume_%Template1typecode%
%End

  rc_ptr_Volume_%Template1typecode%
    doDilation( const rc_ptr_Volume_%Template1typecode% & dataIn, float radius );
%Docstring
  doDilation(dataIn, radius)

  Dilation operation

  Parameters
  ----------
  dataIn: rc_ptr_Volume_%Template1typecode%
      volume to perform the operation on
  radius: float
      radius of the morphological operation (in mm)

  Returns
  -------
  result: rc_ptr_Volume_%Template1typecode%
%End

  rc_ptr_Volume_%Template1typecode%
    doClosing( const rc_ptr_Volume_%Template1typecode% & dataIn, float radius );
%Docstring
  doClosing(dataIn, radius)

  Closing operation (dilation + erosion)

  Parameters
  ----------
  dataIn: rc_ptr_Volume_%Template1typecode%
      volume to perform the operation on
  radius: float
      radius of the morphological operation (in mm)

  Returns
  -------
  result: rc_ptr_Volume_%Template1typecode%
%End

  rc_ptr_Volume_%Template1typecode%
    doOpening( const rc_ptr_Volume_%Template1typecode% & dataIn, float radius );
%Docstring
  doOpening(dataIn, radius)

  Opening operation (erosion + dilation)

  Parameters
  ----------
  dataIn: rc_ptr_Volume_%Template1typecode%
      volume to perform the operation on
  radius: float
      radius of the morphological operation (in mm)

  Returns
  -------
  result: rc_ptr_Volume_%Template1typecode%
%End

  bool isChamferBinaryMorphoEnabled() const;
%Docstring
  True if binary moprhological operations are allowed (when binary input data are detected)
%End

  void setChamferBinaryMorphoEnabled( bool x );
%Docstring
  setChamferBinaryMorphoEnabled(enabled)

  Enable or disable binary moprhological operations (when binary input data are detected)

  Parameters
  ----------
  enabled: bool
%End

  float chamferFactor() const;
%Docstring
  chamfer factor is used to store chamfer distances as int with a sufficient precision. Used only in binary operations. The default is 50.
%End

  void setChamferFactor( float x );
%Docstring
  setChamferFactor(factor)

  Set the chamfer distance factor, used in binary operations.
%End

  Point3d chamferMaskSize() const;
%Docstring
  Get the chamfer mask size used in binary operations. The default is 3x3x3.

  Returns
  -------
  mask_size: Point3df
      size in the 3 directions, in voxels
%End

  void setChamferMaskSize( const Point3d & p );
%Docstring
  setChamferMaskSize(size)

  Set the chamfer mask size (for binary operations)

  Parameters
  ----------
  size: Point3d
      mask size triplet, in voxels
%End

  int neededBorderWidth() const;
%Docstring
  border width needed to perform chamfer mask operations, in binary mode (int).
%End
};

