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

#ifndef _AIMSMIREGISTER_OBJFUNC_H_
#define _AIMSMIREGISTER_OBJFUNC_H_

#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/resampling/sampler.h>
#include <aims/resampling/resampler.h>
#include <aims/information/pdf.h>
#include <aims/information/information.h>
#include <time.h>

class ObjFunc : public ObjectiveFunc<float,6>
{
public:
  
  ObjFunc( int numLevel,
	   Resampler<short>* interpolator,
	   Sampler<short>* comb,	     
	   int maskSize );
  virtual ~ObjFunc();
  
  virtual float eval( const AimsVector<float,6>& ) const { return 0.0; }
  
  void setRef( const AimsData<short>& ref ) ;
  void setTest( const AimsData<short>& test ) ;
  void setGcRef(const float x, const float y, const float z);
  void setGcTest(const float x, const float y, const float z); 
  bool dataSettingOK() const { return _refOK && _testOK; };
  bool usePV() const { return !_interpolator; };

  Motion getDepl( const AimsVector<float,6> &p) const;

  // This method adds int the attributed object ueseful parameters of the
  // objective function to be monitored
  virtual void fillMonitoredParameters( carto::AttributedObject& ) const  ; 
  
protected:
  
  void doPdf( const AimsVector<float,6>& param ) const;
  
  
  const AimsData<short>*  _ref;    // Master ref image geree comme reference
  const AimsData<short>*  _test;   // Master Test image gere comme reference
  AimsData<short>*  _reg;
  AimsData<PVItem>*  _regc;
  
  int                    _numLevel;
  Resampler<short>*      _interpolator;
  Sampler<short>*        _comb;
  AimsData<float>*       _mask;
  AimsData<float>*       _p1;
  AimsData<float>*       _p2;
  AimsData<float>*       _p12;
  
  
  AimsVector<float,3>    _gcref;
  AimsVector<float,3>    _gctest;
  
  bool                   _refOK, _testOK;
};


inline
ObjFunc::ObjFunc( int numLevel, 
		  Resampler<short>* interpolator,
		  Sampler<short>* comb,
		  int maskSize )
        : ObjectiveFunc<float,6>(), _ref( NULL ), _test( NULL ),
	                      _numLevel( numLevel ),
                              _interpolator( interpolator ),
                              _comb( comb )
{
  _refOK = _testOK = false;
  _mask = new AimsData<float>( maskSize );

  for ( int n = 0; n < maskSize; n++ )  
    (*_mask)( n ) = exp( - 0.5 * aims::sqr( n - maskSize / 2 ) );


  _p1  = new AimsData<float>( _numLevel );
  _p2  = new AimsData<float>( _numLevel );
  _p12 = new AimsData<float>( _numLevel, _numLevel );
 
  _ref = _test = NULL;
  _gctest = _gcref = AimsVector<float, 3>(0., 0., 0.) ;
  
}


inline
void ObjFunc::setGcRef(const float x, const float y, const float z)
{
  _gcref[0] = x;
  _gcref[1] = y;
  _gcref[2] = z;
}

inline
void ObjFunc::setGcTest(const float x, const float y, const float z)
{
  _gctest[0] = x;
  _gctest[1] = y;
  _gctest[2] = z;
}


inline
void ObjFunc::setTest( const AimsData<short>& test )
{

  _test = &test;
  //_Vtest = converter.doit( *_test );
  // Ici insertion des codes de seuils et morpho
  _testOK = true;
}

inline
void ObjFunc::setRef( const AimsData<short>& ref )
{

  _ref = &ref;
  if ( usePV() )
    {
      _regc = new AimsData<PVItem>(  ref.dimX(), ref.dimY(), ref.dimZ() );
      _regc->setSizeXYZT( ref.sizeX(), ref.sizeY(), ref.sizeZ(), 1.0);
    }
  else
    {
      _reg = new AimsData<short>( ref.dimX(), ref.dimY(), ref.dimZ() );
      _reg->setSizeXYZT( ref.sizeX(), ref.sizeY(), ref.sizeZ(), 1.0);
    }

  _refOK = true;
}



inline
ObjFunc::~ObjFunc()
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
void ObjFunc::doPdf( const AimsVector<float,6>& param ) const
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
      _comb->doit( motion, *_regc );
      AimsJointPVPdf( *_ref, *_test, *_regc,  *_p12, *_p1, *_p2 );
    }
  else         // Calcul direct de la pdf
    {
      ASSERT( _interpolator );  // il doit exister necesaire un interpolateur
      _interpolator->setRef( *_test );
      
      _interpolator->doit( motion, *_reg );
      AimsJointMaskPdf( *_ref, *_reg, *_p12, *_p1, *_p2 );
    }
}


inline
Motion ObjFunc::getDepl( const AimsVector< float,6 > &param ) const
{
  Motion motion;

  // rotation autout du centre de gravite dans l'image test.
  motion.setRotationAffine(param[4], param[5], param[2], _gctest);
  // translation des centres de gravite  
  motion.setTranslation(_gcref-_gctest+Point3df(param[0],param[1],param[3]));

  return(motion);
}

void ObjFunc::fillMonitoredParameters( carto::AttributedObject& o ) const 
{
  o.setProperty( "joint_histogram", *_p12 ) ;
  o.setProperty( "mask", *_mask ) ;
  o.setProperty( "gc_ref", _gcref ) ;
  o.setProperty( "gc_test", _gctest ) ;  
  
  return ;
} 

#endif
