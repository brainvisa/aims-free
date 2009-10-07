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



#include <aims/perfusion/perfSVDTikhoG.h>

#include <aims/math/svd.h>

using namespace aims;

AimsData< float > PerfusionSVDTikhonovGradient::doit( AimsData< float >& h, 
                                                      float s )
{
  // SVD decomposition of h ( h = U.W.V' )
  AimsData< float > u = h.clone(); // because it is replaced on SVD output
  AimsData< float > v( h.dimY(), h.dimY() );

  AimsSVD< float > svd;
  AimsData< float > w = svd.doit( u, &v );
  
  float min = w.minimum();
  float max = w.maximum();
  float lambda = s * ( max - min );
  
  int i, j, n = w.dimX();
  AimsData< float > g_prime( n, n-1 );  // Transpose of G
  for ( i=0; i<n-1; i++ )
    {
      g_prime( i, i ) = -1.0f;
      g_prime( i + 1, i ) = 1.0f;
    }
  
  // G'.G
  g_prime = g_prime.cross( transpose( g_prime ) );

  // T = S^2 + lambda * G'.G
  AimsData< float > uu = w.clone();
  for ( i=0; i<n; i++ )
    for ( j=0; j<n; j++ )
      uu( i, j ) = uu( i, j ) * uu( i, j ) + lambda *  g_prime( i, j );

  // svd( T ) = UU.WW.VV'
  AimsData< float > vv( n, n );
  AimsData< float > ww = svd.doit( uu, &vv );

  // T-1 = VV.WW-1.UU'
  for ( i=0; i<n; i++ ) 
    if ( ww( i, i ) > 0.0f )  ww( i, i ) = 1.0f / ww( i, i );

  AimsData< float > tm = vv.cross( ww.cross( transpose( uu ) ) );

  // Get the SVD inverse of h ( h-1 = V.W-2.V'.H' )
  // for solution of (H'.H+lambda.G'.G).x = H'.b  
  // <=>  min(||H.x-b||^2+lambda.||G.x||^2)
  //
  //            ( -1  1  0   ...   0 )
  // with   G = (  0 -1  1  0  ... 0 )   in R^{(N-1)xN}
  //            (  .  .  .  .  ... . )
  //            (  0   ...   0 -1  1 )
  //
  return v.cross( tm.cross( w.cross( transpose( u ) ) ) );
}
