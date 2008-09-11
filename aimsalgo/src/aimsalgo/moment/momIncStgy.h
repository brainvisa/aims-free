/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_MOMENT_MOMINCSTGY_H
#define AIMS_MOMENT_MOMINCSTGY_H

#include <aims/moment/momStgy.h>


template< class T >
class MomentIncrementalStrategy : public MomentStrategy< T >
{
  public:
  
    MomentIncrementalStrategy() : MomentStrategy< T >() { }
    
    void update( Moment< T > *, double, double, double, int );
    void doit( Moment< T > *, AimsData< T >&, T, int );
    void doit( Moment< T > *, const aims::BucketMap<Void> &, int );
};


template< class T > inline
void MomentIncrementalStrategy< T >::update( Moment< T > *m, double x,
                                             double y, double z, int dir )
{
  double ct = m->ct();
  double c2 = ct * ct;
  double sum = m->sum();
  double a = ( dir == Moment< T >::mSub && sum < 2.0 ) ? 0.0 : ct;
  double d = ( dir == Moment< T >::mAdd && sum < 1.0 ) ? ct  : sum * ct;
  double d2 = ( dir == Moment< T >::mAdd ) ? ( sum + 1.0 ) * ct : 
              ( sum < 2.0 ) ? ct : ( sum - 1.0 ) * ct;
  double d4 = d2 * d2;
  double s = (double)dir;
  double d1 = s - ct / d;

  x *= m->cx();
  y *= m->cy();
  z *= m->cz();
  double x2 = x * x;
  double y2 = y * y;
  double z2 = z * z;
  double x3 = x * x2;
  double y3 = y * y2;
  double z3 = z * z2;
  double xy = x * y;
  double xz = x * z;
  double yz = y * z;
  double x2y = x2 * y;
  double x2z = x2 * z;
  double xy2 = x * y2;
  double y2z = y2 * z;
  double xz2 = x * z2;
  double yz2 = y * z2;
  double xyz = x * y * z;

  double mx = m->m1()[ 0 ];
  double my = m->m1()[ 1 ];
  double mz = m->m1()[ 2 ];
  double mx2 = mx * mx;
  double my2 = my * my;
  double mz2 = mz * mz;

  double m2x = m->m2()[ 0 ];
  double m2y = m->m2()[ 1 ];
  double m2z = m->m2()[ 2 ];
  double m2xy = m->m2()[ 3 ];
  double m2xz = m->m2()[ 4 ];
  double m2yz = m->m2()[ 5 ];

  double xymz = xy * mz;
  double xzmy = xz * my;
  double yzmx = yz * mx;

  double mxd = mx / d;
  double myd = my / d;
  double mzd = mz / d;

  double mxmy = mx * my;
  double mxmz = mx * mz;
  double mymz = my * mz;
  double mxyzd = mxmy * mz / d;

  double mxmyz = mxmy * z;
  double mxymz = mxmz * y;
  double xmymz = mymz * x;

  m->m3()[ 0 ] += s*(x3-3.0*(m2x*(x-mxd)+s*x3*ct+x2*mx)/d2+(2.0*x3*c2+
                  6.0*s*x2*mx*ct+3.0*x*mx2-mx2/d*(3.0*s*x*ct+mx-
                  s*mxd*ct))/d4)*a;
  m->m3()[ 1 ] += s*(y3-3.0*(m2y*(y-myd)+s*y3*ct+y2*my)/d2+(2.0*y3*c2+
                  6.0*s*y2*my*ct+3.0*y*my2-my2/d*(3.0*s*y*ct+my-
                  s*myd*ct))/d4)*a;
  m->m3()[ 2 ] += s*(z3-3.0*(m2z*(z-mzd)+s*z3*ct+z2*mz)/d2+(2.0*z3*c2+
                  6.0*s*z2*mz*ct+3.0*z*mz2-mz2/d*(3.0*s*z*ct+mz-
                  s*mzd*ct))/d4)*a;
  m->m3()[ 3 ] += s*(x2y-(2.0*m2xy*(x-mxd)+m2x*(y-myd)+2.0*xy*mx+3.0*s*
                  x2y*ct+x2*my)/d2+(2.0*x*mxmy+y*mx2+4.0*s*xy*mx*ct+2.0*
                  s*x2*my*ct+2.0*x2y*c2-s*mxd*(mxmy*d1+2.0*x*my*ct+
                  y*mx*ct))/d4)*a;
  m->m3()[ 4 ] += s*(x2z-(2.0*m2xz*(x-mxd)+m2x*(z-mzd)+2.0*xz*mx+3.0*s*
                  x2z*ct+x2*mz)/d2+(2.0*x*mxmz+z*mx2+4.0*s*xz*mx*ct+2.0*
                  s*x2*mz*ct+2.0*x2z*c2-s*mxd*(mxmz*d1+2.0*x*mz*ct+
                  z*mx*ct))/d4)*a;
  m->m3()[ 5 ] += s*(xy2-(2.0*m2xy*(y-myd)+m2y*(x-mxd)+2.0*xy*my+3.0*s*
                  xy2*ct+y2*mx)/d2+(2.0*y*mxmy+x*my2+4.0*s*xy*my*ct+2.0*
                  s*y2*mx*ct+2.0*xy2*c2-s*myd*(mxmy*d1+2.0*y*mx*ct+
                  x*my*ct))/d4)*a;
  m->m3()[ 6 ] += s*(y2z-(2.0*m2yz*(y-myd)+m2y*(z-mzd)+2.0*yz*my+3.0*s*
                  y2z*ct+y2*mz)/d2+(2.0*y*mymz+z*my2+4.0*s*yz*my*ct+2.0*
                  s*y2*mz*ct+2.0*y2z*c2-s*myd*(mymz*d1+2.0*y*mz*ct+
                  z*my*ct))/d4)*a;
  m->m3()[ 7 ] += s*(xz2-(2.0*m2xz*(z-mzd)+m2z*(x-mxd)+2.0*xz*mz+3.0*s*
                  xz2*ct+z2*mx)/d2+(2.0*z*mxmz+x*mz2+4.0*s*xz*mz*ct+2.0*
                  s*z2*mx*ct+2.0*xz2*c2-s*mzd*(mxmz*d1+2.0*z*mx*ct+
                  x*mz*ct))/d4)*a;
  m->m3()[ 8 ] += s*(yz2-(2.0*m2yz*(z-mzd)+m2z*(y-myd)+2.0*yz*mz+3.0*s*
                  yz2*ct+z2*my)/d2+(2.0*z*mymz+y*mz2+4.0*s*yz*mz*ct+2.0*
                  s*z2*my*ct+2.0*yz2*c2-s*mzd*(mymz*d1+2.0*z*my*ct+
                  y*mz*ct))/d4)*a;
  m->m3()[ 9 ] += s*(xyz-(m2yz*(x-mxd)+m2xz*(y-myd)+m2xy*(z-mzd)+3.0*s*
                  xyz*ct+xymz+xzmy+yzmx)/d2+(2.0*xyz*c2+2.0*s*(yzmx+xymz+
                  xzmy)*ct+xmymz+mxymz+mxmyz-mxyzd-s*ct*(xmymz+mxymz+mxmyz-
		  mxyzd)/d)/d4)*a;

  m->m2()[ 0 ] += s * ( x2 - ( 2*x*mx + s*x2*ct - mx2/d ) / d2 ) * a;
  m->m2()[ 1 ] += s * ( y2 - ( 2*y*my + s*y2*ct - my2/d ) / d2 ) * a;
  m->m2()[ 2 ] += s * ( z2 - ( 2*z*mz + s*z2*ct - mz2/d ) / d2 ) * a;
  m->m2()[ 3 ] += s * ( xy - ( x*my + y*mx + s*xy*ct - mxmy/d ) / d2 ) * a;
  m->m2()[ 4 ] += s * ( xz - ( x*mz + z*mx + s*xz*ct - mxmz/d ) / d2 ) * a;
  m->m2()[ 5 ] += s * ( yz - ( y*mz + z*my + s*yz*ct - mymz/d ) / d2 ) * a;

  m->m1()[ 0 ] += s * x * ct;
  m->m1()[ 1 ] += s * y * ct;
  m->m1()[ 2 ] += s * z * ct;

  m->m0() += s * ct;

  m->sum() += s;

  double dd = ( m->sum() < 2 ) ? ct : m->sum() * ct;
  m->gravity()[ 0 ] = m->m1()[ 0 ] / dd;
  m->gravity()[ 1 ] = m->m1()[ 1 ] / dd;
  m->gravity()[ 2 ] = m->m1()[ 2 ] / dd;
}


template< class T > inline
void MomentIncrementalStrategy< T >::doit( Moment< T > *m, AimsData< T >& d,
                                           T label, int dir )
{
  int x, y, z;
  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  int olbs = d.oLineBetweenSlice();
  int opbl = d.oPointBetweenLine();
  typename AimsData< T >::const_iterator it = d.begin() + d.oFirstPoint();

  for ( z=0; z<dz; z++, it+=olbs )
    for ( y=0; y<dy; y++, it+=opbl )
      for ( x=0; x<dx; x++, it++ )
        if ( *it == label )
          update( m, (double)x, (double)y, (double)z, dir );
}


template< class T > inline
void MomentIncrementalStrategy< T >::doit( Moment< T > *m, 
                                           const aims::BucketMap<Void> & b, 
                                           int dir )
{
  if (!b.empty())
  {
    const aims::BucketMap<Void>::Bucket	& bk = b.begin()->second;	

    if (!bk.empty())
    {
      aims::BucketMap<Void>::Bucket::const_iterator	ib, eb = bk.end();

      for( ib=bk.begin(); ib!=eb; ++ib )
      {
        update( m, ib->first[0], ib->first[1], ib->first[2], dir );
      }
    }
  }
}

#endif
