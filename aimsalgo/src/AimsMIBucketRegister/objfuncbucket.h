/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef _AIMSMIREGISTER_OBJFUNCBUCKET_H_
#define _AIMSMIREGISTER_OBJFUNCBUCKET_H_


#include <aims/bucket/bucketMap.h>

aims::BucketMap<short> *toto = new aims::BucketMap<short>;

#include <aims/optimization/objective.h>
#include <aims/resampling/sampler.h>
#include <aims/resampling/resampler.h>
#include <aims/math/mathelem.h>
#include <aims/information/pdf.h>

class ObjFuncBucket : public ObjectiveFunc<float,6>
{
public:
  
  ObjFuncBucket( int numLevel,
		 Resampler<short>* interpolator,
		 BucketSampler<short>* comb,	     
		 int maskSize );
  virtual ~ObjFuncBucket();
  
  virtual float eval( const AimsVector<float,6>& ) const { 
    cout << "eval non initialisee" << endl; return 0.0; }
  
  void setTest( const AimsData<short>& test ) ;
  void setRef( const aims::BucketMap<short>& ref ) ;
  void setGcRef(const float x, const float y, const float z);
  void setGcTest(const float x, const float y, const float z); 
  bool dataSettingOK() const { return (_refOK == 1L) && (_testOK == 1L);};
  bool usePV() const { return !_interpolator; };
  
  Motion getDepl( const AimsVector<float,6> &p) const;

  // This method adds int the attributed object ueseful parameters of the
  // objective function to be monitored
  virtual void fillMonitoredParameters( AttributedObject& ) const  ; 
  
protected:
  
  void doPdf( const AimsVector<float,6>& param ) const;
  
  

  const aims::BucketMap<short>*  _ref;      // Master ref bucket geree comme reference
  const AimsData<short>*         _test;     // Master Test image gere comme test
  AimsData<short>*  _reg;
  vector< PVVectorItem >*  _regc;
  
  int                    _numLevel;
  Resampler<short>*      _interpolator;
  BucketSampler<short>*        _comb;
  AimsData<float>*       _mask;
  AimsData<float>*       _p1;
  AimsData<float>*       _p2;
  AimsData<float>*       _p12;
  
  
  AimsVector<float,3>    _gcref;
  AimsVector<float,3>    _gctest;
  
  int                    _refOK, _testOK;
};


inline
ObjFuncBucket::ObjFuncBucket( int numLevel, 
			      Resampler<short>* interpolator,
			      BucketSampler<short>* comb,
			      int maskSize )
  : ObjectiveFunc<float,6>(), _ref( NULL ), _test( NULL ),
    _numLevel( numLevel ),
    _interpolator( interpolator ),
    _comb( comb )
{
  _refOK = _testOK = 0L;
  _mask = new AimsData<float>( maskSize );

  for ( int n = 0; n < maskSize; n++ )  
    (*_mask)( n ) = exp( - 0.5 * aims::sqr( n - maskSize / 2 ) );


  _p1  = new AimsData<float>( _numLevel );
  _p2  = new AimsData<float>( _numLevel );
  _p12 = new AimsData<float>( _numLevel, _numLevel );
 
  _ref = NULL ;
  _test = NULL;
/*   _test = new aims::BucketMap<short> ; */
  _gctest = _gcref = AimsVector<float, 3>(0., 0., 0.) ;
  
}


inline
void ObjFuncBucket::setGcRef(const float x, const float y, const float z)
{
  _gcref[0] = x;
  _gcref[1] = y;
  _gcref[2] = z;
}

inline
void ObjFuncBucket::setGcTest(const float x, const float y, const float z)
{
  _gctest[0] = x;
  _gctest[1] = y;
  _gctest[2] = z;
}


inline
void ObjFuncBucket::setRef( const aims::BucketMap<short>& ref )
{

  _ref = &ref;

  if ( usePV() )
    {
      _regc = new vector< PVVectorItem > ;
    }
  else
    {
      cout << "ATTENTION NE DEVRAIT JAMAIS PASSER LA..." << endl;
      ASSERT ( 0 );
/*      _reg = new AimsData<short>( ref.dimX(), ref.dimY(), ref.dimZ() ); */
/*       _reg->setSizeXYZT( ref.sizeX(), ref.sizeY(), ref.sizeZ(), 1.0); */
    }

  _refOK = 1L;
}

inline
void ObjFuncBucket::setTest( const AimsData<short>& test )
{

  _test = &test;

  _testOK = 1L;
}



inline
ObjFuncBucket::~ObjFuncBucket()
{
  if ( _p12 )
    delete _p12;
  if ( _p2 )
    delete _p2;
  if ( _p1 )
    delete _p1;
  if ( _mask )
    delete _mask;
  if ( _reg ) delete( _reg );
}

inline
void ObjFuncBucket::doPdf( const AimsVector<float,6>& param ) const
{
  Motion motion;

  // rotation autout du centre de gravite dans l'image test.
  motion.setRotationAffine( param[4], param[5], param[2], _gctest );

  // translation des centres de gravite  
  motion.setTranslation(_gcref-_gctest+Point3df(param[0],param[1],param[3]));

  //------------------- 
  //clock_t start = clock();

  if ( usePV() ) // Calcul pdf par PV 
    {
      _comb->setRef( *_test );
      _comb->doit( motion, *_regc, *_ref ) ;      
      AimsJointPVPdf( *_ref, *_test, *_regc,  *_p12, *_p1, *_p2 );      
    }
  else         // Calcul direct de la pdf
    {
//       ASSERT( _interpolator );  // il doit exister necesaire un interpolateur
//       _interpolator->setRef( *_test );
//       _interpolator->doit( motion, *_reg );
//       AimsJointMaskPdf( *_ref, *_reg, *_p12, *_p1, *_p2 );
    }

// =======================================Dump     en cours d'algo.
//==========================================================
/*   ostrstream     ss; */
/*   ss         <<  param[1]; */

/*   AimsDataWriter<short> dataW3("refsingle"); */
/*   dataW3 << *_ref; */
/*   char nom[64]; */
/*   static int vfc = -1; */
/*   vfc++;cout << "static vfc " << vfc << endl; */
/*   sprintf(nom, "p12_%03d", vfc); */
/*   AimsDataWriter<float> vf( nom ); */
/*   vf << *_p12; */

/*   cout << "DEBUG:OBJF>>" << "etat des parametres internes :" << endl; */
/*   cout << "DEBUG:OBJF>>" <<param << endl; */
/*   cout << "DEBUG:OBJF>>" <<"Vision via motion :" << endl; */
/*   cout << "DEBUG:OBJF>>" <<getDepl( param ) << endl; */
/*   if (!usePV() ) */
/*     { */
/*       sprintf(nom, "testsingle_%03d",(int) vfc); */
/*       AimsDataWriter<short> dataW2(nom); */
/*       dataW2 << *_reg; */
/*     } */
/*   else  // Dump de l#image via le sampling */
/*     { */
/*       float stock1, stock2, stock3; */
/*       const int TWO_THEN_SIXTEEN = 65536; */
/*       AimsData<short> ecrire(_regc->dimX(), _regc->dimY(), _regc->dimZ()); */
/*       ecrire.setSizeX( _regc->sizeX()); */
/*       ecrire.setSizeY( _regc->sizeY()); */
/*       ecrire.setSizeZ( _regc->sizeZ()); */
/*       AimsData<short>::iterator out = ecrire.begin() + ecrire.oFirstPoint(); */
/*       AimsData<short>::const_iterator it; */
/*       AimsData<PVItem>::const_iterator cm = _regc->begin()+_regc->oFirstPoint(); */
/*       for(; cm < _regc->end(); cm++) */
/* 	if (cm->offset != -1) */
/* 	  { */
/* 	    int dx = cm->x; */
/* 	    int dy = cm->y; */
/* 	    int dz = cm->z; */

/* 	    it = _test->begin() + cm->offset; */

/* 	    ----------------	     */
/* 	    stock1 = *it * ( TWO_THEN_SIXTEEN - dx ); */
/* 	    stock1 += *(++it) * dx; */
/* 	    stock1 /= TWO_THEN_SIXTEEN; */

/* 	    it += _ref->dimX(); */
/* 	    stock2 = *it * dx; */
/* 	    stock2 += *(--it) * ( TWO_THEN_SIXTEEN - dx ); */
/* 	    stock2 /= TWO_THEN_SIXTEEN; */

/* 	    stock1 *= ( TWO_THEN_SIXTEEN - dy ); */
/* 	    stock1 += stock2 * dy; */
/* 	    stock1 /= TWO_THEN_SIXTEEN; */

/* 	    it += _ref->oSlice() - _ref->dimX(); */
/* 	    stock2 = *it * ( TWO_THEN_SIXTEEN - dx ); */
/* 	    stock2 += *(++it) * dx; */
/* 	    stock2 /= TWO_THEN_SIXTEEN; */
	
/* 	    it += _ref->dimX(); */
/* 	    stock3 = *it * dx; */
/* 	    stock3 += *(--it) * ( TWO_THEN_SIXTEEN - dx ); */
/* 	    stock3 /= TWO_THEN_SIXTEEN; */
	
/* 	    stock2 *= TWO_THEN_SIXTEEN - dy; */
/* 	    stock2 += stock3 * dy; */
/* 	    stock2 /= TWO_THEN_SIXTEEN; */

/* 	    stock1 *= ( TWO_THEN_SIXTEEN - dz ); */
/* 	    stock1 += stock2 * dz; */

/* 	    *out++ = ( short )( stock1 / TWO_THEN_SIXTEEN ); */
/* 	    ------------------- */
/* 	    *out++ = *it; */
/* 	  } */
/* 	else */
/* 	  *out++ = ( short )-32768; */

/*       sprintf(nom, "testsinglepv%03d",(int) vfc); */
/*       AimsDataWriter<short> dataW2(nom); */
/*       dataW2 << ecrire; */
/*     } */
//==========================================================
  
}


inline
Motion ObjFuncBucket::getDepl( const AimsVector< float,6 > &param ) const
{
  Motion motion;
  
  // rotation autout du centre de gravite dans l'image test.
  motion.setRotationAffine(param[4], param[5], param[2], _gctest);
  // translation des centres de gravite  
  motion.setTranslation(_gcref-_gctest+Point3df(param[0],param[1],param[3]));

  return(motion);
}

void ObjFuncBucket::fillMonitoredParameters( AttributedObject& o ) const 
{
  o.setProperty( "joint_histogram", *_p12 ) ;
  o.setProperty( "mask", *_mask ) ;
  o.setProperty( "gc_ref", _gcref ) ;
  o.setProperty( "gc_test", _gctest ) ;  
  
  return ;
} 

#endif
