

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
