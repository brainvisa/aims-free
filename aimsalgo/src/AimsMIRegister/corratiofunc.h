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
