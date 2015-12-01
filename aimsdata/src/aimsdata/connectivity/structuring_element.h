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

#ifndef AIMS_CONNECTIVITY_STRUCTURING_ELEMENT_2_H
#define AIMS_CONNECTIVITY_STRUCTURING_ELEMENT_2_H

//--- aims -------------------------------------------------------------------
#include <aims/vector/vector.h>                                     // Point3d
//--- carto ------------------------------------------------------------------
#include <cartobase/smart/rcptr.h>                           // smart pointers
#include <cartobase/smart/refwrapper.h>               // wrapper to references
//--- std --------------------------------------------------------------------
#include <string>
#include <vector>
#include <set>
#include <map>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class Volume;
}
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // STRUCTURING ELEMENT: BASE CLASS
  //==========================================================================
  /// \brief aims::StructuringElement class representing a 3D structuring
  /// element.
  ///
  /// The aims::StructuringElement class is used  to get neighbors of a
  /// coordinate in a 3D grid.
  ///
  /// Each type of structuring element is a class deriving from
  /// aims::StructuringElement. There are currently two intermediate
  /// interface classes which are aims::strel::Shape and
  /// aims::strel::Connectivity. Two factory classes are also there
  /// to get specific structuring elements from string identifiers.
  ///
  /// \code
  /// #include <aims/connectivity/structuring_element.h>
  /// #include <aims/vector/vector.h>
  /// #include <cartobase/smart/rcptr.h>
  /// #include <iostream>
  /// using namespace aims;
  /// using namesapce carto;
  /// using namespace std;
  ///
  /// // Create spherical structuring element of radius 5 voxels
  /// rc_ptr<StructuringElement> se( new strel::Sphere(5) );
  /// StructuringElement::iterator ic, ie = se->end();
  ///
  /// for( ic = se->begin(); ic != ie; ++ic ) {
  ///     cout << "Structuring element point: " << *ic << endl;
  /// \endcode
  ///
  /// \sa aims::strel::Shape aims::strel::ShapeFactory,
  ///     aims::strel::Connectivity aims::strel::ConnectivityFactory
  class StructuringElement {
    public:
      //----------------------------------------------------------------------
      // TYPEDEF
      //----------------------------------------------------------------------
      /// \typedef iterator over Point3d of the aims::StructuringElement.
      typedef std::vector<Point3d>::iterator iterator;
      /// \typedef const_iterator over Point3d of the
      ///          const aims::StructuringElement.
      typedef std::vector<Point3d>::const_iterator const_iterator;

      //----------------------------------------------------------------------
      // CONSTRUCTOR
      //----------------------------------------------------------------------
      StructuringElement(): _vector() {}
      StructuringElement( const std::vector<Point3d> & vector ):
        _vector(vector) {}
      virtual ~StructuringElement() {}
      // Use default compiler-supplied copy constructor and assignment operator
      // StructuringElement( const StructuringElement & se );
      // StructuringElement & operator= ( const StructuringElement & se );

      //----------------------------------------------------------------------
      // ITERATORS
      //----------------------------------------------------------------------
      /// \brief Return iterator to beginning
      iterator begin() { return _vector.begin(); }
      /// \brief Return iterator to end
      iterator end() { return _vector.end(); }
      /// \brief Return const_iterator to beginning
      /// \return aims::StructuringElement::const_iterator that points to
      ///         the beginning Point3d
      const_iterator begin() const { return _vector.begin(); }
      /// \brief Return const_iterator to end
      /// \return aims::StructuringElement::const_iterator that points to
      ///         the element past the end
      const_iterator end() const { return _vector.end(); }


      //----------------------------------------------------------------------
      // ACCESSOR
      //----------------------------------------------------------------------
      size_t size() const { return _vector.size(); }
      /// \brief Return std::vector<Point3d>
      /// \return std::vector<Point3d> that contains 3d positions of the
      ///         structuring element
      const std::vector<Point3d> & getVector() const { return _vector; }
      /// \brief Maximum distance to \c origin in each direction.
      /// Warning: it is computed at each call.
      /// \return std::vector<uint16_t> that contains the borders availables
      ///         for the volume.
      ///         vector[0]: amplitude in low x direction
      ///         vector[1]: amplitude in high x direction
      ///         vector[2]: amplitude in low y direction
      ///         vector[3]: amplitude in high y direction
      ///         vector[4]: amplitude in low z direction
      ///         vector[5]: amplitude in high z direction
      std::vector<int> getAmplitude( const Point3d & origin = Point3d(0,0,0) ) const;

      /// Clone the StructuringElement
      virtual StructuringElement* clone() const { return new StructuringElement(*this); }
    protected:
      std::vector<Point3d> _vector;
  };

  //==========================================================================
  // STRUCTURED VOLUME
  //==========================================================================
  /// Class giving access to an iterator over the voxels of a volume inside
  /// of a structuring element.
  template <typename T>
  class StructuredVolume
  {
    public:
      typedef typename std::vector<carto::reference_wrapper<T> >::iterator iterator;
      typedef typename std::vector<carto::reference_wrapper<T> >::const_iterator const_iterator;
      StructuredVolume( carto::Volume<T> & vol,
                        const StructuringElement & se ):
        _vector()
      {
        StructuringElement::const_iterator i, e = se.end();
        for( i=se.begin(); i!=e; ++i )
          _vector.push_back( carto::wrap::ref( vol( (*i)[0], (*i)[1], (*i)[2] ) ) );
      }
      ~StructuredVolume() {}
      typename StructuredVolume<T>::iterator begin() { return _vector.begin(); }
      typename StructuredVolume<T>::iterator end() { return _vector.end(); }
      typename StructuredVolume<T>::const_iterator begin() const { return _vector.begin(); }
      typename StructuredVolume<T>::const_iterator end() const { return _vector.end(); }
    protected:
      std::vector<carto::reference_wrapper<T> > _vector;
  };

  /// \see StructuredVolume
  template <typename T>
  class StructuredConstVolume
  {
    public:
      typedef typename std::vector<carto::reference_wrapper<const T> >::iterator iterator;
      typedef typename std::vector<carto::reference_wrapper<const T> >::const_iterator const_iterator;
      StructuredConstVolume( const carto::Volume<T> & vol,
                             const StructuringElement & se ):
        _vector()
      {
        StructuringElement::const_iterator i, e = se.end();
        for( i=se.begin(); i!=e; ++i )
          _vector.push_back( carto::wrap::cref( vol( (*i)[0], (*i)[1], (*i)[2] ) ) );
      }
      ~StructuredConstVolume() {}
      typename StructuredConstVolume<T>::iterator begin() { return _vector.begin(); }
      typename StructuredConstVolume<T>::iterator end() { return _vector.end(); }
      typename StructuredConstVolume<T>::const_iterator begin() const { return _vector.begin(); }
      typename StructuredConstVolume<T>::const_iterator end() const { return _vector.end(); }
    protected:
      std::vector<carto::reference_wrapper<const T> > _vector;
  };

  //==========================================================================
  // STRUCTURING ELEMENT: REFERENCE
  //==========================================================================
  /// Convenient handle for a StructuringElement
  ///
  /// This class is a reference counting pointer to a StructuringElement
  /// (or one of its derived classes).
  /// It also implements all StructuringElement public methods.
  class StructuringElementRef: public carto::rc_ptr<StructuringElement>
  {
    public:
      typedef StructuringElement::iterator        iterator;
      typedef StructuringElement::const_iterator  const_iterator;

      /// Default constructor
      ///
      /// This constructor builds an empty pointer. Such a pointer equals to
      /// strel::none().
      StructuringElementRef():
        carto::rc_ptr<StructuringElement>() {}
      /// New object constructor
      ///
      /// This constructor initiate reference counting. It should always be
      /// passed an unowned pointer (for example, built using new)
      StructuringElementRef( StructuringElement * se ):
        carto::rc_ptr<StructuringElement>(se) {}
      /// Copy constructor
      ///
      /// This constructor takes any reference counted pointer.
      /// It increments the reference counter by one.
      StructuringElementRef( const carto::rc_ptr<StructuringElement> & se ):
        carto::rc_ptr<StructuringElement>(se) {}

      iterator begin() { return (*this)->begin(); }
      iterator end() { return (*this)->end(); }
      const_iterator begin() const { return (*this)->begin(); }
      const_iterator end() const { return (*this)->end(); }

      size_t size() const { return (*this)->size(); }
      const std::vector<Point3d> & getVector() const
      { return (*this)->getVector(); }
      std::vector<int>
      getAmplitude( const Point3d & origin = Point3d(0,0,0) ) const
      { return (*this)->getAmplitude(origin); }
  };

  namespace strel {
    /// Pointer to an empty aims::StructuringElement
    extern StructuringElementRef none();

    //========================================================================
    // STRUCTURING ELEMENT: SHAPE INTERFACE
    //========================================================================
    /// \brief aims::strel::Shape pure abstract class representing a "shape"
    /// 3D structuring element.
    ///
    /// Shapes are defined by an origin and an amplitude.
    /// They can be generated from a keyword using aims::strel::ShapeFactory
    ///
    /// \sa aims::strel::Cube aims::strel::SquareXY
    ///     aims::strel::SquareXZ aims::strel::SquareYZ
    ///     aims::strel::Sphere aims::strel::DiskXY
    ///     aims::strel::DiskXZ aims::strel::DiskYZ
    class Shape: public StructuringElement
    {
      friend class ShapeFactory;
      public:
        typedef StructuringElement::iterator        iterator;
        typedef StructuringElement::const_iterator  const_iterator;
        virtual ~Shape() {};
        virtual Shape* clone() const = 0;
      protected:
        // Defined methods
        void setParameters( const double amplitude = 1.,
                            const bool usecenter = false );
        void setParameters( const Point3d & origin,
                            const double amplitude = 1.,
                            const bool usecenter = false );
        void setParameters( const std::vector<double> & amplitude,
                            const bool usecenter = false );
        // Pure virtual methods:
        virtual void setParameters( const Point3d & origin,
                                    const std::vector<double> & amplitude,
                                    const bool usecenter = false ) = 0;
    };

    //========================================================================
    // STRUCTURING ELEMENT: SHAPE FACTORY
    //========================================================================
    /// \brief aims::strel::Shape generator
    ///
    /// Here are listed the registered keywords as of July 2014:
    ///
    /// \code
    /// registerShape( "cube", Cube() );
    /// registerShape( "squarexy", SquareXY() );
    /// registerShape( "squarexz", SquareXZ() );
    /// registerShape( "squareyz", SquareYZ() );
    /// registerShape( "sphere", Sphere() );
    /// registerShape( "diskxy", DiskXY() );
    /// registerShape( "diskxz", DiskXZ() );
    /// registerShape( "diskyz", DiskYZ() );
    /// registerShape( "cross", Cross() );
    /// registerShape( "crossxy", CrossXY() );
    /// registerShape( "crossxz", CrossXZ() );
    /// registerShape( "crossyz", CrossYZ() );
    /// registerShape( "diagonalcross", DiagonalCross() );
    /// registerShape( "diagonalcrossxy", DiagonalCrossXY() );
    /// registerShape( "diagonalcrossxz", DiagonalCrossXZ() );
    /// registerShape( "diagonalcrossyz", DiagonalCrossYZ() );
    /// \endcode
    class ShapeFactory {
      public:
        static Shape* create( const std::string & type,
                              const double amplitude = 1.,
                              const bool usecenter = false );
        static Shape* create( const std::string & type,
                              const Point3d & origin,
                              const double amplitude = 1.,
                              const bool usecenter = false );
        static Shape* create( const std::string & type,
                              const std::vector<double> & amplitude,
                              const bool usecenter = false );
        static Shape* create( const std::string & type,
                              const Point3d & origin,
                              const std::vector<double> & amplitude,
                              const bool usecenter = false );
        static void registerShape( const std::string & type,
                                   const Shape & strel );
        static std::set<std::string> shapes();
      protected:
        static void init();
        static std::map<std::string,carto::rc_ptr<Shape> > & _map();
    };

    //========================================================================
    // STRUCTURING ELEMENT: CONNECTIVITY INTERFACE
    //========================================================================
    /// \brief aims::strel::Connectivity pure abstract class representing
    /// a "connectivity" 3D structuring element.
    ///
    /// Connectivities are defined by a 3x3x3 boolean matrix.
    /// They can be generated from a keyword using
    /// aims::strel::ConnectivityFactory
    ///
    /// \sa aims::strel::Connectivity4XY
    class Connectivity: public StructuringElement
    {
      friend class ConnectivityFactory;
      public:
        typedef StructuringElement::iterator        iterator;
        typedef StructuringElement::const_iterator  const_iterator;
        virtual ~Connectivity() {};
        virtual Connectivity* clone() const = 0;
        //--------------------------------------------------------------------
        // TYPEDEF
        //--------------------------------------------------------------------
        /// \typedef bool matrix 3x3x3 elements that describe a 3D
        ///          structuring element.
        typedef const bool (&Matrix3x3x3Const)[3][3][3];
        //--------------------------------------------------------------------
        // METHODS
        //--------------------------------------------------------------------
         /// \brief return the Matrix3x3x3Const representing a structuring
         ///        element.
         /// \tparam ConnectivityType Connectivity::Type used to get the
         ///         Matrix3x3x3Const.
         /// \return Matrix3x3x3Const that contains \c bool set to \c true
         ///         for the structuring element.
        virtual Matrix3x3x3Const getMatrix() const = 0;
      protected:
        void setVectorFromMatrix( const Point3d & origin = Point3d(0,0,0),
                                  const bool usecenter = false );
    };

    //========================================================================
    // STRUCTURING ELEMENT: CONNECTIVITY FACTORY
    //========================================================================
    /// \brief aims::strel::Connectivity generator
    ///
    /// Here are listed the registered keywords as of July 2014:
    ///
    /// \code
    /// registerConnectivity( "4xy", Connectivity4XY() );
    /// registerConnectivity( "4xz", Connectivity4XZ() );
    /// registerConnectivity( "4yz", Connectivity4YZ() );
    /// registerConnectivity( "4xydiag", Connectivity4XYDiag() );
    /// registerConnectivity( "4xzdiag", Connectivity4XZDiag() );
    /// registerConnectivity( "4yzdiag", Connectivity4YZDiag() );
    /// registerConnectivity( "6", Connectivity6XYZ() );
    /// registerConnectivity( "8xy", Connectivity8XY() );
    /// registerConnectivity( "8xz", Connectivity8XZ() );
    /// registerConnectivity( "8yz", Connectivity8YZ() );
    /// registerConnectivity( "8xyz", Connectivity8XYZ() );
    /// registerConnectivity( "18", Connectivity18XYZ() );
    /// registerConnectivity( "26", Connectivity26XYZ() );
    /// registerConnectivity( "9xyz-", Connectivity9XYZMinus() );
    /// registerConnectivity( "9xyz+", Connectivity9XYZPlus() );
    /// registerConnectivity( "9xzy-", Connectivity9XZYMinus() );
    /// registerConnectivity( "9xzy+", Connectivity9XZYPlus() );
    /// registerConnectivity( "9yzx-", Connectivity9YZXMinus() );
    /// registerConnectivity( "9yzx+", Connectivity9YZXPlus() );
    /// registerConnectivity( "5x-y", Connectivity5XMinusY() );
    /// registerConnectivity( "5x+y", Connectivity5XPlusY() );
    /// registerConnectivity( "5xy-", Connectivity5XYMinus() );
    /// registerConnectivity( "5xy+", Connectivity5XYPlus() );
    /// registerConnectivity( "5x-z", Connectivity5XMinusZ() );
    /// registerConnectivity( "5x+z", Connectivity5XPlusZ() );
    /// registerConnectivity( "5xz-", Connectivity5XZMinus() );
    /// registerConnectivity( "5xz+", Connectivity5XZPlus() );
    /// registerConnectivity( "5y-z", Connectivity5YMinusZ() );
    /// registerConnectivity( "5y+z", Connectivity5YPlusZ() );
    /// registerConnectivity( "5yz-", Connectivity5YZMinus() );
    /// registerConnectivity( "5yz+", Connectivity5YZPlus() );
    /// \endcode
    class ConnectivityFactory
    {
      public:
        static Connectivity* create( const std::string & type,
                                     const bool usecenter = false );
        static Connectivity* create( const std::string & type,
                                     const Point3d & origin,
                                     const bool usecenter = false );
        static void registerConnectivity( const std::string & type,
                                          const Connectivity & strel );
        static std::set<std::string> connectivities();
      protected:
        static void init();
        static std::map<std::string,carto::rc_ptr<Connectivity> > & _map();
    };

  } // namespace strel
} // namespace aims


//============================================================================
// STRUCTURING ELEMENT: SHAPE DERIVED CLASSES
//============================================================================
#define AIMS_DECLARE_STREL_SHAPE( NAME )                                     \
  class NAME: public Shape                                                   \
  {                                                                          \
    public:                                                                  \
      typedef StructuringElement::iterator        iterator;                  \
      typedef StructuringElement::const_iterator  const_iterator;            \
      NAME( const double amplitude = 1.,                                     \
            const bool usecenter = false ): Shape()                          \
          { Shape::setParameters( amplitude, usecenter ); }                  \
      NAME( const Point3d & origin,                                          \
            const double amplitude = 1.,                                     \
            const bool usecenter = false ): Shape()                          \
          { Shape::setParameters( origin, amplitude, usecenter ); }          \
      NAME( const std::vector<double> & amplitude,                           \
            const bool usecenter = false ): Shape()                          \
          { Shape::setParameters( amplitude, usecenter ); }                  \
      NAME( const Point3d & origin,                                          \
            const std::vector<double> & amplitude,                           \
            const bool usecenter = false ): Shape()                          \
          { setParameters( origin, amplitude, usecenter ); }                 \
      virtual ~NAME() {}                                                     \
    protected:                                                               \
      virtual void setParameters( const Point3d & origin,                    \
                                  const std::vector<double> & amplitude,     \
                                  const bool usecenter = false );            \
      virtual NAME* clone() const { return new NAME(*this); }                \
  }

namespace aims {
  namespace strel {
    AIMS_DECLARE_STREL_SHAPE( Cube );
    AIMS_DECLARE_STREL_SHAPE( SquareXY );
    AIMS_DECLARE_STREL_SHAPE( SquareXZ );
    AIMS_DECLARE_STREL_SHAPE( SquareYZ );
    AIMS_DECLARE_STREL_SHAPE( Sphere );
    AIMS_DECLARE_STREL_SHAPE( DiskXY );
    AIMS_DECLARE_STREL_SHAPE( DiskXZ );
    AIMS_DECLARE_STREL_SHAPE( DiskYZ );
    AIMS_DECLARE_STREL_SHAPE( Cross );
    AIMS_DECLARE_STREL_SHAPE( CrossXY );
    AIMS_DECLARE_STREL_SHAPE( CrossXZ );
    AIMS_DECLARE_STREL_SHAPE( CrossYZ );
    AIMS_DECLARE_STREL_SHAPE( DiagonalCross );
    AIMS_DECLARE_STREL_SHAPE( DiagonalCrossXY );
    AIMS_DECLARE_STREL_SHAPE( DiagonalCrossXZ );
    AIMS_DECLARE_STREL_SHAPE( DiagonalCrossYZ );
  } // namespace strel
} // namespace aims

//============================================================================
// STRUCTURING ELEMENT: CONNECTIVITY DERIVED CLASSES
//============================================================================
#define AIMS_DECLARE_STREL_CONNECTIVITY( NAME )                              \
  class NAME: public Connectivity                                            \
  {                                                                          \
    public:                                                                  \
      typedef StructuringElement::iterator        iterator;                  \
      typedef StructuringElement::const_iterator  const_iterator;            \
      NAME(): Connectivity() { setVectorFromMatrix(); }                      \
      virtual ~NAME() {}                                                     \
      Connectivity::Matrix3x3x3Const getMatrix() const;                      \
    protected:                                                               \
      virtual NAME* clone() const                                            \
        { return new NAME(*this); }                                          \
  }

namespace aims {
  namespace strel {
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4XY );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4XZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4YZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4XYDiag );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4XZDiag );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity4YZDiag );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity6XYZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity8XY );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity8XZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity8YZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity8XYZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity18XYZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity26XYZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9XYZMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9XYZPlus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9XZYMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9XZYPlus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9YZXMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity9YZXPlus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XMinusY );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XPlusY );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XYMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XYPlus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XMinusZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XPlusZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XZMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5XZPlus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5YMinusZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5YPlusZ );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5YZMinus );
    AIMS_DECLARE_STREL_CONNECTIVITY( Connectivity5YZPlus );
  } // namespace strel
} // namespace aims

#endif
