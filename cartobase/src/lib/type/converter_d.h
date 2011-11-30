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

#ifndef CARTOBASE_TYPE_CONVERTER_D_H
#define CARTOBASE_TYPE_CONVERTER_D_H

#include <cartobase/type/converter.h>

namespace carto
{

  template<typename INP, typename OUTP>
  DefaultedRescalerInfo<INP,OUTP>::DefaultedRescalerInfo( const RescalerInfo & info )
  {
    // Initialize scale information
    _defaultedvmin = carto::getcheckedmin<INP>( info.vmin );
    _defaultedvmax = carto::getcheckedmax<INP>( info.vmax );
    _defaultedomin = carto::getcheckedmin<OUTP>( info.omin );
    _defaultedomax = carto::getcheckedmax<OUTP>( info.omax );

    _scale = this->getscale();
  }

  template<typename INP, typename OUTP>
  double DefaultedRescalerInfo<INP,OUTP>::getscale() const {
    double vdiff, odiff;
    vdiff = static_cast<double>(this->_defaultedvmax)
      - static_cast<double>(this->_defaultedvmin);
    vdiff = ( vdiff != 0 ? vdiff : 1 );
    odiff = static_cast<double>(this->_defaultedomax)
      - static_cast<double>(this->_defaultedomin);

    return odiff / vdiff ;
  }

}

#endif
