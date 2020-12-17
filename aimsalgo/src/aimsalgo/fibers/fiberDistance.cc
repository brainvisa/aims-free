#include <aims/fibers/fiberDistance.h>

using namespace std;
using namespace carto;
using namespace aims;

namespace aims
{

float fiberDistance(FiberPoint* f1, FiberPoint* f2, int &fiberSize1, int &fiberSize2)
{
  // calculate mean closest points distance
  // f1 and f2 must have the same length
  float dist1 = 0.0, dist2 = 0.0;
  Point3df pMin;

  // distance(f1, f2)
  for (int i = 0; i < fiberSize1; i++)
  {

    dist1 += getClosestFiberPointDist(f1[i], f2, pMin, fiberSize2);
  }

  // distance(f2, f1)
  for (int i = 0; i < fiberSize2; i++)
  {
    dist2 += getClosestFiberPointDist(f2[i], f1, pMin, fiberSize1);
  }

  //calculate the average
  float dist = (dist1 + dist2)/(fiberSize1 + fiberSize1);

  return dist;

} // fiberDistance

float fiberDistance(vector<Point3df>& f1, vector<Point3df>& f2)
{
  // calculate mean closest points distance
  // f1 and f2 must have the same length
  float dist1 = 0.0, dist2 = 0.0;
  Point3df pMin;

  // distance(f1, f2)
  for (size_t i = 0; i < f1.size(); i++)
  {
    dist1 += getClosestFiberPointDist(f1[i], f2, pMin);
  }

  // distance(f2, f1)
  for (size_t i = 0; i < f2.size(); i++)
  {
    dist2 += getClosestFiberPointDist(f2[i], f1, pMin);
  }

  //calculate the average
  float dist = (dist1 + dist2)/(f1.size() + f1.size());

  return dist;

} // fiberDistance


float fiberDistanceMax(FiberPoint* f1, FiberPoint* f2, int &fiberSize1, int &fiberSize2)
{
  // calculate maximum closest points distance
  // f1 and f2 must have the same length
  float dist1 = 0.0, dist2 = 0.0;
  Point3df pMin;

  // distance(f1, f2)
  for (int i = 0; i < fiberSize1; i++)
  {
    dist1 += getClosestFiberPointDist(f1[i], f2, pMin, fiberSize2);
  }

  // distance(f2, f1)
  for (int i = 0; i < fiberSize2; i++)
  {
    dist2 += getClosestFiberPointDist(f2[i], f1, pMin, fiberSize1);
  }

  //select the bigger distance
  float dist = dist1/fiberSize1;
  if (dist2 > dist1) dist = dist2/fiberSize1;

  return dist;

} // fiberDistanceMax

float fiberDistanceMax(vector<Point3df>& f1, vector<Point3df>& f2)
{
  // calculate maximum closest points distance
  // f1 and f2 must have the same length
  float dist1 = 0.0, dist2 = 0.0;
  Point3df pMin;

  // distance(f1, f2)
  for (size_t i = 0; i < f1.size(); i++)
  {
    dist1 += getClosestFiberPointDist(f1[i], f2, pMin);
  }

  // distance(f2, f1)
  for (size_t i = 0; i < f2.size(); i++)
  {
    dist2 += getClosestFiberPointDist(f2[i], f1, pMin);
  }

  //select the bigger distance
  float dist = dist1/f1.size();
  if (dist2 > dist1) dist = dist2/f1.size();

  return dist;

} // fiberDistanceMax

float getClosestFiberPointDist(Point3df& p1, FiberPoint* f2, Point3df& pMin, int &fiberSize2)
{

  // calculate closest
  float min = 10e10, norm2;
  Point3df p3;

  for (int i = 0; i < fiberSize2; i++)
  {
    p3 = f2[i] - p1;

    norm2 = p3.norm2();

    if (norm2 < min)
    {
      min = norm2;
      pMin = p3;
    }
  }

  return sqrt(min);

} // getClosestFiberPointDist

float getClosestFiberPointDist(Point3df& p1, vector<Point3df>& f2, Point3df& pMin)
{
  // calculate closest
  float min = 10e10, norm2;
  Point3df p3;

  for (size_t i = 0; i < f2.size(); i++)
  {
    p3 = f2[i] - p1;
    norm2 = p3.norm2();
    if (norm2 < min)
    {
      min = norm2;
      pMin = p3;
    }
  }

  return sqrt(min);

} // getClosestFiberPointDist


float fiberDistanceMaxDistCorrespPoints(FiberPoint* f1, FiberPoint* f2, int &fiberSize1)
{
  // use point correspondence
  // f1 and f2 must have the same length
  float dist = 0.0, hd1 = -1.0, hd2 = -1.0;
  Point3df p;
  int size = fiberSize1;

  // distance(f1, f2)
  for (int i = 0; i < size; i++)
  {
    p = f2[i] - f1[i];
    dist = p.norm2();
    if (dist > hd1)
    {
       hd1 = dist;
    }
  }

  // distance(f1, f2)
  for (int i = 0; i < size; i++)
  {
    p = f2[size - i - 1] - f1[i];
    dist = p.norm2();

    if (dist > hd2)
    {
       hd2 = dist;
    }
  }

  if (hd1 < hd2)
  {
     dist = hd1;
  }
  else
  {
     dist = hd2;
  }

  //cout << "fiberDistanceHausdorff: dist 2 = " << sqrt(dist) << endl;
  return sqrt(dist);

} // fiberDistanceMaxDistCorrespPoints



float fiberDistanceMaxDistCorrespPoints(vector<Point3df>& f1, vector<Point3df>& f2)
{
  // use point correspondence
  // f1 and f2 must have the same length
  float dist = 0.0, hd1 = -1.0, hd2 = -1.0;
  Point3df p;
  int size = f1.size();

  // distance(f1, f2)
  for (int i = 0; i < size; i++)
  {
    p = f2[i] - f1[i];
    dist = p.norm2();

    if (dist > hd1)
    {
       hd1 = dist;
    }
  }

  // distance(f1, f2)
  for (int i = 0; i < size; i++)
  {
    p = f2[size - i - 1] - f1[i];
    dist = p.norm2();

    if (dist > hd2)
    {
       hd2 = dist;
    }
  }

  if (hd1 < hd2)
  {
     dist = hd1;
  }
  else
  {
     dist = hd2;
  }

  //cout << "fiberDistanceHausdorff: dist 2 = " << sqrt(dist) << endl;
  return sqrt(dist);

} // fiberDistanceMaxDistCorrespPoints


//calculate min fibers length, assumes uniformly spaced points
float calculateMinFibersLength(FiberPoint* f1, FiberPoint* f2, int &fiberSize1, int &fiberSize2)
{
   //fiber 1
   Point3df diff = f1[1] - f1[0];
   float d = diff.norm();
   float l1 = float(fiberSize1) * d;
   //fiber 2
   diff = f2[1] - f2[0];
   d = diff.norm();
   float l2 = float(fiberSize2) * d;

   if (l1 < l2)
   {
      return l1;
   }
   else
   {
      return l2;
   }
}

float calculateMinFibersLength(vector<Point3df>& f1, vector<Point3df>& f2)
{
   //fiber 1
   Point3df diff = f1[1] - f1[0];
   float d = diff.norm();
   float l1 = float(f1.size()) * d;
   //fiber 2
   diff = f2[1] - f2[0];
   d = diff.norm();
   float l2 = float(f2.size()) * d;

   if (l1 < l2)
   {
      return l1;
   }
   else
   {
      return l2;
   }
}

//
float fiberDistanceMaxDistCorrespPointsNormByMinLength(FiberPoint* f1, FiberPoint* f2, float normFactor, int &fiberSize1, int &fiberSize2)
{
   float dist = fiberDistanceMaxDistCorrespPoints(f1, f2, fiberSize1);
   float l = calculateMinFibersLength(f1, f2, fiberSize1, fiberSize2);
   float c1 = normFactor * 0.08333333333335502;
   float c2 = normFactor * 0.15000000000001501 + c1;

   float norm_dist;
   if (l < 70)
   {
      norm_dist = dist - normFactor + (200.0 - l)*0.005555555555555*normFactor;
   }
   else if (l < 110)
   {
      norm_dist = dist - normFactor*1.3 + c1 + (200.0 - l)*0.005555555555555*normFactor*1.3;
   }
   else
   {
      norm_dist = dist - normFactor*1.6 + c2 + (200.0 - l)*0.005555555555555*normFactor*1.6;
   }

   if (norm_dist < 0.0)
   {
      norm_dist =  0.1;
   }

   return norm_dist;
}

float fiberDistanceMaxDistCorrespPointsNormByMinLength(vector<Point3df>& f1, vector<Point3df>& f2, float normFactor)
{
   float dist = fiberDistanceMaxDistCorrespPoints(f1, f2);
   float l = calculateMinFibersLength(f1, f2);
   float c1 = normFactor*0.08333333333335502;
   float c2 = normFactor*0.15000000000001501 + c1;

   float norm_dist;
   if (l < 70)
   {
      norm_dist = dist - normFactor + (200.0 - l)*0.005555555555555*normFactor;
   }
   else if (l < 110)
   {
      norm_dist = dist - normFactor*1.3 + c1 + (200.0 - l)*0.005555555555555*normFactor*1.3;
   }
   else
   {
      norm_dist = dist - normFactor*1.6 + c2 + (200.0 - l)*0.005555555555555*normFactor*1.6;
   }

   if (norm_dist < 0.0)
   {
      norm_dist =  0.1;
   }

   return norm_dist;
}

float fiberAffinityMax(vector<Point3df>& f1, vector<Point3df>& f2, float var)
{
  float dist, aff;
  dist = fiberDistanceMax(f1, f2);
  aff = exp(-dist*dist/var);
  //cout << "dist:" << dist << ", affinity: " << aff << endl;

  return aff;

} // fiberAffinityMax


float fiberAffinity(vector<Point3df>& f1, vector<Point3df>& f2, float var)
{
  float dist, aff;
  dist = fiberDistance(f1, f2);
  aff = exp(-dist*dist/var);
  //cout << "dist:" << dist << ", affinity: " << aff << endl;

  return aff;

} // fiberAffinity

float fiberAffinityMaxDistCorrespPoints(vector<Point3df>& f1, vector<Point3df>& f2, float var)
{
  float dist, aff;
  dist = fiberDistanceMaxDistCorrespPoints(f1, f2);
  aff = exp(-dist*dist/var);

  return aff;

} // fiberAffinityMaxDistCorrespPoints


float fiberAffinityMaxDistCorrespPointsNormByMinLength(vector<Point3df>& f1, vector<Point3df>& f2, float var, float normFactor)
{
  float dist, aff;
  dist = fiberDistanceMaxDistCorrespPointsNormByMinLength(f1, f2, normFactor);
  aff = exp(-dist*dist/var);

  return aff;

} // fiberAffinityMaxDistCorrespPoints

//////////////////////////////
//Affinity with Fiberpoint*


float fiberAffinityMax(FiberPoint* f1, FiberPoint* f2, float var, int &fiberSize1, int &fiberSize2)
{
  float dist, aff;
  dist = fiberDistanceMax(f1, f2, fiberSize1, fiberSize2);
  aff = exp(-dist*dist/var);
  //cout << "dist:" << dist << ", affinity: " << aff << endl;

  return aff;

} // fiberAffinityMax


float fiberAffinity(FiberPoint* f1, FiberPoint* f2, float var, int &fiberSize1, int &fiberSize2)
{
  float dist, aff;
  dist = fiberDistance(f1, f2, fiberSize1, fiberSize2);
  aff = exp(-dist*dist/var);
  //cout << "dist:" << dist << ", affinity: " << aff << endl;

  return aff;

} // fiberAffinity

float fiberAffinityMaxDistCorrespPoints(FiberPoint* f1, FiberPoint* f2, float var, int &fiberSize1, int & /*fiberSize2*/)
{
  float dist, aff;
  dist = fiberDistanceMaxDistCorrespPoints(f1, f2, fiberSize1);
  aff = exp(-dist*dist/var);

  return aff;

} // fiberAffinityMaxDistCorrespPoints


float fiberAffinityMaxDistCorrespPointsNormByMinLength(FiberPoint* f1, FiberPoint* f2, float var, float normFactor, int &fiberSize1, int &fiberSize2)
{
  float dist, aff;
  dist = fiberDistanceMaxDistCorrespPointsNormByMinLength(f1, f2, normFactor, fiberSize1, fiberSize2);
  aff = exp(-dist*dist/var);

  return aff;

} // fiberAffinityMaxDistCorrespPoints


}



