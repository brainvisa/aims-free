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


#ifndef AIMS_TOPOLOGY_TOPOBASE_H
#define AIMS_TOPOLOGY_TOPOBASE_H

#include <aims/vector/vector.h>
#include <aims/topology/components18.h>
#include <aims/topology/components26.h>
#include <aims/topology/connectivity26.h>


class TopologyBase
{
public:

  TopologyBase();
  virtual ~TopologyBase();

  int Cstar() { return _cstar; }
  int Cbar()  { return _cbar; }

  int *X() { return myX; }

  void flipX();
  void computeLocalCCNumbers();

  virtual void fillNeighborhood( const Point3d&, int ) { }
  virtual void fillNeighborhood( const Point3d&, int, int ) { }
  virtual void fillNeighborhoodComplement( const Point3d&, int ) { }

protected:

  Connectivity26 connex26;

  Components26Neighborhood *_comp26;
  Components18Neighborhood *_comp18;

  int myX[ 27 ];

  int _cstar;
  int _cbar;
};

#endif
