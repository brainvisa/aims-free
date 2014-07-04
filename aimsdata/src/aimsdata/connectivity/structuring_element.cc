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

#include <map>
#include <cartobase/config/verbose.h>
#include <aims/connectivity/structuring_element.h>
namespace aims{
  
  // Constructors
  
  // Constructor for Connectivity::Type
  StructuringElement::StructuringElement( 
                      const Connectivity::Type connectivity,
                      const Point3d origin,
                      const double amplitude,
                      const bool usecenter )
                      : _type(connectivity),
                        _origin(origin),
                        _amplitude(amplitude),
                        _usecenter(usecenter),
                        _useconnectivity(true) {
    _vector = StructuringElement::get_vector( _type.connectivity,
                                              _origin,
                                              _amplitude,
                                              _usecenter );
  }
  
  StructuringElement::StructuringElement( 
                      const Connectivity::Type connectivity,
                      const double amplitude,
                      const bool usecenter )
                      : _type(connectivity),
                        _origin(Point3d(0, 0, 0)),
                        _amplitude(amplitude),
                        _usecenter(usecenter),
                        _useconnectivity(true) {
    _vector = StructuringElement::get_vector( _type.connectivity,
                                              _origin,
                                              _amplitude,
                                              _usecenter );
  }
  
  // Constructor for Shape::Type
  StructuringElement::StructuringElement( 
                      const Shape::Type shape,
                      const Point3d origin,
                      const double amplitude,
                      const bool usecenter )
                      : _type(shape),
                        _origin(origin),
                        _amplitude(amplitude),
                        _usecenter(usecenter),
                        _useconnectivity(false) {
    _vector = StructuringElement::get_vector( _type.shape,
                                              _origin,
                                              _amplitude,
                                              _usecenter );
  }
  
  StructuringElement::StructuringElement(
                      const Shape::Type shape,
                      const double amplitude,
                      const bool usecenter )
                      : _type(shape),
                        _origin(Point3d(0, 0, 0)),
                        _amplitude(amplitude),
                        _usecenter(usecenter),
                        _useconnectivity(false) {
    _vector = StructuringElement::get_vector( _type.shape,
                                              _origin,
                                              _amplitude,
                                              _usecenter );
  }
  
  // Static members
  std::vector<Point3d> StructuringElement::get_vector( StructuringElement::Matrix3x3x3Const matrix,
                                                      const Point3d origin,
                                                      const double amplitude,
                                                      const bool usecenter ) {
    std::vector<Point3d> se;
    for (int z = 0; z < 3; ++z)
      for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
          if ( (x == 1) && (y == 1) && (z == 1) ) {
            if ( (matrix[z][y][x]) || usecenter )
              se.push_back(origin);
          }
          else if (matrix[z][y][x]) {
            for (int a = 1; a <= amplitude; ++a)
              se.push_back(Point3d(a * (x - 1), a * (y - 1), a * (z - 1)) + origin);
          }
            
    return se;
  }

  std::vector<Point3d> StructuringElement::get_vector( const Connectivity::Type connectivity,
                                                       const Point3d origin,
                                                       const double amplitude,
                                                       const bool usecenter ) {

    return StructuringElement::get_vector( 
            get_matrix(connectivity), 
            origin, 
            amplitude, 
            usecenter );
  }
  
  /// StructuringElement shapes
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::CUBE>( const Point3d origin,
                                                                    const double amplitude,
                                                                    const bool usecenter ){
    std::vector<Point3d> se;
    
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
        for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
          if ( ( i != 0) || (j != 0 ) || (k != 0 ) || usecenter )
            se.push_back(Point3d(i, j, k) + origin);
          
    return se;
  }
  
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::SQUARE_XY>( const Point3d origin,
                                                                         const double amplitude,
                                                                         const bool usecenter ){
    std::vector<Point3d> se;
    
    for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
      for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
        if ( ( i != 0) || (j != 0 ) || usecenter )
          se.push_back(Point3d(i, j, 0) + origin);
          
    return se;
  }
  
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::SQUARE_XZ>( const Point3d origin,
                                                                         const double amplitude,
                                                                         const bool usecenter ){
    std::vector<Point3d> se;
    
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
        if ( ( i != 0) || (k != 0 ) || usecenter )
          se.push_back(Point3d(i, 0, k) + origin);
          
    return se;
  }
  
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::SQUARE_YZ>( const Point3d origin,
                                                                         const double amplitude,
                                                                         const bool usecenter ){
    std::vector<Point3d> se;
    
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
        if ( ( j != 0) || (k != 0 ) || usecenter )
          se.push_back(Point3d(0, j, k) + origin);
          
    return se;
  }
  
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::SPHERE>( const Point3d origin,
                                                                      const double amplitude,
                                                                      const bool usecenter ){
    std::vector<Point3d> se;
    
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
        for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
          if ( ( ( i != 0) || (j != 0 ) || (k != 0 ) || usecenter )
              && (sqrt(pow((double)i, 2) + pow((double)j, 2) + pow((double)k, 2)) <= amplitude ) )
            se.push_back(Point3d(i, j, k) + origin);
          
    return se;
  }
  
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::DISK_XY>( const Point3d origin,
                                                                       const double amplitude,
                                                                       const bool usecenter ){
    std::vector<Point3d> se;
    for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
      for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
        if ( ( ( i != 0) || (j != 0 ) || usecenter )
              && (sqrt(pow((double)i, 2) + pow((double)j, 2)) <= amplitude ) )
            se.push_back(Point3d(i, j, 0) + origin );
    
    return se;
  }
    
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::DISK_XZ>( const Point3d origin,
                                                                       const double amplitude,
                                                                       const bool usecenter ){
    std::vector<Point3d> se;
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int i = - (int)amplitude; i <= (int)amplitude; ++i)
        if ( ( ( i != 0) || (k != 0 ) || usecenter )
              && (sqrt(pow((double)i, 2) + pow((double)k, 2)) <= amplitude ) )
            se.push_back(Point3d(i, 0, k) + origin );
    
    return se;
  }
      
  template <>
  std::vector<Point3d> StructuringElement::get_vector<Shape::DISK_YZ>( const Point3d origin,
                                                                       const double amplitude,
                                                                       const bool usecenter ){
    std::vector<Point3d> se;
    for (int k = - (int)amplitude; k <= (int)amplitude; ++k)
      for (int j = - (int)amplitude; j <= (int)amplitude; ++j)
        if ( ( ( j != 0) || (k != 0 ) || usecenter )
              && (sqrt(pow((double)j, 2) + pow((double)k, 2)) <= amplitude ) )
            se.push_back(Point3d(0, j, k) + origin );
    
    return se;
  }
  
  /// StructuringElement connectivity matrices
  
  // 4-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XY>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 1, 0, 1 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_YZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XYdiag>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 0, 1 },
                                       { 0, 0, 0 },
                                       { 1, 0, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XZdiag>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_YZdiag>(){
    static const bool m[3][3][3] = { { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } } };
    return m;
  }
  
  // 6-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_6_XYZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 1, 0, 1 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  // 8-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XY>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 1 },
                                       { 1, 0, 1 },
                                       { 1, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 1, 1, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 1, 1 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_YZ>(){
    static const bool m[3][3][3] = { { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XYZ>(){
    static const bool m[3][3][3] = { { { 1, 0, 1 },
                                       { 0, 0, 0 },
                                       { 1, 0, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 0, 1 },
                                       { 0, 0, 0 },
                                       { 1, 0, 1 } } };
    return m;
  }

  // 18-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_18_XYZ>(){
    static const bool m[3][3][3] = { { { 0, 1, 0 },
                                       { 1, 1, 1 },
                                       { 0, 1, 0 } },
                                     { { 1, 1, 1 },
                                       { 1, 0, 1 },
                                       { 1, 1, 1 } },
                                     { { 0, 1, 0 },
                                       { 1, 1, 1 },
                                       { 0, 1, 0 } } };
    return m;
  }

  // 26-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_26_XYZ>(){
    static const bool m[3][3][3] = { { { 1, 1, 1 },
                                       { 1, 1, 1 },
                                       { 1, 1, 1 } },
                                     { { 1, 1, 1 },
                                       { 1, 0, 1 },
                                       { 1, 1, 1 } },
                                     { { 1, 1, 1 },
                                       { 1, 1, 1 },
                                       { 1, 1, 1 } } };
    return m;
  }

  // 9-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XY_Zminus>(){
    static const bool m[3][3][3] = { { { 1, 1, 1 },
                                       { 1, 1, 1 },
                                       { 1, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XY_Zplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 1 },
                                       { 1, 1, 1 },
                                       { 1, 1, 1 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XZ_Yminus>(){
    static const bool m[3][3][3] = { { { 1, 1, 1 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 1 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 1 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XZ_Yplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 1, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 1, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 1, 1, 1 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_YZ_Xminus>(){
    static const bool m[3][3][3] = { { { 1, 0, 0 },
                                       { 1, 0, 0 },
                                       { 1, 0, 0 } },
                                     { { 1, 0, 0 },
                                       { 1, 0, 0 },
                                       { 1, 0, 0 } },
                                     { { 1, 0, 0 },
                                       { 1, 0, 0 },
                                       { 1, 0, 0 } } };
    return m;
  }

  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_YZ_Xplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 1 },
                                       { 0, 0, 1 },
                                       { 0, 0, 1 } },
                                     { { 0, 0, 1 },
                                       { 0, 0, 1 },
                                       { 0, 0, 1 } },
                                     { { 0, 0, 1 },
                                       { 0, 0, 1 },
                                       { 0, 0, 1 } } };
    return m;
  }
  
  // 5-connex matrices
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XminusY>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 0 },
                                       { 1, 0, 0 },
                                       { 1, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XplusY>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 1 },
                                       { 0, 0, 1 },
                                       { 0, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XYminus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 1, 1, 1 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XYplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 1, 1, 1 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XminusZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 1, 1, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 1, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XplusZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 1, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 1, 1 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XZminus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 1, 1, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XZplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 0, 1 },
                                       { 0, 0, 0 } },
                                     { { 0, 0, 0 },
                                       { 1, 1, 1 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YminusZ>(){
    static const bool m[3][3][3] = { { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YplusZ>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YZminus>(){
    static const bool m[3][3][3] = { { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } } };
    return m;
  }
  
  template <>
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YZplus>(){
    static const bool m[3][3][3] = { { { 0, 0, 0 },
                                       { 0, 0, 0 },
                                       { 0, 0, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 0, 0 },
                                       { 0, 1, 0 } },
                                     { { 0, 1, 0 },
                                       { 0, 1, 0 },
                                       { 0, 1, 0 } } };
    return m;
  }
  
  // Matrix switch method
  StructuringElement::Matrix3x3x3Const StructuringElement::get_matrix(Connectivity::Type connectivity)
  {
    switch(connectivity) {
      case Connectivity::CONNECTIVITY_4_XY :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XY>();
        break;
        
      case Connectivity::CONNECTIVITY_4_XZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XZ>();
        break;
        
      case Connectivity::CONNECTIVITY_4_YZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_YZ>();
        break;
        
      case Connectivity::CONNECTIVITY_4_XYdiag :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XYdiag>();
        break;
        
      case Connectivity::CONNECTIVITY_4_XZdiag :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_XZdiag>();
        break;
        
      case Connectivity::CONNECTIVITY_4_YZdiag :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_4_YZdiag>();
        break;

      case Connectivity::CONNECTIVITY_6_XYZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_6_XYZ>();
        break;

      case Connectivity::CONNECTIVITY_8_XY :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XY>();
        break;

      case Connectivity::CONNECTIVITY_8_XZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XZ>();
        break;

      case Connectivity::CONNECTIVITY_8_YZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_YZ>();
        break;

      case Connectivity::CONNECTIVITY_8_XYZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_8_XYZ>();
        break;
        
      case Connectivity::CONNECTIVITY_18_XYZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_18_XYZ>();
        break;

      case Connectivity::CONNECTIVITY_26_XYZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_26_XYZ>();
        break;

      case Connectivity::CONNECTIVITY_9_XY_Zminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XY_Zminus>();
        break;

      case Connectivity::CONNECTIVITY_9_XY_Zplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XY_Zplus>();
        break;
        
      case Connectivity::CONNECTIVITY_9_XZ_Yminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XZ_Yminus>();
        break;
        
      case Connectivity::CONNECTIVITY_9_XZ_Yplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_XZ_Yplus>();
        break;
        
      case Connectivity::CONNECTIVITY_9_YZ_Xminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_YZ_Xminus>();
        break;
        
      case Connectivity::CONNECTIVITY_9_YZ_Xplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_9_YZ_Xplus>();
        break;

      case Connectivity::CONNECTIVITY_5_XminusY :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XminusY>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XplusY :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XplusY>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XYminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XYminus>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XYplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XYplus>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XminusZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XminusZ>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XplusZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XplusZ>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XZminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XZminus>();
        break;
        
      case Connectivity::CONNECTIVITY_5_XZplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_XZplus>();
        break;

      case Connectivity::CONNECTIVITY_5_YminusZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YminusZ>();
        break;
        
      case Connectivity::CONNECTIVITY_5_YplusZ :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YplusZ>();
        break;
        
      case Connectivity::CONNECTIVITY_5_YZminus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YZminus>();
        break;
        
      case Connectivity::CONNECTIVITY_5_YZplus :
        return StructuringElement::get_matrix<Connectivity::CONNECTIVITY_5_YZplus>();
        break;
        
      default:
        throw std::runtime_error( 
              "connectivity matrix does not exists for the connectivity type " 
              + carto::toString((int)connectivity) );
        break;
    }
  }
  
  
  std::vector<Point3d> StructuringElement::get_vector( const Shape::Type shape,
                                                       const Point3d origin,
                                                       const double amplitude,
                                                       const bool usecenter ) {

    switch(shape) {
      case Shape::CUBE:
        return StructuringElement::get_vector<Shape::CUBE>( origin, amplitude, usecenter);
        break;
        
      case Shape::SQUARE_XY:
        return StructuringElement::get_vector<Shape::SQUARE_XY>( origin, amplitude, usecenter);
        break;
        
      case Shape::SQUARE_XZ:
        return StructuringElement::get_vector<Shape::SQUARE_XZ>( origin, amplitude, usecenter);
        break;
        
      case Shape::SQUARE_YZ:
        return StructuringElement::get_vector<Shape::SQUARE_YZ>( origin, amplitude, usecenter);
        break;
        
      case Shape::SPHERE:
        return StructuringElement::get_vector<Shape::SPHERE>( origin, amplitude, usecenter);
        break;
        
      case Shape::DISK_XY:
        return StructuringElement::get_vector<Shape::DISK_XY>( origin, amplitude, usecenter);
        break;

      case Shape::DISK_XZ:
        return StructuringElement::get_vector<Shape::DISK_XZ>( origin, amplitude, usecenter);
        break;
        
      case Shape::DISK_YZ:
        return StructuringElement::get_vector<Shape::DISK_YZ>( origin, amplitude, usecenter);
        break;
        
      default:
        throw std::runtime_error( 
              "shape does not exists for the shape type " 
              + carto::toString((int)shape) );
        break;
    }
  }

  aims::StructuringElement StructuringElement::getStructuringElement( 
                                                            const std::string type,
                                                            const Point3d origin,
                                                            const double amplitude,
                                                            const bool usecenter ) {
    // Structuring element types map
    std::map<std::string, Connectivity::Type>  connectivities;
    connectivities[ "4xy" ] = Connectivity::CONNECTIVITY_4_XY;
    connectivities[ "4xz" ] = Connectivity::CONNECTIVITY_4_XZ;
    connectivities[ "4yz" ] = Connectivity::CONNECTIVITY_4_YZ;
    connectivities[ "4xydiag" ] = Connectivity::CONNECTIVITY_4_XYdiag;
    connectivities[ "4xzdiag" ] = Connectivity::CONNECTIVITY_4_XZdiag;
    connectivities[ "4yzdiag" ] = Connectivity::CONNECTIVITY_4_YZdiag;
    connectivities[ "6" ] = Connectivity::CONNECTIVITY_6_XYZ;
    connectivities[ "8xy" ] = Connectivity::CONNECTIVITY_8_XY;
    connectivities[ "8xz" ] = Connectivity::CONNECTIVITY_8_XZ;
    connectivities[ "8yz" ] = Connectivity::CONNECTIVITY_8_YZ;
    connectivities[ "8xyz" ] = Connectivity::CONNECTIVITY_8_XYZ;
    connectivities[ "18" ] = Connectivity::CONNECTIVITY_18_XYZ;
    connectivities[ "26" ] = Connectivity::CONNECTIVITY_26_XYZ;
    connectivities[ "9xyz-" ] = Connectivity::CONNECTIVITY_9_XY_Zminus;
    connectivities[ "9xyz+" ] = Connectivity::CONNECTIVITY_9_XY_Zplus;
    connectivities[ "9xzy-" ] = Connectivity::CONNECTIVITY_9_XZ_Yminus;
    connectivities[ "9xzy+" ] = Connectivity::CONNECTIVITY_9_XZ_Yplus;
    connectivities[ "9yzx-" ] = Connectivity::CONNECTIVITY_9_YZ_Xminus;
    connectivities[ "9yzx+" ] = Connectivity::CONNECTIVITY_9_YZ_Xplus;
    connectivities[ "5x-y" ] = Connectivity::CONNECTIVITY_5_XminusY;
    connectivities[ "5x+y" ] = Connectivity::CONNECTIVITY_5_XplusY;
    connectivities[ "5xy-" ] = Connectivity::CONNECTIVITY_5_XYminus;
    connectivities[ "5xy+" ] = Connectivity::CONNECTIVITY_5_XYplus;
    connectivities[ "5x-z" ] = Connectivity::CONNECTIVITY_5_XminusZ;
    connectivities[ "5x+z" ] = Connectivity::CONNECTIVITY_5_XplusZ;
    connectivities[ "5xz-" ] = Connectivity::CONNECTIVITY_5_XZminus;
    connectivities[ "5xz+" ] = Connectivity::CONNECTIVITY_5_XZplus;
    connectivities[ "5y-z" ] = Connectivity::CONNECTIVITY_5_YminusZ;
    connectivities[ "5y+z" ] = Connectivity::CONNECTIVITY_5_YplusZ;
    connectivities[ "5yz-" ] = Connectivity::CONNECTIVITY_5_YZminus;
    connectivities[ "5yz+" ] = Connectivity::CONNECTIVITY_5_YZplus;

    std::map<std::string, Connectivity::Type>::iterator cit = connectivities.find( type );

    std::map<std::string, Shape::Type> shapes;
    shapes[ "sphere" ] = Shape::SPHERE;
    shapes[ "cube" ] = Shape::CUBE;
    shapes[ "diskxy" ] = Shape::DISK_XY;
    shapes[ "diskxz" ] = Shape::DISK_XZ;
    shapes[ "diskyz" ] = Shape::DISK_YZ;
    shapes[ "squarexy" ] = Shape::SQUARE_XY;
    shapes[ "squarexz" ] = Shape::SQUARE_XZ;
    shapes[ "squareyz" ] = Shape::SQUARE_YZ;

    std::map<std::string, Shape::Type>::iterator sit = shapes.find( type );

    if ( cit != connectivities.end() ) {
      if (carto::verbose)
        std::cout << "Found structuring element using connectivity" << std::endl;
      aims::StructuringElement se( cit->second, 
                                   origin,
                                   amplitude,
                                   usecenter );
      return se;
    }
    else if ( sit != shapes.end() ) {
      if (carto::verbose)
        std::cout << "Found structuring element using shape" << std::endl;
      sit = shapes.find( type );
      
      aims::StructuringElement se( sit->second, 
                                   origin,
                                   amplitude,
                                   usecenter );
      return se;
    }
    else
      throw std::invalid_argument( "Invalid structuring element type: " + type );

  }
  
  aims::StructuringElement StructuringElement::getStructuringElement( 
                                                            const std::string type,
                                                            const double amplitude,
                                                            const bool usecenter ) {
    return StructuringElement::getStructuringElement( type, amplitude, usecenter );
  }
}
