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



#ifndef AIMS_TOPOLOGY_TOPOCLASSIFBASE_H
#define AIMS_TOPOLOGY_TOPOCLASSIFBASE_H

#include <aims/topology/topoBase.h>


class TopologicalClassificationBase
{
public:

  // Numbering of neighborhood points according to Malandain
  // 
  //  0  1  2     9 10 11    17 18 19
  //  3  4  5    12  C 13    20 21 22
  //  6  7  8    14 15 16    23 24 25
  // 
  // Numbering according to Mangin
  //
  // 19 11 23     7  2 15    20 12 24
  //  8  3 16     1  0  6     9  4 17
  // 21 13 25    10  5 18    22 14 26
  //
  enum PointNumber
  {
    PointC  =  0,
    Point0  = 19,
    Point1  = 11,
    Point2  = 23,
    Point3  =  8,
    Point4  =  3,
    Point5  = 16,
    Point6  = 21,
    Point7  = 13,
    Point8  = 25,
    Point9  =  7,
    Point10 =  2,
    Point11 = 15,
    Point12 =  1,
    Point13 =  6,
    Point14 = 10,
    Point15 =  5,
    Point16 = 18,
    Point17 = 20,
    Point18 = 12,
    Point19 = 24,
    Point20 =  9,
    Point21 =  4,
    Point22 = 17,
    Point23 = 22,
    Point24 = 14,
    Point25 = 26
  };

  TopologicalClassificationBase();
  virtual ~TopologicalClassificationBase() { }

  int Cstar() { return _topo->Cstar(); }
  int Cbar()  { return _topo->Cbar(); }

  void computeLocalCCNumbers( const Point3d&, int );
  void computeLocalCCNumbers( const Point3d&, int, int );
  void computeLocalCCNumbersComplement( const Point3d&, int );

  bool isSimplePoint();
  bool isCurvesPoint();
  bool isSurfacesPoint();
  bool isRealSurfacePoint();

  // Faster method using a binary graph
  bool isSimplePoint( const Point3d&, int );
  bool isSimplePoint( const Point3d&, int, int );
  bool isSimplePointComplement( const Point3d&, int );

protected:

  int bdd( int * );

  TopologyBase *_topo;

  bool _statInv;
};

#endif
