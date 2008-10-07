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

#ifndef _AIMSMIREGISTER_MUTUALFUNC_H_
#define _AIMSMIREGISTER_MUTUALFUNC_H_


#include "objfunc.h"
#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/resampling/sampler.h>
#include <aims/information/pdf.h>
#include <aims/information/information.h>

class MutualInfoFunc : public ObjFunc
{
  public:
    MutualInfoFunc( int numLevel, 
		    Resampler<short>* interpolator,
		    Sampler<short>* comb,
                    int maskSize )
      : ObjFunc( numLevel, interpolator, comb, maskSize ) {}
      
    ~MutualInfoFunc() {}
      
    float eval( const AimsVector<float,6>& param ) const;
};


inline
float MutualInfoFunc::eval( const AimsVector<float,6>& param ) const
{
  ASSERT( dataSettingOK() );
  doPdf( param );
  float mi = - AimsMutualInformation( *_p1, *_p2, *_p12 );


  //cout << "Eval : " << mi << " " << param << endl;


  return mi;
}


#endif
