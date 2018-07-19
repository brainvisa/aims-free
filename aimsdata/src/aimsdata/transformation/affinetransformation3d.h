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
#ifndef AIMS_TRANSFORMATION_AFFINETRANSFORMATION3D_H
#define AIMS_TRANSFORMATION_AFFINETRANSFORMATION3D_H

#include <aims/transformation/transformation.h>
#include <aims/config/aimsdata_config.h>
#include <aims/data/fastAllocationData.h>
#include <aims/math/mathelem.h>
#include <iostream>

namespace aims
{
  class AffineTransformation3d;
  class PythonHeader;
  class Quaternion;

  void transformBoundingBox
      ( const AffineTransformation3d &AffineTransformation3d,
        const Point3df & pmin1, const Point3df & pmax1, Point3df & pmin2,
        Point3df & pmax2 );

  std::ostream&
    operator << ( std::ostream& os,
                  const AffineTransformation3d& thing );

//-----------------------------------------------------------------------------

  //--------------------------//
 //  AffineTransformation3d  //
//--------------------------//
//-----------------------------------------------------------------------------
/** Affine 3D transformation

WARNING: this is the old Motion class of Aims 3.x, which has been renamed and
changed in Aims 4.0.

A new transformation classes tree has been setup to allow non-linear
transformations: see Transformation and Transformation3d.
AffineTransformation3d now inherits Transformation3d.

A typedef is still provided for backward compatibility, in
aims/resampling/motion.h (the former location of the Motion class) and should
minimize inconvenience when compiling old code which used Motion. However a few
API differences may cause compilation problems:

- forward declatations for the Motion class will not work any longer, since
  Motion is not a class anymore but a typedef. It is still possible to replace:
\code class Motion;
\endcode
  by:
\code namespace aims { class AffineTransformation3d; }
typedef aims::AffineTransformation3d Motion;
\endcode
  but of course it is better to use directly the AffineTransformation3d class
  under its real name.

- the Motion::transform_normal() method is now transformUnitNormal()

- there are now several overloaded transform() methods taking double, or float
  numbers, or Point3df or Point3dd arguments. As there were formerly only float
  and Point3df arguments, ambiguities may raise from calling them with mixed
  double/float arguments

- the base aims::Transformation class introduces a name ambiguity with
  anatomist anatomist::Transformation class, so it now requires to handle
  namespaces carefully.

- the Motion DataTypeCode has also changed to AffineTransformation3d.
*/
class AffineTransformation3d : public Transformation3d
{
public:

  /// Create an identity transformation
  AffineTransformation3d();
  AffineTransformation3d( const AffineTransformation3d& other );
  /// Create a AffineTransformation3d from a 4x4 matrix given as a line vector
  AffineTransformation3d( const std::vector<float> & mat );
  /// Create a AffineTransformation3d from a 4x4 matrix given as a line vector in an Object
  AffineTransformation3d( const carto::Object mat );
  AffineTransformation3d( const aims::Quaternion & quat );
  virtual ~AffineTransformation3d();
  virtual AffineTransformation3d &operator = ( const AffineTransformation3d& other );
  virtual AffineTransformation3d &operator = ( const std::vector<float> & mat );
  virtual AffineTransformation3d &operator = ( const carto::Object mat );
  virtual AffineTransformation3d &operator = ( const aims::Quaternion & quat );

  virtual bool operator == ( const AffineTransformation3d & );

  AffineTransformation3d & operator *= ( const AffineTransformation3d & trans );
  // AffineTransformation3d & operator += ( const AffineTransformation3d & trans );
  virtual AffineTransformation3d operator - () const;

  aims::PythonHeader* header() { return _header; }
  const aims::PythonHeader* header() const { return _header; }
  void setHeader( aims::PythonHeader* ph );

  inline Point3df& translation() { return _translation; }
  inline const Point3df& translation() const { return _translation; }

  inline AimsData<float>& rotation() { return _rotation; }
  inline const AimsData<float>& rotation() const { return _rotation; }

  virtual bool isIdentity() const;
  virtual void setToIdentity() ;

  // AffineTransformation3d algebraic operation
  AffineTransformation3d inverse() const;
  virtual void scale( const Point3df& sizeFrom, const Point3df& sizeTo );
  /// true if the transformation is direct, false if it changes orientation
  bool isDirect() const;

  //Initialisation
  void setTranslation(Point3df trans);
  virtual void setRotationAffine( float rx, float ry, float rz, 
                                  const Point3df &cg = Point3df( 0.0 )  );
  //virtual void setRotationVectorial( const Point3df& v1, const Point3df& v2 );

  /// transform AffineTransformation3d to a line vector (useful for
  /// conversions and IO)
  std::vector<float> toVector() const;
  /// transform AffineTransformation3d to a column vector (useful for
  /// conversions to OpenGL matrices)
  std::vector<float> toColumnVector() const;
  /// transform a column vector to an AffineTransformation3d (useful for
  /// conversions from OpenGL matrices)
  void fromColumnVector( const std::vector<float> & vec )
  {
    return fromColumnVector( &vec[0] );
  }
  /// transform a column vector to an AffineTransformation3d (useful for
  /// conversions from OpenGL matrices)
  void fromColumnVector( const float* vec );

  static AimsData<float> rotationaroundx(float rx) ;
  static AimsData<float> rotationaroundy(float ry) ;
  static AimsData<float> rotationaroundz(float rz) ;


protected:
  virtual Point3dd transformDouble( double x, double y, double z ) const;
  virtual Point3df transformFloat( float x, float y, float z ) const;

  virtual Point3dd transformVectorDouble( double x, double y, double z ) const;
  virtual Point3df transformVectorFloat( float x, float y, float z ) const;


  Point3df _translation;
  // _rotation contient pour le moment la matrice affine en fait!!
  AimsFastAllocationData<float> _rotation;
  aims::PythonHeader  *_header;
};

  // Compose AffineTransformation3d
  AffineTransformation3d
      operator * ( const AffineTransformation3d& affineTransformation3d1,
                   const AffineTransformation3d& affineTransformation3d2 );

}




#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template <> class DataTypeCode< aims::AffineTransformation3d >
  {
  public:
    static std::string objectType()
    { return "AffineTransformation3d"; }
    static std::string dataType()
    { return "VOID"; }
    static std::string name() 
    { 
      return "AffineTransformation3d";
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

#endif
