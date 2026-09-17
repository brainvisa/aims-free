
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
