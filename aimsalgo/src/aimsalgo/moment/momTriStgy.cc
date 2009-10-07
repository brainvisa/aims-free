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


#include <aims/moment/momTriStgy.h>


void MomentTriangleStrategy::computeFirst( Point3df& v1, Point3df& v2, 
					   Point3df& v3 )
{
  double a1, a2, a3, b1, b2, b3, c1, c2, c3;

  a1 = (double)v1[0];
  a2 = (double)v1[1];
  a3 = (double)v1[2];

  b1 = (double)v2[0];
  b2 = (double)v2[1];
  b3 = (double)v2[2];

  c1 = (double)v3[0];
  c2 = (double)v3[1];
  c3 = (double)v3[2];

  _mt1[ 0 ] = a1 + b1 + c1;
  _mt1[ 1 ] = a2 + b2 + c2;
  _mt1[ 2 ] = a3 + b3 + c3;

  _mt2[ 0 ] = a1*(a1+b1+c1)+b1*(b1+c1)+c1*c1;
  _mt2[ 1 ] = a2*(a2+b2+c2)+b2*(b2+c2)+c2*c2;
  _mt2[ 2 ] = a3*(a3+b3+c3)+b3*(b3+c3)+c3*c3;
  _mt2[ 3 ] = 2.0*(a1*a2+b1*b2+c1*c2)+a1*(b2+c2)+b1*(a2+c2)+c1*(a2+b2);
  _mt2[ 4 ] = 2.0*(a1*a3+b1*b3+c1*c3)+a1*(b3+c3)+b1*(a3+c3)+c1*(a3+b3);
  _mt2[ 5 ] = 2.0*(a2*a3+b2*b3+c2*c3)+a2*(b3+c3)+b2*(a3+c3)+c2*(a3+b3);

  _mt3[ 0 ] = (a1+b1+c1)*(a1*a1+b1*b1+c1*c1)+a1*b1*c1;
  _mt3[ 1 ] = (a2+b2+c2)*(a2*a2+b2*b2+c2*c2)+a2*b2*c2;
  _mt3[ 2 ] = (a3+b3+c3)*(a3*a3+b3*b3+c3*c3)+a3*b3*c3;
  _mt3[ 3 ] = 3.0*(a1*a1*a2+b1*b1*b2+c1*c1*c2)+
    2.0*(a1*a2*(b1+c1)+b1*b2*(a1+c1)+c1*c2*(a1+b1))+a1*a1*(b2+c2)+
    b1*b1*(a2+c2)+c1*c1*(a2+b2)+a1*(b1*c2+b2*c1)+a2*b1*c1;
  _mt3[ 4 ] = 3.0*(a1*a1*a3+b1*b1*b3+c1*c1*c3)+
    2.0*(a1*a3*(b1+c1)+b1*b3*(a1+c1)+c1*c3*(a1+b1))+a1*a1*(b3+c3)+
    b1*b1*(a3+c3)+c1*c1*(a3+b3)+a1*(b1*c3+b3*c1)+a3*b1*c1;
  _mt3[ 5 ] = 3.0*(a2*a2*a1+b2*b2*b1+c2*c2*c1)+
    2.0*(a2*a1*(b2+c2)+b2*b1*(a2+c2)+c2*c1*(a2+b2))+a2*a2*(b1+c1)+
    b2*b2*(a1+c1)+c2*c2*(a1+b1)+a2*(b2*c1+b1*c2)+a1*b2*c2;
  _mt3[ 6 ] = 3.0*(a2*a2*a3+b2*b2*b3+c2*c2*c3)+
    2.0*(a2*a3*(b2+c2)+b2*b3*(a2+c2)+c2*c3*(a2+b2))+a2*a2*(b3+c3)+
    b2*b2*(a3+c3)+c2*c2*(a3+b3)+a2*(b2*c3+b3*c2)+a3*b2*c2;
  _mt3[ 7 ] = 3.0*(a3*a3*a1+b3*b3*b1+c3*c3*c1)+
    2.0*(a3*a1*(b3+c3)+b3*b1*(a3+c3)+c3*c1*(a3+b3))+a3*a3*(b1+c1)+
    b3*b3*(a1+c1)+c3*c3*(a1+b1)+a3*(b3*c1+b1*c3)+a1*b3*c3;
  _mt3[ 8 ] = 3.0*(a3*a3*a2+b3*b3*b2+c3*c3*c2)+
    2.0*(a3*a2*(b3+c3)+b3*b2*(a3+c3)+c3*c2*(a3+b3))+a3*a3*(b2+c2)+
    b3*b3*(a2+c2)+c3*c3*(a2+b2)+a3*(b3*c2+b2*c3)+a2*b3*c3;
  _mt3[ 9 ] = 6.0*(a1*a2*a3+b1*b2*b3+c1*c2*c3)+
    2.0*(a1*a2*(b3+c3)+b1*b2*(a3+c3)+c1*c2*(a3+b3)+a1*a3*(b2+c2)+b1*b3*(a2+c2)+
	 c1*c3*(a2+b2)+a1*(b2*b3+c2*c3)+b1*(a2*a3+c2*c3)+c1*(a2*a3+b2*b3))+
    a1*(b2*c3+b3*c2)+b1*(a2*c3+a3*c2)+c1*(a2*b3+a3*b2);
}
