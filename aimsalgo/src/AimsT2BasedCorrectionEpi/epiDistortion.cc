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



#include <iostream>
#include "epiDistortion.h"
#include "interpolation.h"
#include "mutualInfo.h"
#include "smoother.h"

using namespace std;

EpiDistorsion::EpiDistorsion( short* reference, short* distorded, short* output,
                              int size, float resolution,
                              float** p12, float* p1, float* p2, int levels,
                              Smoother* smoother )
              : ObjectiveFunc< float, 3 >(),
                _reference( reference ),
                _distorded( distorded ),
                _output( output ),
                _size( size ),
                _resolution( resolution ),
                _p12( p12 ),
                _p1( p1 ),
                _p2( p2 ),
                _levels( levels ),
                _smoother( smoother )
{
}



EpiDistorsion::~EpiDistorsion()
{
}


float EpiDistorsion::eval( const AimsVector< float, 3 >& param ) const
{

  linearInterpolation( _distorded, _output, _size, _resolution, param );
  getWindowParzenJointPdf( _reference, _output, _size,
                           _p12, _p1, _p2, _levels,
                           _smoother );

  return - mutualInformation( _p1, _p2, _p12, _levels );

}
