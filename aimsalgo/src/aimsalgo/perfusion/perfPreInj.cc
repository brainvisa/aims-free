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



#include <aims/perfusion/perfPreInj.h>

using namespace std;
using namespace aims;

int PerfusionPreInjection::value( AimsData< short >& d, list< Point4d >& sl,
                                  PerfusionParameters& params )
{
  int t, bestT = d.dimT(), skip = params.skip();
  short max10;

  list< Point4d >::iterator it = sl.begin();
  while( it != sl.end() )
    {
      Point4d pt = *it;
      max10 = ( 9 * d( pt[0], pt[1], pt[2], skip + 1 ) + d( pt ) ) / 10;
      t = pt[ 3 ];
      while( d( pt[0], pt[1], pt[2], t ) < max10 && t > skip )  t--;
    
      if ( t - 1 < bestT )  bestT = t - 1;
    
      ++it;
    }

  return ( bestT <= skip ) ? skip + 1 : bestT;
}
