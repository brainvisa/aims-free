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



#include <aims/perfusion/perfAifSel.h>

using namespace std;
using namespace aims;

AimsData< float > PerfusionAifSelection::doit( AimsData< float >& d, 
                                               list< Point4dl >&,
					       list< Point4dl >& sel, int )
{
  int t, dt = d.dimT(), n = sel.size();
  list< Point4dl >::iterator it = sel.begin();
    
  AimsData< float > res( dt );
  res.setSizeX( d.sizeT() );

  if ( n < 2 )
    {
      Point4dl pt = *it;
      for ( t=0; t<dt; t++ )  res( t ) = d( pt[0], pt[1], pt[2], t );
    }
  else 
    {
      while( it != sel.end() )
        {
	  Point4dl pt = *it;
	  for ( t=0; t<dt; t++ )  res( t ) += d( pt[0], pt[1], pt[2], t );
	  ++it;
	}
	
      float div = (float)n;
      for ( t=0; t<dt; t++ )  res( t ) /= div;
    }
  
  return res;
}
