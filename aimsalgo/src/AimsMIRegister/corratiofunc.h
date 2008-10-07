/*
 *  Copyright (C) 1999-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 *  Mutual Information Function Class
 */

#ifndef AIMS_AIMSMIREGISTER_CORRATIOFUNC_H
#define AIMS_AIMSMIREGISTER_CORRATIOFUNC_H


#include "objfunc.h"
#include <aims/optimization/optimization_g.h>
#include <aims/data/data_g.h>
#include <aims/information/pdf.h>


class CorRatioFunc : public ObjFunc
{
  public:
    CorRatioFunc( int numLevel, 
		  Resampler<short>* interpolator,
		  Sampler<short>* comb,
                  int maskSize )
      : ObjFunc( numLevel, interpolator, comb, maskSize ) { }
    ~CorRatioFunc() { }

    float eval( const AimsVector<float,6>& param ) const;
};


inline
float CorRatioFunc::eval( const AimsVector<float,6>& param ) const
{
  ASSERT( dataSettingOK() );
  doPdf( param );
  float cr = AimsCorrelationRatio( *_p1, *_p2, *_p12 );

#ifdef DEBUG
cout << "Eval : " << cr << " " << param << endl;
#endif
  return cr;
}


#endif
