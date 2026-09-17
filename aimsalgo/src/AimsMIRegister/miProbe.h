#ifndef _AIMSMIREGISTER_MIPROBE_H_
#define _AIMSMIREGISTER_MIPROBE_H_

#include <aims/optimization/optimization_g.h>

template <class T, int D>
class MIRegistrationProbe : public OptimizerProbe<T, D> {
public:
  MIRegistrationProbe( int numLevel, int maxIteration = 200, int verb = 0 ) ;
  virtual ~MIRegistrationProbe() ;

  virtual MIRegistrationProbe * clone() ;
  virtual void iteration( const AimsVector<T,D> & parameters, 
			  const carto::AttributedObject& specific,
			  const float * costFunction = 0) ;
  virtual void test( const AimsVector<T,D> & parameters, 
		     const carto::AttributedObject& specific,
		     const float * costFunction = 0) ;
  virtual void end() ;

private:
  carto::VolumeRef<float>* _pdfOverIterations ;
  int _iteration ;
  int _verbosity ;
} ;



#endif
