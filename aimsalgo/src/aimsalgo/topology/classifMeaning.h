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


#ifndef AIMS_TOPOLOGY_CLASSIFMEANING_H
#define AIMS_TOPOLOGY_CLASSIFMEANING_H


#include <aims/topology/topoClassifBase.h>


class TopologicalClassificationMeaning
{
public:

  enum TopoType
  {
    TopoA = 10,
    TopoB = 20,
    TopoC = 30,
    TopoD = 40,
    TopoE = 50,
    TopoF = 60,
    TopoG = 70,
    TopoH = 80,
    TopoI = 90
  };

  enum TopoTypeFull
  {
    VolumePoint = 10,
    IsolatedPoint = 20,
    BorderPoint = 30,
    SimplePoint = 30,
    CurvePoint = 40,
    CurvesJunction = 50,
    SurfacePoint = 60,
    SurfaceCurvesJunction = 70,
    SurfacesJunction = 80,
    SurfacesCurvesJunction = 90
  };

  static std::string stringFromDefine( int );
  static std::string name( int, int );
  static int classification( int, int );
};


#endif
