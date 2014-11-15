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


#ifndef AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_H
#define AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_H

#include <aims/primalsketch/imageCoupledSmoother.h>
#include <aims/signalfilter/g2dsmooth.h>

namespace aims
{

  template<typename T> class CoupledDiffusion2DSmoother
    : public CoupledSmoother<AimsData<T>, AimsData<T> >
  {
  private:
    float _dt;
    float _alpha;
    float _beta;
    float _gamma;

  public:

    CoupledDiffusion2DSmoother( float delta_t, float alpha=1.0f,
                                float beta=1.0f, float gamma=1.0f )
      : _alpha(alpha), _beta(beta), _gamma(gamma)
    { SetDt(delta_t); }
    void SetDt(float Delta_t)
    {
      if (Delta_t<=0.25) _dt=Delta_t;
      else
      {
        std::cerr << "Diffusion Smoother : dt must be <= 0.25" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    float dt() {return _dt;}   //smooooth operatooor...

    std::pair<AimsData<T>, AimsData<T> > doSmoothing(
      const std::pair<AimsData<T>, AimsData<T> > & ima,
      const std::pair<AimsData<T>, AimsData<T> > & constraint,
      int maxiter, bool verbose=false );

    bool optimal() {return true;}

  };
}

#endif
