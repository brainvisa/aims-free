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
 *  StructuringElement class
 */

#ifndef AIMS_CONNECTIVITY_STRUCTURING_ELEMENT_H
#define AIMS_CONNECTIVITY_STRUCTURING_ELEMENT_H

#include <vector>
#include <aims/vector/vector.h>
#include <aims/connectivity/connectivity.h>

namespace aims{
  
  struct Shape {
    /**
    *  \c enum that defines basic 3D shapes type.
    */
    enum Type {
      CUBE,
      SPHERE,
      SQUARE_XY,
      SQUARE_XZ,
      SQUARE_YZ,
      DISK_XY,
      DISK_XZ,
      DISK_YZ,
    };
  };

  
  /**
   *  \brief aims::StructuringElement class representing a 3D structuring element.
   *
   *  The aims::StructuringElement class is used 
   *  to get neighbors of a coordinate in a 3D grid.
   *  It could be initialized using either an aims::Connectivity::Type
   *  or an aims::Shape::Type.
   *
   * \code
   * #include <aims/connectivity/structuring_element.h>
   * #include <aims/vector/vector.h>
   * #include <vector>
   * #include <iostream>
   *
   * // Create spherical structuring element of radius 5
   * StructuringElement se = StructuringElement::getStructuringElement( "sphere", 
   *                                                                    5 );
   * StructuringElement::iterator ic, ie = se.end();
   *
   * for (ic = se.begin(); ic != ie; ++ic){
   *     std::cout << "Structuring element point :" << *ic << std::endl;
   *
   * \endcode
   *
   * \sa aims::Shape, aims::Connectivity
   */
  class StructuringElement {
    public:
      /** 
       * \typedef bool matrix 3x3x3 elements that describe a 3D structuring element.
       */
      typedef const bool (&Matrix3x3x3Const)[3][3][3];
      /** 
       * \typedef iterator over Point3d of the aims::StructuringElement.
       */
      typedef std::vector<Point3d>::iterator iterator;
      /** 
       * \typedef const_iterator over Point3d of the const aims::StructuringElement.
       */
      typedef std::vector<Point3d>::const_iterator const_iterator;
      
      /// Constructors
      
      /** 
       * \brief Constructor based on Connectivity::Type
       * \param connectivity Connectivity::Type used to initialize
       *                     the aims::StructuringElement.
       * \param origin Point3d added to each 3D position of the
       *               aims::StructuringElement.
       * \param amplitude double used to scale the aims::StructuringElement.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must
       *                  be added to the aims::StructuringElement.
       *
       * \sa aims::Connectivity, Point3d
       */
      StructuringElement( const Connectivity::Type connectivity,
                          const Point3d origin,
                          const double amplitude = 1.,
                          const bool usecenter = false );
       /** 
       * \see StructuringElement(const Connectivity::Type, const Point3d, const double, const bool)
       */
      StructuringElement( const Connectivity::Type connectivity,
                          const double amplitude = 1.,
                          const bool usecenter = false );
                          
      /** 
       * \brief Constructor based on Shape::Type
       * \param shape Shape::Type used to initialize
       *              the aims::StructuringElement.
       * \param origin Point3d added to each 3D position of the
       *               aims::StructuringElement.
       * \param amplitude double used to scale the aims::StructuringElement.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must
       *                  be added to the aims::StructuringElement.
       *
       * \sa aims::Shape, Point3d
       */
      StructuringElement( const Shape::Type shape,
                          const Point3d origin,
                          const double amplitude = 1.,
                          const bool usecenter = false );
       /** 
       * \see StructuringElement(const Shape::Type, const Point3d, const double, const bool)
       */
      StructuringElement( const Shape::Type shape,
                          const double amplitude = 1.,
                          const bool usecenter = false );
      
      /// Members
      /** 
       * \brief Return iterator to beginning
       */
      StructuringElement::iterator begin() { return _vector.begin(); }
      /** 
       * \brief Return iterator to end
       */
      StructuringElement::iterator end() { return _vector.end(); }
      
      /** 
       * \brief Return const_iterator to beginning
       * \return aims::StructuringElement::const_iterator that points to the beginning Point3d
       */
      StructuringElement::const_iterator begin() const { return _vector.begin(); }
      /** 
       * \brief Return const_iterator to end
       * \return aims::StructuringElement::const_iterator that points to the element past the end
       */
      StructuringElement::const_iterator end() const { return _vector.end(); }
      
      /** 
       * \brief Return std::vector<Point3d>
       * \return std::vector<Point3d> that contains 3d positions of the structuring element
       */
      std::vector<Point3d> get_vector() const { return _vector; }
      /** 
       * \brief Return Connectivity::Type use.
       * \return bool that specifies wether the structuring element was initialized using Connectivity::Type.
       */
      bool use_connectivity() const { return _useconnectivity; }
      /** 
       * \brief Return Shape::Type use.
       * \return bool that specifies wether the structuring element was initialized using Shape::Type.
       */
      bool use_shape() const { return !_useconnectivity; }
      
      /// Static members
      /** 
       * \brief Return aims::StructuringElement for a type.
       *
       * \param type std::string that represents the type used to initialize
       *             the aims::StructuringElement. It can be either :
       *             - one of the following connectivity type :
       *                4xy, 4xz, 4yz, 6, 8xy, 8xz, 8yz, 18, 26,
       *                9xyz-, 9xyz+, 9xzy-, 9xzy+, 9yzx-, 9yzx+,
       *                5x-y, 5x+y, 5xy-, 5xy+, 5x-z, 5x+z, 5xz-,
       *                5xz+, 5y-z, 5y+z, 5yz-, 5yz+
       *             - one of the following shape type :
       *                sphere, cube,
       *                diskxy, diskxz, diskyz
       *                squarexy, squarexz, squareyz
       * \param origin Point3d added to each 3D position of the
       *               aims::StructuringElement.
       * \param amplitude double used to scale the aims::StructuringElement.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must
       *                  be added to the aims::StructuringElement.
       * \return aims::StructuringElement for the specified type and parameters.
       */
      static StructuringElement getStructuringElement( 
                                                const std::string type,
                                                const Point3d origin = Point3d(0, 0, 0),
                                                const double amplitude = 1.,
                                                const bool usecenter = false );
                                                
      /**
       * \see getStructuringElement(const std::string, const Point3d, const double, const bool)
       */
      static StructuringElement getStructuringElement( 
                                                const std::string type,
                                                const double amplitude = 1.,
                                                const bool usecenter = false );
      /**
       * \brief return std::vector<Point3d> representing a structuring element.
       * 
       * \param connectivity Connectivity::Type used to build
       *                     the std::vector<Point3d>.
       * \param origin Point3d added to each 3D position of the structuring element.
       * \param amplitude double used to scale the structuring element.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must 
       *                  be added to the structuring element.
       * \return std::vector<Point3d> that contains 3D positions for the structuring
       *         element.
       */
      static std::vector<Point3d> get_vector( const Connectivity::Type connectivity,
                                              const Point3d origin = Point3d(0, 0, 0),
                                              const double amplitude = 1.,
                                              const bool usecenter = false );
      /**
       * \brief return std::vector<Point3d> representing a structuring element.
       * 
       * \param shape Shape::Type used to build
       *              the std::vector<Point3d>.
       * \param origin Point3d added to each 3D position of the structuring element.
       * \param amplitude double used to scale the structuring element.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must 
       *                  be added to the structuring element.
       * \return std::vector<Point3d> that contains 3D positions for the structuring
       *         element.
       */
      static std::vector<Point3d> get_vector( const Shape::Type shape,
                                              const Point3d origin = Point3d(0, 0, 0),
                                              const double amplitude = 1.,
                                              const bool usecenter = false );
      /**
       * \brief return std::vector<Point3d> representing a structuring element.
       * 
       * \param matrix StructuringElement::Matrix3x3x3Const used to build
       *               the std::vector<Point3d>.
       * \param origin Point3d added to each 3D position of the structuring element.
       * \param amplitude double used to scale the structuring element.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must 
       *                  be added to the structuring element.
       * \return std::vector<Point3d> that contains 3D positions for the structuring
       *         element.
       */
      static std::vector<Point3d> get_vector( StructuringElement::Matrix3x3x3Const matrix,
                                              const Point3d origin = Point3d(0, 0, 0),
                                              const double amplitude = 1.,
                                              const bool usecenter = false );
      /**
       * \brief return std::vector<Point3d> representing a structuring element.
       * \tparam ShapeType Shape::Type used to build the std::vector<Point3d>.
       * \param origin Point3d added to each 3D position of the structuring element.
       * \param amplitude double used to scale the structuring element.
       *                  Amplitude is a number of voxels.
       * \param usecenter bool used to specify that the central voxel must 
       *                  be added to the structuring element.
       * \return std::vector<Point3d> that contains 3D positions for the structuring
       *         element.
       */
      template <Shape::Type ShapeType>
      static std::vector<Point3d> get_vector( const Point3d origin = Point3d(0, 0, 0),
                                              const double amplitude = 1.,
                                              const bool usecenter = false );
      
      /**
       * \brief return the Matrix3x3x3Const representing a structuring element.
       * \tparam ConnectivityType Connectivity::Type used to get the Matrix3x3x3Const.
       * \return Matrix3x3x3Const that contains \c bool set to \c true for the structuring
       *         element.
       */
      template <Connectivity::Type ConnectivityType>
      static StructuringElement::Matrix3x3x3Const get_matrix();
      
      /**
       * \brief return the Matrix3x3x3Const representing a structuring element.
       * \param ConnectivityType Connectivity::Type used to get the Matrix3x3x3Const.
       * \return Matrix3x3x3Const that contains \c bool set to \c true for the structuring
       *         element.
       */      
      static StructuringElement::Matrix3x3x3Const get_matrix(const Connectivity::Type connectivity);
      
    protected :
      union Type {
        Connectivity::Type connectivity;
        Shape::Type shape;
        
        Type(Connectivity::Type type):connectivity(type){}
        Type(Shape::Type type):shape(type){}
      };
      
      std::vector<Point3d> _vector;
      Point3d _origin;
      double _amplitude;
      bool _usecenter;
      bool _useconnectivity;
      Type _type;
      
  };

}

#endif
