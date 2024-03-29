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

/*
 *  Inversion, determinant of a matrix.
 */
#include <cstdlib>
#include <aims/math/gausslu.h>
#include <aims/math/mathelem.h>
#include <cartodata/volume/volume.h>
#include <math.h>
#include <complex>

using namespace std;
using namespace carto;

float
AimsDecompositionLU( VolumeRef<float> &a,
                     VolumeRef<int32_t>  &indx)
{
  ASSERT(a.getSizeZ()==1 && a.getSizeT()==1);
  ASSERT(indx.getSizeY()==1 && indx.getSizeZ()==1 && indx.getSizeT()==1);

  int   i,imax=0,j,k,n;
  float big,dum,sum,temp,TINY=1.0e-20,d;

  n = a.getSizeX();
  ASSERT(n == a.getSizeY());

  VolumeRef<float> vv(n);

  d = 1.0;

  for (i=0;i<n;i++)
  { big = 0.0;
    for (j=0;j<n;j++)
      if ((temp = fabs(a(i,j))) > big)  big = temp;
    ASSERT(big != 0.0);
    vv(i) = 1.0 / big;
  }
  for (j=0;j<n;j++)
  { for (i=0;i<j;i++)
    { sum = a(i,j);
      for (k=0;k<i;k++)  sum -= a(i,k) * a(k,j);
      a(i,j) = sum;
    }

    big = 0.0;
    for (i=j;i<n;i++)
    { sum = a(i,j);
      for (k=0;k<j;k++)  sum -= a(i,k) * a(k,j);
      a(i,j) = sum;
      if ((dum = vv(i) * fabs(sum)) >= big)
      { big  = dum;
        imax = i;
      }
    }

    if (j != imax)
    { for (k=0;k<n;k++)
      { dum = a(imax,k);
        a(imax,k) = a(j,k);
        a(j,k)    = dum;
      }
      d = -d;
      vv(imax) = vv(j);
    }
    indx(j) = imax;
    if (a(j,j) == 0.0)  a(j,j) = TINY;
    if (j != n)
    { dum = 1.0 / a(j,j);
      for (i=j+1;i<n;i++)  a(i,j) *= dum;
    }
  }
  return d;
}


void AimsBackSubstitutionLU( const VolumeRef<float> &a,
                             const VolumeRef<int32_t> &indx,
                             VolumeRef<float> &b)
{
  ASSERT(a.getSizeZ()==1 && a.getSizeT()==1);
  ASSERT(indx.getSizeY()==1 && indx.getSizeZ()==1 && indx.getSizeT()==1);
  ASSERT(b.getSizeY()==1 && b.getSizeZ()==1 && b.getSizeT()==1);

  int   i,ii=-1,ip,j,n;
  float sum;

  n = a.getSizeX();
  ASSERT(n == a.getSizeY());

  for (i=0;i<n;i++)
  { ip = indx(i);
    sum = b(ip);
    b(ip) = b(i);
    if (ii != -1)
      for (j=ii;j<=i-1;j++)  sum -= a(i,j) * b(j);
    else if (sum!=0)  
      ii = i;
    b(i) = sum;
  }
  for (i=n;i--;)
  { sum = b(i);
    for (j=i+1;j<n;j++)  sum -= a(i,j) * b(j);
    b(i) = sum / a(i,i);
  }
}


VolumeRef<float>
AimsInversionLU( const VolumeRef<float> &matrix )
{
  ASSERT(matrix.getSizeZ()==1 && matrix.getSizeT()==1);

  int                 n,i,j;
  //float d;

  n = matrix.getSizeX();
  ASSERT(n == matrix.getSizeY());

  VolumeRef<float> inverse(n,n);
  VolumeRef<float> stockage(n,n);

  for (j=n;j--;)
    for (i=n;i--;)  stockage(i,j) = matrix(i,j);

  VolumeRef<float> column(n);

  VolumeRef<int32_t>  indx(n);

  /*d =*/ AimsDecompositionLU(stockage,indx);

  for (j=0;j<n;j++)
  { for (i=0;i<n;i++)  column(i) = 0.0;
    column(j) = 1.0;
    AimsBackSubstitutionLU(stockage,indx,column);
    for (i=0;i<n;i++)  inverse(i,j) = column(i);
  }
  return inverse;
}


VolumeRef<float>
AimsLinearResolutionLU( const VolumeRef<float> &matrix,
                        const VolumeRef<float> &b )
{
  ASSERT(matrix.getSizeZ()==1 && matrix.getSizeT()==1);
  ASSERT(b.getSizeY()==1 && b.getSizeZ()==1 && b.getSizeT()==1);

  int   n,i,j;
  //float d;

  n = matrix.getSizeX();
  ASSERT(n == matrix.getSizeY());

  VolumeRef<float> stockage(n,n);

  for (j=n;j--;)
    for (i=n;i--;)  stockage(i,j) = matrix(i,j);

  VolumeRef<float> column(n);

  VolumeRef<int32_t> indx(n);

  /*d =*/ AimsDecompositionLU(stockage,indx);

  for (i=0;i<n;i++)  column(i) = b(i);
  AimsBackSubstitutionLU(stockage,indx,column);

  return column;
}


float
AimsDeterminantLU( const VolumeRef<float> &matrix )
{
  ASSERT(matrix.getSizeZ()==1 && matrix.getSizeT()==1);

  float determinant;
  int   j,i,n;  

  n = matrix.getSizeX();
  ASSERT(n == matrix.getSizeY());

  VolumeRef<float> stockage(n,n);

  for (j=n;j--;)
    for (i=n;i--;)  stockage(i,j) = matrix(i,j);

  VolumeRef<int32_t> indx(n);
  determinant = AimsDecompositionLU(stockage,indx);
  for (j=0;j<n;j++)  determinant *= stockage(j,j);

  return determinant;
}


VolumeRef< cfloat >
AimsInversionLU( const VolumeRef< cfloat > &matrix )
{
  ASSERT(matrix.getSizeZ()==1 && matrix.getSizeT()==1);

  int   n,i,j;
  //float d;
  int	x, y;

  n = matrix.getSizeX();
  ASSERT(n == matrix.getSizeY());

  VolumeRef<float> inverse(2*n,2*n);
  VolumeRef<float> stockage(2*n,2*n);

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x,y) = real(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x+n,y+n) = real(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x+n,y) = -imag(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x,y+n) = imag(matrix(x,y));

  VolumeRef<float> column(2*n);

  VolumeRef<int32_t> indx(2*n);

  /*d =*/ AimsDecompositionLU(stockage,indx);

  for (j=0;j<2*n;j++)
  { for (i=0;i<2*n;i++)  column(i) = 0;
    column(j) = 1;
    AimsBackSubstitutionLU(stockage,indx,column);
    for (i=0;i<2*n;i++)  inverse(i,j) = column(i);
  }

  VolumeRef< cfloat > cplxinverse(n,n);

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
    { cplxinverse(x,y) = cfloat(inverse(x,y),inverse(x+n,y));
    }
  return cplxinverse;
}


VolumeRef< cfloat >
AimsLinearResolutionLU( const VolumeRef< cfloat > &matrix,
                        const VolumeRef< cfloat > &b)
{
  ASSERT(matrix.getSizeZ()==1 && matrix.getSizeT()==1);
  ASSERT(b.getSizeY()==1 && b.getSizeZ()==1 && b.getSizeT()==1);

  int   n;
  //float d;
  int	x, y;

  n = matrix.getSizeX();
  ASSERT(n == matrix.getSizeY());

  VolumeRef<float> stockage(2*n,2*n);

  for (y=0;y<n;y++)
    for (int x=0;x<n;x++)
      stockage(x,y) = real(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x+n,y+n) = real(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x+n,y) = -imag(matrix(x,y));

  for (y=0;y<n;y++)
    for (x=0;x<n;x++)
      stockage(x,y+n) = imag(matrix(x,y));

  VolumeRef<float> column(2*n);

  for (x=0;x<n;x++)
    column(x) = real(b(x));

  for (x=0;x<n;x++)
    column(x+n) = imag(b(x));

  VolumeRef<int32_t>  indx(2*n);

  /*d =*/ AimsDecompositionLU(stockage,indx);

  AimsBackSubstitutionLU(stockage,indx,column);

  VolumeRef< cfloat > result(n);

  for (x=0;x<n;x++)
  { result(x) = cfloat(column(x),column(x+n));
  }

  return result;
}
