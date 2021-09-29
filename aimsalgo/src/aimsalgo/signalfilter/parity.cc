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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/parity.h>
#include <cartodata/volume/volume.h>

using namespace carto;


VolumeRef<float>
AimsEvenPart(const rc_ptr<Volume<float> > &vec,
             int xmin,int xmax,int center)
{
  VolumeRef<float> even( vec->getSize(), vec->getBorders() );
  even = 0.0;

  int dx = vec->getSizeX();
  for (int x=xmin;x<=xmax;x++)
  { if (x<dx)
      even(x) += 0.5 * vec->at(x);
    if ((2*center-x>=0) &&
        (2*center-x<dx))
      even(x) += 0.5 * vec->at(2*center-x);
  }
  return even;
}


VolumeRef<float>
AimsOddPart(const rc_ptr<Volume<float> > &vec,
            int xmin,int xmax,int center)
{
  VolumeRef<float> odd( vec->getSize(), vec->getBorders() );
  odd = 0.0;

  int dx = vec->getSizeX();
  for (int x=xmin;x<=xmax;x++)
  { if (x<dx)
      odd(x) += 0.5 * vec->at(x);
    if ((2*center-x>=0) &&
        (2*center-x<dx))
      odd(x) -= 0.5 * vec->at(2*center-x);
  }
  return odd;
}
