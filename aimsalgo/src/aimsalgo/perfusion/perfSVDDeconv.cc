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



#include <aims/perfusion/perfSVDDeconv.h>
#include <aims/perfusion/perfAifMatrix.h>
#include <aims/perfusion/perfSVDInv.h>

using namespace aims;


AimsData< float > PerfusionSVDDeconvolution::doit( AimsData< float >& d,
                                                   AimsData< float >& aif,
						   BucketMap< Void >& mask,
						   PerfusionParameters& pp )
{
  int skip = pp.skip(); // preInj();
  int dEnd = d.dimT()-1;

  // Set the Aif in matrix form
  PerfusionAifMatrix pam;
  if ( pp.hasCorrection() )  
    pam.setCorrection( PerfusionAifMatrix::LinearCorrection );
  else  pam.setCorrection( PerfusionAifMatrix::NoCorrection );
  AimsData< float > h = pam.doit( aif, skip, dEnd );

  // Inversion of the convolution kernel matrix
  PerfusionSVDInversion svdInv( (PerfusionSVDInversion::SvdType)pp.svdType() );
  AimsData< float > invh = svdInv.doit( h, pp.svdThreshold() );

  // Perform the deconvolution
  AimsData< float > res( d.dimX(), d.dimY(), d.dimZ(), d.dimT() );
  res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), h.sizeX() );

  BucketMap< Void >::Bucket::iterator it = mask[0].begin();

  int i, n = dEnd - skip + 1;
  float val;
  AimsData< float > vec( n );
  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      for ( i=0; i<n; i++ )
	vec( i ) = d( pt[0], pt[1], pt[2], skip + i );

      // Perform vecd = h-1.vec
      AimsData< float > vecd = invh.cross( vec );

      for ( i=0; i<n; i++ )
	{
	  val = vecd( i );
	  res( pt[0], pt[1], pt[2], skip + i ) = ( val < 0.0f ) ? 0.0f : val;
	}

      ++it;
    }

  return res;
}
