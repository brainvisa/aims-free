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
#include <cartobase/config/verbose.h>
#include <aims/vector/vector.h>
#include <aims/math/bspline3.h>
#include <aims/math/bspline2.h>


class TabulSpline
{
 public:
               TabulSpline( std::string name, int  order = 3, int length = 65537, int factor = 2);
  virtual     ~TabulSpline();
  
  int          index( double u ) const;
  bool         isvalid( int index ) const;
  int          getFactor() const;
  int          getTabLength() const;
  float        spline3( double ) const;
  float        spline3derivative( double ) const;
  float        dump( int i ) const {return _splineCoef[i];}

 private:
  std::string _name;

  int                     _factor;
  int                     _splineTabLength;
  float                   *_splineCoef;
  float                   *_derivatedSplineCoef ;  
};

inline int 
TabulSpline::getTabLength() const
{
  return _splineTabLength;
}

inline int 
TabulSpline::getFactor() const
{
  return _factor;
}

inline int 
TabulSpline::index( double u ) const
{
  return (int)( fabs(u) * (_splineTabLength - 1) / _factor );
}

inline bool
TabulSpline::isvalid( int index ) const
{
  return ( index >= 0 && index < _splineTabLength );
}

inline float 
TabulSpline::spline3( double u ) const
{
  int i = index(u);

  if ( !isvalid( i ) ){
     cartoDbgMsg( 1,  "Spline3 : Index " + carto::toString(i)
                    + " corresponding to parameter " + carto::toString(u)
                    + " out of tabulspline : " + carto::toString(_splineTabLength));
     return 0. ;
  }
  return _splineCoef[ i ] ;
}


inline float 
TabulSpline::spline3derivative( double u ) const
{
  int i = index(u);
  if ( !isvalid( i ) ) {
     cartoDbgMsg( 1, "Spline3 derivatives : Index " + carto::toString(i)
                   + " corresponding to parameter " + carto::toString(u)
                   + " out of tabulspline : " + carto::toString(_splineTabLength));
     return 0. ;
  }
  
  if (u > 0.0)
    return _derivatedSplineCoef[ i ];
  else
    return(- _derivatedSplineCoef[ i ]);
}

#endif
