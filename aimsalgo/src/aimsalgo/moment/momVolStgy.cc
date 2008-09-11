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



#include <aims/moment/momVolStgy.h>


void MomentVolumicStrategy::update( Moment< Void > *m, Point3df& v1,
				    Point3df& v2, Point3df& v3 )
{
  double det = 0.0;
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

  det = a1*( b2*c3-b3*c2 ) - b1*( a2*c3-a3*c2 ) + c1*( a2*b3-a3*b2 );

  m->sum()++;

  m->m0() += det / 6.0;

  m->m1()[ 0 ] += det * _mt1[ 0 ] / 24.0;
  m->m1()[ 1 ] += det * _mt1[ 1 ] / 24.0;
  m->m1()[ 2 ] += det * _mt1[ 2 ] / 24.0;

  m->m2()[ 0 ] += det * _mt2[ 0 ] / 60.0;
  m->m2()[ 1 ] += det * _mt2[ 1 ] / 60.0;
  m->m2()[ 2 ] += det * _mt2[ 2 ] / 60.0;
  m->m2()[ 3 ] += det * _mt2[ 3 ] / 120.0;
  m->m2()[ 4 ] += det * _mt2[ 4 ] / 120.0;
  m->m2()[ 5 ] += det * _mt2[ 5 ] / 120.0;

  m->m3()[ 0 ] += det * _mt3[ 0 ] / 120.0;
  m->m3()[ 1 ] += det * _mt3[ 1 ] / 120.0;
  m->m3()[ 2 ] += det * _mt3[ 2 ] / 120.0;
  m->m3()[ 3 ] += det * _mt3[ 3 ] / 360.0;
  m->m3()[ 4 ] += det * _mt3[ 4 ] / 360.0;
  m->m3()[ 5 ] += det * _mt3[ 5 ] / 360.0;
  m->m3()[ 6 ] += det * _mt3[ 6 ] / 360.0;
  m->m3()[ 7 ] += det * _mt3[ 7 ] / 360.0;
  m->m3()[ 8 ] += det * _mt3[ 8 ] / 360.0;
  m->m3()[ 9 ] += det * _mt3[ 9 ] / 720.0;
}


void MomentVolumicStrategy::doit( Moment< Void > *m, AimsSurfaceTriangle& s )
{
  AimsSurfaceTriangle::iterator it = s.begin();

  while ( it != s.end() )
    {
      update( m, it->second.vertex()[0], it->second.vertex()[1], 
	      it->second.vertex()[2] );
      ++it;
    }
}
