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

#include <aims/math/multilinfit.h>
#include <aims/math/gausslu.h>
#include <cartodata/volume/volume.h>

using namespace carto;

// Y(0->N-1)   X(0->M-1,0->N-1)
// yi = a0 + [b0 b1 ....... bM].[xi(0) xi(1) ......... xi(M)]
VolumeRef<float>
AimsMultilinearLeastSquareFit( const rc_ptr<Volume<float> > &X,
                               const rc_ptr<Volume<float> > &Y )
{ ASSERT(X->getSizeZ()==1 && X->getSizeT()==1); // X = matrix
  ASSERT(Y->getSizeY()==1 && Y->getSizeZ()==1 && Y->getSizeT()==1); // Y = vector
  ASSERT(Y->getSizeX() == X->getSizeY());

  int  N=Y->getSizeX(),M=X->getSizeX();
  float Sy=0;
  int	i, j, k;

  for (i=0;i<N;i++)
    Sy += Y->at(i);

  VolumeRef<float> Sx( M, 1, 1, 1, AllocatorContext::fast() );
  Sx = 0.0;
  for (j=0;j<M;j++)
    for (i=0;i<N;i++)
      Sx(j) += X->at(j,i);

  VolumeRef<float> Sxy( M, 1, 1, 1, AllocatorContext::fast() );
  Sxy = 0.0;
  for (j=0;j<M;j++)
    for (i=0;i<N;i++)
      Sxy(j) += X->at(j,i) * Y->at(i);

  VolumeRef<float> Sxx( M, M, 1, 1, AllocatorContext::fast() );
  Sxx = 0.0;
  for (j=0;j<M;j++)
    for (k=0;k<M;k++)
      for (i=0;i<N;i++)
        Sxx(j,k) += X->at(j,i) * X->at(k,i);

  //                          =>                              ->
  // expression of the system A . [a0 b0 b1 ......... bM-1] = B 
  //

  VolumeRef<float> A( M+1, M+1, 1, 1, AllocatorContext::fast() );
  VolumeRef<float> B( M+1, 1, 1, 1, AllocatorContext::fast() );

  B(0) = Sy;
  for (k=1;k<=M;k++) B(k) = Sxy(k-1);

  A(0,0) = (float)N;
  for (k=1;k<=M;k++)
    A(k,0) = A(0,k) = Sx(k-1);
  
  for (i=1;i<=M;i++)
    for (j=1;j<=M;j++)
      A(i,j) = Sxx(i-1,j-1);

  VolumeRef<float> res( M+1, 1, 1, 1, AllocatorContext::fast() );
  res = AimsLinearResolutionLU(A,B);
  return(res);
}
