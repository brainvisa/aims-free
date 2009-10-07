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


#include <aims/topology/topoBase.h>
#include <aims/topology/cc6adjacent.h>
#include <aims/topology/cc26adjacent.h>
#include <aims/topology/connectivity6.h>


TopologyBase::TopologyBase() : _cstar( 0 ), _cbar( 0 )
{
  CC26Adjacent adj26;
  _comp26 = new Components26Neighborhood( connex26, adj26 );

  Connectivity6 connex6;
  CC6Adjacent adj6;
  _comp18 = new Components18Neighborhood( connex6, adj6 );
}


TopologyBase::~TopologyBase()
{
  delete _comp26;
  delete _comp18;
}


void TopologyBase::flipX()
{
  int *xptr = myX + 1;
  for ( int i=26; i--; xptr++ )  *xptr = !(*xptr);
}


void TopologyBase::computeLocalCCNumbers()
{
  _cstar = _comp26->getScalar( myX );
  flipX();
  _cbar = _comp18->getScalar( myX );
}
