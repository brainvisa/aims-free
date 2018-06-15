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
#include <aims/data/data.h>
#include <aims/math/mathelem.h>
#include <iosfwd>

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

- forward declarations for the Motion class will not work any longer, since
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

  Point3dd transformVector( const Point3dd & vec ) const;
  Point3df transformVector( const Point3df & dir ) const;
  Point3dd transformVector( double x, double y, double z ) const;
  Point3df transformVector( float x, float y, float z ) const;
  Point3dd transformNormal( const Point3dd & dir ) const;
  Point3df transformNormal( const Point3df & dir ) const;
  Point3dd transformNormal( double x, double y, double z ) const;
  Point3df transformNormal( float x, float y, float z ) const;
  Point3dd transformUnitNormal( const Point3dd & dir ) const;
  Point3df transformUnitNormal( const Point3df & dir ) const;
  Point3dd transformUnitNormal( double x, double y, double z ) const;
  Point3df transformUnitNormal( float x, float y, float z ) const;

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

  /// transform AffineTransformation3d to a vector (useful for conversions and IO)
  std::vector<float> toVector() const;

  static AimsData<float> rotationaroundx(float rx) ;
  static AimsData<float> rotationaroundy(float ry) ;
  static AimsData<float> rotationaroundz(float rz) ;


protected:
  virtual Point3dd transformDouble( double x, double y, double z ) const;
  virtual Point3df transformFloat( float x, float y, float z ) const;

  virtual Point3dd transformVectorPoint3dd( const Point3dd & vec ) const;
  virtual Point3df transformVectorPoint3df( const Point3df & dir ) const;
  virtual Point3dd transformVectorDouble( double x, double y, double z ) const;
  virtual Point3df transformVectorFloat( float x, float y, float z ) const;
  virtual Point3dd transformNormalPoint3dd( const Point3dd & dir ) const;
  virtual Point3df transformNormalPoint3df( const Point3df & dir ) const;
  virtual Point3dd transformNormalDouble( double x, double y, double z ) const;
  virtual Point3df transformNormalFloat( float x, float y, float z ) const;

  Point3df _translation;
  // _rotation contient pour le moment la matrice affine en fait!!
  AimsData<float> _rotation;
  aims::PythonHeader  *_header;
};

  // Compose AffineTransformation3d
  AffineTransformation3d
      operator * ( const AffineTransformation3d& affineTransformation3d1,
                   const AffineTransformation3d& affineTransformation3d2 );


  inline Point3dd
  AffineTransformation3d::transformVector( double x, double y, double z ) const
  {
    return transformVectorDouble( x, y, z );
  }


  inline Point3df
  AffineTransformation3d::transformVector( float x, float y, float z ) const
  {
    return transformVectorFloat( x, y, z );
  }


  inline Point3df
  AffineTransformation3d::transformVector( const Point3df & pos ) const
  {
    return transformVectorPoint3df( pos );
  }


  inline Point3dd
  AffineTransformation3d::transformVector( const Point3dd & pos ) const
  {
    return transformVectorPoint3dd( pos );
  }


  inline Point3dd
  AffineTransformation3d::transformNormal( double x, double y, double z ) const
  {
    return transformNormalDouble( x, y, z );
  }


  inline Point3df
  AffineTransformation3d::transformNormal( float x, float y, float z ) const
  {
    return transformNormalFloat( x, y, z );
  }


  inline Point3df
  AffineTransformation3d::transformNormal( const Point3df & pos ) const
  {
    return transformNormalPoint3df( pos );
  }


  inline Point3dd
  AffineTransformation3d::transformNormal( const Point3dd & pos ) const
  {
    return transformNormalPoint3dd( pos );
  }


  inline Point3df
  AffineTransformation3d::transformVectorPoint3df( const Point3df & pos ) const
  {
    return transformVectorFloat( pos[0], pos[1], pos[2] );
  }


  inline Point3dd
  AffineTransformation3d::transformVectorPoint3dd( const Point3dd & pos ) const
  {
    return transformVector( pos[0], pos[1], pos[2] );
  }


  inline Point3df
  AffineTransformation3d::transformNormalPoint3df( const Point3df & pos ) const
  {
    Point3dd transformed = transformNormal( (double) pos[0], (double) pos[1],
                                             (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3dd
  AffineTransformation3d::transformNormalPoint3dd( const Point3dd & pos ) const
  {
    return transformNormal( pos[0], pos[1], pos[2] );
  }


  inline Point3df
  AffineTransformation3d::
  transformNormalFloat( float x, float y, float z ) const
  {
    Point3dd transformed = transformNormal( (double) x, (double) y,
                                             (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }


  inline Point3dd
  AffineTransformation3d::
  transformUnitNormal( double x, double y, double z ) const
  {
    return transformNormal( x, y, z ).normalize();
  }


  inline Point3df
  AffineTransformation3d::transformUnitNormal( const Point3df & pos ) const
  {
    Point3dd transformed
        = transformUnitNormal( (double) pos[0], (double) pos[1],
                                (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }


  inline Point3dd
  AffineTransformation3d::transformUnitNormal( const Point3dd & pos ) const
  {
    return transformUnitNormal( pos[0], pos[1], pos[2] );
  }


  inline Point3df
  AffineTransformation3d::
  transformUnitNormal( float x, float y, float z ) const
  {
    Point3dd transformed = transformUnitNormal( (double) x, (double) y,
                                                (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }

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
