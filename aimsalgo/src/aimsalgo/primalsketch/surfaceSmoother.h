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


#ifndef AIMS_PRIMALSKETCH_IMAGESMOOTHER_H
#define AIMS_PRIMALSKETCH_IMAGESMOOTHER_H

#include <aims/config/aimsalgo_config.h>
#include <aims/primalsketch/smoother.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/data/data.h>
#include <iostream>
#include <set>
#include <map>

using namespace aims;
using namespace std;

namespace aims
{

  // REM: surface smoothing is geometry dependent, therefore 
  // smoother contains the geometry it is specialised for
  // It can then smooth any texture based on that geometry (mesh)

  template<int D, typename T> class Smoother<AimsSurface<D, Void>, Texture<T> >
  {
  protected:

    AimsSurface<D, Void> *_mesh;
    
  public:
    Smoother() {}
    Smoother(AimsSurface<D> *mesh) {_mesh=mesh;}
    virtual ~Smoother() {}
		virtual float dt() {return 0.0;}                                                                 //smooooth operatooor...                                                        // virtuel pur

    virtual Texture<T> doSmoothing(const Texture<T> & ima, int maxiter, bool verbose=false)=0;
    // Virtuel pur

    virtual bool optimal() {return false;}   // optimal smoothing method:
    //     false -> from original image (ex: convolution)
    //     true -> from previous scale (ex: diffusion)
  };

}

#endif
