
#ifndef AIMS_TALAIRACH_ACPCNORM_H
#define AIMS_TALAIRACH_ACPCNORM_H

#include <aims/talairach/talairach.h>
#include <aims/resampling/motion.h>


class TalairachACPCNorm : public TalairachReferential
{
 public:
  TalairachACPCNorm();
  virtual ~TalairachACPCNorm() { }

  virtual Motion computeTransformation( const TalairachPoints& );
    
 private:
  float _norm ;

 protected:
  virtual void computeVectors( const TalairachPoints& );
};

#endif
