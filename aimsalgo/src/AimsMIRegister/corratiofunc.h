#ifndef AIMS_AIMSMIREGISTER_CORRATIOFUNC_H
#define AIMS_AIMSMIREGISTER_CORRATIOFUNC_H

#include "objfunc.h"
#include <aims/optimization/optimization_g.h>
#include <aims/data/data_g.h>
#include <aims/information/pdf.h>

// ============================================================
// Definition
// ============================================================

// ------------------------------------------------------------
// CorRatioFunc<T, P, S>
// ------------------------------------------------------------
template <class T=DefaultVoxelType, 
          class P=DefaultParameterType, 
          int S=DefaultParametersSize>
class CorRatioFunc: public ObjFunc<T, P, S> {
  public:
    // ------------------------------------------------------------
    // Public type definitions
    // ------------------------------------------------------------
    typedef ObjFunc<T, P, S> BaseType;

    typedef typename BaseType::VoxelType VoxelType;
    typedef typename BaseType::ResamplerType ResamplerType;
    typedef typename BaseType::SamplerType SamplerType;

    typedef typename BaseType::ParameterType ParameterType;
    typedef typename BaseType::ParametersType ParametersType;

    // ------------------------------------------------------------
    // Public methods
    // ------------------------------------------------------------
    CorRatioFunc(int numLevel, 
                 ResamplerType * interpolator,
                 SamplerType * comb,
                 int maskSize )
      : BaseType(numLevel, interpolator, comb, maskSize) {}
    ~CorRatioFunc() {}

    virtual float eval(const ParametersType & param) const;
};

// ============================================================
// Implementation
// ============================================================

// ------------------------------------------------------------
// CorRatioFunc<T, P, S>
// ------------------------------------------------------------
template <class T, class P, int S>
inline
float CorRatioFunc<T, P, S>::eval(const ParametersType & param) const
{
  ASSERT(this->dataSettingOK());
  this->doPdf(param);
  float cr = AimsCorrelationRatio(*(this->_p1), *(this->_p2), *(this->_p12));

#ifdef DEBUG
  std::cout << "Eval: " << cr << " " << param << std::endl;
#endif

  return cr;
}


#endif
