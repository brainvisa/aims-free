
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/ffd/tabulSpline.h>
#include <cstdlib>
#include <iostream>

using namespace std;


TabulSpline::TabulSpline( string name, int  order, int length, int factor) :
  _name(name), _factor(factor), _splineTabLength(length + 1)
{
  if (order != 3)
    {
      cerr << "Only cubic spline are tabulated. Stop." << endl;
      exit(1);
    }
    
  float parameter;
  _splineCoef          = new float[ _splineTabLength ] ;
  _derivatedSplineCoef = new float[ _splineTabLength ] ;
  
  for(int i=0; i < _splineTabLength; i++)
    {
      parameter = (float) i * _factor / (_splineTabLength - 1) ;
      _splineCoef[i] = betaSpline3( parameter );
      _derivatedSplineCoef[i] = betaSpline2( parameter + 0.5 ) -
                                betaSpline2( parameter - 0.5 ) ;
    }

}

TabulSpline::~TabulSpline()
{
  delete [] ( _splineCoef );
  delete [] ( _derivatedSplineCoef ) ;
}
