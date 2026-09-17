#ifndef _AIMSMIREGISTER_MUTUALFUNC_H_
#define _AIMSMIREGISTER_MUTUALFUNC_H_

#include "objfunc.h"
#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/resampling/sampler.h>
#include <aims/information/pdf.h>
#include <aims/information/information.h>

// ============================================================
// Definition
// ============================================================

// ------------------------------------------------------------
// MutualInfoFunc<T, P, S>
// ------------------------------------------------------------
template <class T=DefaultVoxelType, 
          class P=DefaultParameterType, 
          int S=DefaultParametersSize>
class MutualInfoFunc : public ObjFunc<T, P, S> {
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
    MutualInfoFunc(int numLevel, 
                   ResamplerType* interpolator,
                   SamplerType* comb,
                   int maskSize)
      : BaseType(numLevel, interpolator, comb, maskSize) {}
    ~MutualInfoFunc() {}
      
    virtual float eval(const ParametersType & param) const;
};

// ============================================================
// Implementation
// ============================================================

// ------------------------------------------------------------
// MutualInfoFunc<T, P, S>
// ------------------------------------------------------------
template <class T, class P, int S>
inline
float MutualInfoFunc<T, P, S>::eval(const ParametersType & param) const {
  ASSERT(this->dataSettingOK());
  this->doPdf(param);
  float mi = - AimsMutualInformation(*(this->_p1), *(this->_p2), *(this->_p12));
 
#ifdef DEBUG
  std::cout << "Eval: " << mi << " " << param << std::endl << std::flush;
#endif

  return mi;
}


#endif
