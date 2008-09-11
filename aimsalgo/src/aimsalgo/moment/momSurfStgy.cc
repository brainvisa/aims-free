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



#include <aims/moment/momSurfStgy.h>


void MomentSurfacicStrategy::update( Moment< Void > *m, Point3df& v1,
				     Point3df& v2, Point3df& v3 )
{
  double Ar, x, y, z;
  double a1, a2, a3, b1, b2, b3, c1, c2, c3;

  computeFirst( v1, v2, v3 );

  a1 = (double)v1[0];
  a2 = (double)v1[1];
  a3 = (double)v1[2];

  b1 = (double)v2[0];
  b2 = (double)v2[1];
  b3 = (double)v2[2];

  c1 = (double)v3[0];
  c2 = (double)v3[1];
  c3 = (double)v3[2];

  // Surface d'un triangle: Ar = ||(v2-v1)x(v3-v1)||/2
  x = ( b2 - a2 ) * ( c3 - a3 ) - ( b3 - a3 ) * ( c2 - a2 );
  y = ( b3 - a3 ) * ( c1 - a1 ) - ( b1 - a1 ) * ( c3 - a3 );
  z = ( b1 - a1 ) * ( c2 - a2 ) - ( b2 - a2 ) * ( c1 - a1 );
  Ar=(double)sqrt( x*x + y*y + z*z ) / 2.0;

  m->sum()++;

  m->m0() += Ar;

  m->m1()[ 0 ] += Ar * _mt1[ 0 ] / 3.0;
  m->m1()[ 1 ] += Ar * _mt1[ 1 ] / 3.0;
  m->m1()[ 2 ] += Ar * _mt1[ 2 ] / 3.0;

  m->m2()[ 0 ] += Ar * _mt2[ 0 ] / 12.0;
  m->m2()[ 1 ] += Ar * _mt2[ 1 ] / 12.0;
  m->m2()[ 2 ] += Ar * _mt2[ 2 ] / 12.0;
  m->m2()[ 3 ] += Ar * _mt2[ 3 ] / 24.0;
  m->m2()[ 4 ] += Ar * _mt2[ 4 ] / 24.0;
  m->m2()[ 5 ] += Ar * _mt2[ 5 ] / 24.0;

  m->m3()[ 0 ] += Ar * _mt3[ 0 ] / 60.0;
  m->m3()[ 1 ] += Ar * _mt3[ 1 ] / 60.0;
  m->m3()[ 2 ] += Ar * _mt3[ 2 ] / 60.0;
  m->m3()[ 3 ] += Ar * _mt3[ 3 ] / 180.0;
  m->m3()[ 4 ] += Ar * _mt3[ 4 ] / 180.0;
  m->m3()[ 5 ] += Ar * _mt3[ 5 ] / 180.0;
  m->m3()[ 6 ] += Ar * _mt3[ 6 ] / 180.0;
  m->m3()[ 7 ] += Ar * _mt3[ 7 ] / 180.0;
  m->m3()[ 8 ] += Ar * _mt3[ 8 ] / 180.0;
  m->m3()[ 9 ] += Ar * _mt3[ 9 ] / 360.0;
}


void MomentSurfacicStrategy::doit( Moment< Void > *m, AimsSurfaceTriangle& s )
{
  AimsSurfaceTriangle::iterator it = s.begin();

  while ( it != s.end() )
    {
      update( m, it->second.vertex()[0], it->second.vertex()[1], 
	      it->second.vertex()[2] );
      ++it;
    }
}
