/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

/*
 *  Non elastic AffineTransformation3d ( rotation + translation )
 */
#ifndef AIMS_RESAMPLING_MOTION_H
#define AIMS_RESAMPLING_MOTION_H

#include <aims/config/aimsdata_config.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/data/data.h>

namespace aims
{

  //------------------------------------//
 //  DecomposedAffineTransformation3d  //
//------------------------------------//


//-----------------------------------------------------------------------------
class AIMSDATA_API DecomposedAffineTransformation3d
  : public AffineTransformation3d
{
public:

  DecomposedAffineTransformation3d();
  DecomposedAffineTransformation3d( const DecomposedAffineTransformation3d& other );
  inline virtual ~DecomposedAffineTransformation3d() {}
  DecomposedAffineTransformation3d &operator = ( const DecomposedAffineTransformation3d& other );
  virtual AffineTransformation3d &operator = ( const AffineTransformation3d& other );

  // Get shearing
  AimsData<float>& shearing() { return _shear; }
  const AimsData<float>& shearing() const { return _shear; }

  // Get scaling
  AimsData<float>& scaling() { return _scaling; }
  const AimsData<float>& scaling() const { return _scaling; }

  // Get rot
  AimsData<float>& rot() { return _rot;}
  const AimsData<float>& rot() const { return _rot;}

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

  AimsData<float> _shear;
  AimsData<float> _scaling;
  AimsData<float> _rot;

};

}


typedef aims::AffineTransformation3d Motion;
typedef aims::DecomposedAffineTransformation3d DecomposedMotion;

#endif
