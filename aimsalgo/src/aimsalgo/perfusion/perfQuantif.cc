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


#include <aims/perfusion/perfQuantif.h>
#include <math.h>

using namespace aims;


AimsData< float > PerfusionQuantification::doit( AimsData< short >& d,
						 BucketMap< Void >& mask,
						 PerfusionParameters& pp )
{
  AimsData< float > res( d.dimX(), d.dimY(), d.dimZ(), d.dimT() );
  res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), d.sizeT() );

  BucketMap<Void>::Bucket::iterator it = mask[0].begin();

  int t, dt = d.dimT(), skip = pp.skip(), preinj = pp.preInj();
  float s0, val, te = pp.te() / 1000.0f;
  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      s0 = 0.0f;
      for ( t=skip; t<preinj; t++ )
	s0 += (float)d( pt[0], pt[1], pt[2], t );

      s0 /= (float)(preinj - skip);

      for ( t=skip; t<dt; t++ )
	{
	  val = (float)d( pt[0], pt[1], pt[2], t );

	  if ( val > 0.0f && s0 > 0.0f )  val = -log( val / s0 ) / te;
	  else val = 0.0f;

	  res( pt[0], pt[1], pt[2], t ) = val;
	}

      ++it;
    }

  return res;
}
