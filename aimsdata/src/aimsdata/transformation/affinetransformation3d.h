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

#include <aims/config/aimsdata_config.h>
#include <soma-io/transformation/affinetransformation3d_base.h>
#include <aims/data/data.h>
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

  //---------------------------------------------------------------------------

    //--------------------------//
   //  AffineTransformation3d  //
  //--------------------------//
  //--------------------------------------------------------------------------
  /** Affine 3D transformation

  WARNING: this is the old Motion class of Aims 3.x, which has been renamed and
  changed in Aims 4.0. Since Aims 4.7, most of the code is in soma-io where it
  is used to transform voxels coordinates in IO code. This additional layer
  mainly adds compatibility with older code which uses AimsData for rotation()
  and Point3df for translation(), and the header.

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

  - there are several overloaded transform() methods taking double, or float
    numbers, or Point3df or Point3dd arguments. As there were formerly only
    float and Point3df arguments, ambiguities may raise from calling them with
    mixed double/float arguments

  - the base soma::Transformation class and its typedef in aims namespace
    introduces a name ambiguity with anatomist anatomist::Transformation class,
    so it now requires to handle namespaces carefully.

  \section transformation_aims_4_7 Changes in AIMS 4.7

  In Aims and Soma-IO 4.7, it was needed to move transformations to Soma-IO,
  because they are used in IO code that is moving to soma-io. To do to, we
  also had to move AimsVector and typedefs Point3df etc. Then several changes
  had to be done:

  - Transformation and Transformation3d are now in the namespace soma. They are
    still typedef'd in aims for compatibility, but programmers should use them
    in soma now.

  - The AffineTransformation3d has been split in two parts,
    soma::AffineTransformation3dBase which allready does pretty much
    everything, which is in soma-io and used this way there, and
    AffineTransformation3d which is still here in aims, and merely adds
    compatibility methods converting with aims types (AimsData, Volume,
    Quaternion).

  - AffineTransformation3d::rotation does not return a reference any longer.
    Code using it as a reference should be modified. As the returned AimsData
    object is itself a reference to the transformation array, changging this
    is normally merely a matter of removing the "&", the object will still be
    usable to modify the transformation.

  - AffineTransformation3d::translation does not return a reference any longer.
    Code using it as a reference should be modified. This modification however
    requires a bit more changes than for rotation() because the returned
    Point3df cannot be used to modity the transformation itself. Instead you
    can use AffineTransformation3d::setTranslation(), or
    AffineTransformation3dBase::matrix()(i, 3) to modify the translation in
    place.

  - AffineTransformation3d::rotation() and AffineTransformation3d::translation
    are left for partial compatibility with older code, but should be
    considered deprecated. They are less efficient as they used to be, because
    the returned objects are built on-the-fly and are not the native storage
    for the transform anymore. Instead, AffineTransformation3dBase::matrix()
    should be used, it has the same functionalities and allows to address the
    full 4x4 matrix, with same convention as rotation(): accessors are
    (line, column), thus the translation is (line, 3).

  - AffineTransformation3d::setTranslation() has changed behavior: it used to
    add the coordinates to the current translation in the matrix, which was
    wrong or at least unclear. It was almost never used anyway. The behavior is
    now what is expected: it sets the translation part of the matrix, and
    may be used to replace the former expressions
    trans.translation() = Point3df(..) which do not work any longer.
  */
  class AffineTransformation3d : public soma::AffineTransformation3dBase
  {
  public:

    /// Create an identity transformation
    AffineTransformation3d();
    AffineTransformation3d( const AffineTransformation3d& other );
    AffineTransformation3d( const AffineTransformation3dBase& other );
    /// Create a AffineTransformation3d from a 4x4 matrix given as a line vector
    AffineTransformation3d( const std::vector<float> & mat );
    /// Create a AffineTransformation3d from a 4x4 matrix given as a line vector in an Object
    AffineTransformation3d( const carto::Object mat );
    AffineTransformation3d( const aims::Quaternion & quat );
    virtual ~AffineTransformation3d();
    virtual AffineTransformation3d &
    operator = ( const AffineTransformation3d& other );
    virtual AffineTransformation3d &
    operator = ( const std::vector<float> & mat );
    virtual AffineTransformation3d &
    operator = ( const carto::Object mat );
    virtual AffineTransformation3d &
    operator = ( const aims::Quaternion & quat );

    virtual bool operator == ( const AffineTransformation3d & ) const;

    AffineTransformation3d &
    operator *= ( const AffineTransformation3dBase & trans );
    // AffineTransformation3d & operator += ( const AffineTransformation3d & trans );
    virtual AffineTransformation3d operator - () const;

    AffineTransformation3d inverse() const;
    std::unique_ptr<Transformation3d> getInverse() const CARTO_OVERRIDE;

    aims::PythonHeader* header() { return _header; }
    const aims::PythonHeader* header() const { return _header; }
    void setHeader( aims::PythonHeader* ph );

    inline Point3df translation();
    inline Point3df translation() const;

    inline AimsData<float> rotation();
    inline const AimsData<float> rotation() const;

    // get the matrix as a volume
    inline carto::VolumeRef<float> affine();
    inline const carto::VolumeRef<float> affine() const;

    virtual void setRotationAffine( float rx, float ry, float rz,
                                    const Point3df &cg = Point3df( 0.0 )  );
    //virtual void setRotationVectorial( const Point3df& v1, const Point3df& v2 );

    static AimsData<float> rotationaroundx(float rx) ;
    static AimsData<float> rotationaroundy(float ry) ;
    static AimsData<float> rotationaroundz(float rz) ;


  protected:
    aims::PythonHeader  *_header;
  };

  // Compose AffineTransformation3d
  AffineTransformation3d
      operator * ( const AffineTransformation3d& affineTransformation3d1,
                   const AffineTransformation3d& affineTransformation3d2 );

  Point3df AffineTransformation3d::translation()
  {
    return Point3df( _matrix[12], _matrix[13], _matrix[14] );
  }

  Point3df AffineTransformation3d::translation() const
  {
    return Point3df( _matrix[12], _matrix[13], _matrix[14] );
  }

  AimsData<float> AffineTransformation3d::rotation()
  {
    // return a 3x3 view into the 4x4 matrix
    carto::VolumeRef<float> matrix(
      new carto::Volume<float>( 4, 4, 1, 1, &*_matrix.begin() ) );

    carto::VolumeRef<float> view(
      matrix, carto::Volume<float>::Position4Di( 0, 0, 0, 0 ),
      carto::Volume<float>::Position4Di( 3, 3, 1, 1 ) );

    return AimsData<float>( view );
  }

  const AimsData<float> AffineTransformation3d::rotation() const
  {
    // return a 3x3 view into the 4x4 matrix
    carto::VolumeRef<float> matrix(
      new carto::Volume<float>( 4, 4, 1, 1,
                                const_cast<float *>( &*_matrix.begin() ) ) );
    carto::VolumeRef<float> view(
      matrix, carto::Volume<float>::Position4Di( 0, 0, 0, 0 ),
      carto::Volume<float>::Position4Di( 3, 3, 1, 1 ) );

    return AimsData<float>( view );
  }

  carto::VolumeRef<float> AffineTransformation3d::affine()
  {
    carto::VolumeRef<float> matrix(
      new carto::Volume<float>( 4, 4, 1, 1, &*_matrix.begin() ) );
    return matrix;
  }

  const carto::VolumeRef<float> AffineTransformation3d::affine() const
  {
    carto::VolumeRef<float> matrix(
      new carto::Volume<float>( 4, 4, 1, 1,
                                const_cast<float *>( &*_matrix.begin() ) ) );
    return matrix;
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
