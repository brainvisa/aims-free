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
 *  Non elastic motion ( rotation + translation )
 */
#ifndef AIMS_RESAMPLING_MOTION_H
#define AIMS_RESAMPLING_MOTION_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>
#include <aims/math/mathelem.h>
#include <cartobase/smart/rcptr.h>
#include <iostream>

class Motion;
namespace aims
{
  class PythonHeader;
  class Quaternion;

  void transformBoundingBox( const Motion &motion, const Point3df & pmin1,
		const Point3df & pmax1, Point3df & pmin2, Point3df & pmax2 );
}

//-----------------------------------------------------------------------------
AIMSDATA_API std::ostream& operator << ( std::ostream& os, 
					 const Motion& thing );


  //----------//
 //  Motion  //
//----------//

//-----------------------------------------------------------------------------
class AIMSDATA_API Motion : public virtual carto::RCObject
{
public:

  Motion();
  Motion( const Motion& other );
  /// Create a Motion from a 4x4 matrix given as a line vector
  Motion( const std::vector<float> & mat );
  /// Create a Motion from a 4x4 matrix given as a line vector in an Object
  Motion( const carto::Object mat );
  Motion( const aims::Quaternion & quat );
  virtual ~Motion();
  virtual Motion &operator = ( const Motion& other );
  virtual Motion &operator = ( const std::vector<float> & mat );
  virtual Motion &operator = ( const carto::Object mat );
  virtual Motion &operator = ( const aims::Quaternion & quat );

  virtual bool operator == ( const Motion & );

  Motion & operator *= ( const Motion & trans );
  // Motion & operator += ( const Motion & trans );
  virtual Motion operator - () const;

  aims::PythonHeader* header() { return _header; }
  const aims::PythonHeader* header() const { return _header; }
  void setHeader( aims::PythonHeader* ph );

  inline Point3df& translation() { return _translation; }
  inline const Point3df& translation() const { return _translation; }

  inline AimsData<float>& rotation() { return _rotation; }
  inline const AimsData<float>& rotation() const { return _rotation; }

  Point3df transform( float x, float y, float z ) const;
  inline Point3df transform( const Point3df & p ) const
  {
    return transform( p[0], p[1], p[2] );
  }

  // Transform normalized normal data
  Point3df transform_normal( float x, float y, float z ) const;
  // Transform normalized normal data
  inline Point3df transform_normal( const Point3df & p ) const
  {
    return transform_normal( p[0], p[1], p[2] );
  }

  bool isIdentity() const;
  virtual void setToIdentity() ;

  // Motion algebraic operation
  Motion inverse() const;
  virtual void scale( const Point3df& sizeFrom, const Point3df& sizeTo );
  /// true if the transformation is direct, false if it changes orientation
  bool isDirect() const;

  //Initialisation
  void setTranslation(Point3df trans);
  virtual void setRotationAffine( float rx, float ry, float rz, 
                                  const Point3df &cg = Point3df( 0.0 )  );
  //virtual void setRotationVectorial( const Point3df& v1, const Point3df& v2 );

  /// transform Motion to a vector (useful for conversions and IO)
  std::vector<float> toVector() const;

  // Output
  friend std::ostream& operator << ( std::ostream& os, const Motion& thing );


  static AimsData<float> rotationaroundx(float rx) ;
  static AimsData<float> rotationaroundy(float ry) ;
  static AimsData<float> rotationaroundz(float rz) ;


protected:

  Point3df _translation;
  // _rotation contient pour le moment la matrice affine en fait!!
  AimsData<float> _rotation;
  aims::PythonHeader  *_header;
};


// Compose motion
Motion operator * ( const Motion& motion1, const Motion& motion );




  //--------------------//
 //  DecomposedMotion  //
//--------------------//


//-----------------------------------------------------------------------------
class AIMSDATA_API DecomposedMotion : public Motion
{
public:

  DecomposedMotion();
  DecomposedMotion( const DecomposedMotion& other );
  inline virtual ~DecomposedMotion() {}
  DecomposedMotion &operator = ( const DecomposedMotion& other );
  virtual Motion &operator = ( const Motion& other );

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

  // Motion algebraic operation
  virtual void scale( const Point3df& sizeFrom, const Point3df& sizeTo );

  //Initialisation
  void setRotationAffine( float rx, float ry, float rz, 
                          Point3df c = Point3df( 0.0 ) );  
  //void setRotationVectorial( const Point3df& v1, const Point3df& v2 );
  void setShearing(float Cx, float Cy, float Cz ) ;
  void setScaling(float Sx, float Sy, float Sz ) ;
  void transAffine(Point3df cg = Point3df( 0.0 ) );


  // Output
  friend std::ostream& operator << ( std::ostream& os, const Motion& thing );

protected:

  AimsData<float> _shear;
  AimsData<float> _scaling;
  AimsData<float> _rot;

};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template <> class DataTypeCode< Motion >
  {
  public:
    static std::string objectType()
    { return "Motion"; }
    static std::string dataType()
    { return "VOID"; }
    static std::string name() 
    { 
      return "Motion";
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

#endif
