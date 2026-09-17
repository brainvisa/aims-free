

#include <iostream>
#include "epiDistortion.h"
#include "interpolation.h"
#include "mutualInfo.h"
#include "smoother.h"

using namespace std;

EpiDistorsion::EpiDistorsion( short* reference, short* distorded, short* output,
                              int size, float resolution,
                              float** p12, float* p1, float* p2, int levels,
                              Smoother* smoother )
              : ObjectiveFunc< float, 3 >(),
                _reference( reference ),
                _distorded( distorded ),
                _output( output ),
                _size( size ),
                _resolution( resolution ),
                _p12( p12 ),
                _p1( p1 ),
                _p2( p2 ),
                _levels( levels ),
                _smoother( smoother )
{
}



EpiDistorsion::~EpiDistorsion()
{
}


float EpiDistorsion::eval( const AimsVector< float, 3 >& param ) const
{

  linearInterpolation( _distorded, _output, _size, _resolution, param );
  getWindowParzenJointPdf( _reference, _output, _size,
                           _p12, _p1, _p2, _levels,
                           _smoother );

  return - mutualInformation( _p1, _p2, _p12, _levels );

}
