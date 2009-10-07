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



#include <iostream>
#include <unistd.h>
#include <aims/optimization/optimization_g.h>
#include <cartobase/thread/thread.h>
#include "contextEPI.h"
#include "epiDistortion.h"
#include "mutualInfo.h"
#include "g2dSmoother.h"
#include "g2dSmootherD.h"

using namespace std;
using namespace carto;
using namespace aims;

ContextEpi::ContextEpi( int numT2,
                        int size, int sizeDivider, float res,
                        int levels, double ftol,
			const AimsVector< float, 3 >& deltaP,
                        AimsData< float >& S,
                        AimsData< float >& T0,
                        AimsData< float >& T1,
			bool isVerbose )
  : LoopContext(),
    verbose( isVerbose ),
    _numT2( numT2 ),
    _sizeDivider( sizeDivider ),
    _levels( levels ),
    _ftol( ftol ),
    _deltaP( deltaP ),
    _Sss( S ),
    _T0( T0 ),
    _T1( T1 )
             
{

  _realSize = size;
  _realRes = res;
  _procSize = size / sizeDivider;
  _procRes = res * sizeDivider;

  init();

}


ContextEpi::ContextEpi( int numT2,
                        int size, int sizeDivider, float res,
                        int levels, double ftol,
			const AimsVector< float, 3 >& deltaP,
                        AimsData< float >& S,
                        AimsData< float >& T0,
                        AimsData< float >& T1,
                        ThreadGauge* gauge, ThreadCancel* cancel,
			bool isVerbose )
           : LoopContext( gauge, cancel ),
	     verbose( isVerbose ),
             _numT2( numT2 ),
             _sizeDivider( sizeDivider ),
             _levels( levels ),
             _ftol( ftol ),
	     _deltaP( deltaP ),
             _Sss( S ),
             _T0( T0 ),
             _T1( T1 )
             
{

  _realSize = size;
  _realRes = res;
  _procSize = size / sizeDivider;
  _procRes = res * sizeDivider;

  init();

}



void ContextEpi::init()
{

  int t2;

  if ( _sizeDivider != 1 )
  {

    _ref = new short[ _procSize * _procSize ];

    _dis = new short*[ _numT2 - 1 ];
    for ( t2 = 0; t2 < _numT2 - 1; t2++ )
      _dis[ t2 ] = new short[ _procSize * _procSize ];

  }

  _out = new short*[ _numT2 - 1 ];
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    _out[ t2 ] = new short[ _procSize * _procSize ];


  _p1 = new float*[ _numT2 - 1 ];
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    _p1[ t2 ] = new float[ _levels ];

  _p2 = new float*[ _numT2 - 1 ];
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    _p2[ t2 ] = new float[ _levels ];

  _p12linear = new float*[ _numT2 - 1 ];
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    _p12linear[ t2 ] = new float[ _levels * _levels ];

  _p12 = new float**[ _numT2 - 1 ];
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
  {

    _p12[ t2 ] = new float*[ _levels ]; 
    _p12[ t2 ][ 0 ] = _p12linear[ t2 ];
    for ( int i = 1; i < _levels; i++ )
      _p12[ t2 ][ i ] = _p12[ t2 ][ i - 1 ] + _levels;

  }

  _smoothingType = DericheGaussian;
  _sigma = 1.0 / _procRes;
  _sizeMask = 0;

}


ContextEpi::~ContextEpi()
{

  int t2;

  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    delete [] _p12[ t2 ];
  delete [] _p12;

  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    delete [] _p12linear[ t2 ];
  delete [] _p12linear;
  
  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    delete [] _p2[ t2 ];
  delete [] _p2;

  for ( t2 = 0; t2 < _numT2 - 1; t2++ )
    delete [] _p1[ t2 ];
  delete [] _p1;

  if ( _sizeDivider != 1 )
  {

    for ( t2 = 0; t2 < _numT2 - 1; t2++ )
      delete [] _out[ t2 ];
    delete [] _out;


    for ( t2 = 0; t2 < _numT2 - 1; t2++ )
      delete [] _dis[ t2 ];
    delete [] _dis;

    delete [] _ref;

  }

}


void ContextEpi::setDericheGaussianSmoothing( float sigma )
{

  _smoothingType = DericheGaussian;
  _sigma = sigma;
  _sizeMask = 0;

}


void ContextEpi::setDiscreteGaussianSmoothing( int sizeMask )
{

  _smoothingType = DiscreteGaussian;
  _sigma = 1.0;
  _sizeMask = sizeMask;

}


void ContextEpi::setLocation( short** t2Images, int location )
{

  _t2Images = t2Images;
  _location = location;

  if ( _sizeDivider == 1 )
  {

    _ref = t2Images[ 0 ];
    _dis = &t2Images[ 1 ];

  }
  else
  {

    int x, y, t2;

    for ( y = 0; y < _procSize; y++ )
      for ( x = 0; x < _procSize; x++ )
        _ref[ x + y * _procSize ] = t2Images[ 0 ][ _sizeDivider * x +
                                                 _sizeDivider * y * _realSize ];
    for ( t2 = 0; t2 < _numT2 - 1; t2++ )
      for ( y = 0; y < _procSize; y++ )
        for ( x = 0; x < _procSize; x++ )
          _dis[ t2 ][ x + y * _procSize ] = t2Images[ t2 + 1 ][
                                                _sizeDivider * x +
                                                _sizeDivider * y * _realSize ];

  }


}





void ContextEpi::doIt( int startIndex, int /*countIndex*/ )
{

  AimsVector< float, 3 > param( 1.0f, 0.0f, 0.0f );

  Smoother* smoother = 0;
  switch ( _smoothingType )
  {

    case DericheGaussian :

      smoother = new Gaussian2dSmoother( _sigma, _levels );
      break;

    case DiscreteGaussian :

      smoother = new DiscreteGaussian2dSmoother( _sizeMask, _levels );
      break;

    default:

      cerr << "should not pass here" << endl;

  }

  EpiDistorsion 
    objective( _ref, _dis[ startIndex ], _out[ startIndex ], _procSize,
               _procRes, _p12[ startIndex ], _p1[ startIndex ],
               _p2[ startIndex ], _levels, smoother );

  PowellOptimizer< float, 3 > optimizer( objective, _ftol );

  param = optimizer.doit( param, _deltaP );

  _Sss( _location, startIndex ) = param[ 0 ];
  _T0( _location, startIndex ) = param[ 1 ];
  _T1( _location, startIndex ) = param[ 2 ];

  if ( verbose ) {
    cout << "---------------------------------------------------------" << endl
         << "loc=" << _location << " "
         << "t2index=" << startIndex << " "
	 << "S0=" << param[ 0 ] << " " 
	 << "T0=" << param[ 1 ] << " "
	 << "T1=" << param[ 2 ] << " "
	 << "Cost=" << optimizer.getOptCost() << endl
	 << optimizer.getDirection( 0 )[ 0 ] << " "
	 << optimizer.getDirection( 0 )[ 1 ] << " "
	 << optimizer.getDirection( 0 )[ 2 ] << endl
	 << optimizer.getDirection( 1 )[ 0 ] << " "
	 << optimizer.getDirection( 1 )[ 1 ] << " "
	 << optimizer.getDirection( 1 )[ 2 ] << endl
	 << optimizer.getDirection( 2 )[ 0 ] << " "
	 << optimizer.getDirection( 2 )[ 1 ] << " "
	 << optimizer.getDirection( 2 )[ 2 ] << endl << endl;
  }

  delete smoother;

}
