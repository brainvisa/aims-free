/*
 *  Non elastic AffineTransformation3d ( rotation + translation )
 */
#ifndef AIMS_RESAMPLING_MOTION_H
#define AIMS_RESAMPLING_MOTION_H

#include <aims/config/aimsdata_config.h>
#include <aims/transformation/affinetransformation3d.h>

namespace aims
{

  //------------------------------------//
 //  DecomposedAffineTransformation3d  //
//------------------------------------//


//-----------------------------------------------------------------------------
class DecomposedAffineTransformation3d
  : public AffineTransformation3d
{
public:

  DecomposedAffineTransformation3d();
  DecomposedAffineTransformation3d( const DecomposedAffineTransformation3d& other );
  inline virtual ~DecomposedAffineTransformation3d() {}
  DecomposedAffineTransformation3d &operator = ( const DecomposedAffineTransformation3d& other );
  virtual AffineTransformation3d &operator = ( const AffineTransformation3d& other );

  // Get shearing
  carto::VolumeRef<float>& shearing() { return _shear; }
  const carto::VolumeRef<float>& shearing() const { return _shear; }

  // Get scaling
  carto::VolumeRef<float>& scaling() { return _scaling; }
  const carto::VolumeRef<float>& scaling() const { return _scaling; }

  // Get rot
  carto::VolumeRef<float>& rot() { return _rot;}
  const carto::VolumeRef<float>& rot() const { return _rot;}

  virtual void setToIdentity() ;

  // AffineTransformation3d algebraic operation
  virtual void scale( const Point3df& sizeFrom, const Point3df& sizeTo );

  //Initialisation
  virtual void setRotationAffine( float rx, float ry, float rz,
                                  const Point3df & c = Point3df( 0.0 ) );
  //void setRotationVectorial( const Point3df& v1, const Point3df& v2 );
  void setShearing(float Cx, float Cy, float Cz ) ;
  void setScaling(float Sx, float Sy, float Sz ) ;
  void setRotation( float Rx, float Ry, float Rz ) ;
  void transAffine(Point3df cg = Point3df( 0.0 ) );

protected:

  carto::VolumeRef<float> _shear;
  carto::VolumeRef<float> _scaling;
  carto::VolumeRef<float> _rot;

};

}


typedef aims::AffineTransformation3d Motion;
typedef aims::DecomposedAffineTransformation3d DecomposedMotion;

#endif
