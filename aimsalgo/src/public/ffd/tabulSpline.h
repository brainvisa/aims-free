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



#ifndef AIMS_FFD_TABULSPLINE_H
#define AIMS_FFD_TABULSPLINE_H
#include <cstdlib>
#include <aims/vector/vector.h>
#include <aims/data/data_g.h>
#include <aims/math/bspline3.h>
#include <aims/math/bspline2.h>
using namespace aims;




class TabulSpline
{
 public:
               TabulSpline( std::string name, int  order=3, int length=65537);
  virtual     ~TabulSpline() ;

  float        spline3( double ) const ;
  float        spline3derivative( double ) const ;
  float        dump( int i ) const {return _splineCoef[i];}

 private:
  std::string _name;

  int                     _splineTabLength;
  float                   *_splineCoef;
  float                   *_derivatedSplineCoef ;  
};


inline float 
TabulSpline::spline3( double u ) const
{
/*   if ((u>2) || (u < -2)) */
/*     cout << "valeur de u " << u << endl; */
  int index = static_cast<int>( 0.25*fabs(u)*(_splineTabLength-1) ) ;
  
  if( index < 0 || index >= _splineTabLength ){
     //std::cerr << "Spline3 : Index " << index << " corresponding to parameter " << u << " out of tabulspline : " << _splineTabLength << std::endl ;
     return 0. ;
  }
  return _splineCoef[ index ] ;
}


inline float 
TabulSpline::spline3derivative( double u) const
{
  int index = static_cast<int>(0.25*fabs(u)*(_splineTabLength-1 ) ) ;

  if( index < 0 || index >= _splineTabLength ){
     //std::cerr << "Spline3 derivatives : Index " << index << " corresponding to parameter " << u << " out of tabulspline" << _splineTabLength << std::endl ;
     return 0. ;
  }
  
  if (u > 0.0)
    return _derivatedSplineCoef[ index ];
  else
    return(- _derivatedSplineCoef[ index ]);
}
#endif
