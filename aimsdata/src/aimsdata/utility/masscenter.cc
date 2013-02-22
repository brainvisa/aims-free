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
#include <cartobase/config/verbose.h>
#include <aims/utility/masscenter.h>

template <class T>
MassCenters<T>::MassCenters(const AimsData<T> & data, 
                            bool bin)
: _init(false), 
  _data(data), 
  _bin(bin),
  _interpolator( getLinearInterpolator( data ) ) {
}

template <class T>
MassCenters<T>::MassCenters(const AimsData<T> & data, 
                            const carto::rc_ptr<aims::RoiIterator> & roiIterator, 
                            bool bin)
: _init(false), 
  _data(data), 
  _bin(bin),
  _interpolator( getLinearInterpolator( data ) ),
  _roiIterator(roiIterator) {
}

template <class T>
void MassCenters<T>::doit(bool force) {
  if (!_init || force) {
    _init = true;
    TimedMassCenterInfo tm;
    carto::rc_ptr<aims::MaskIterator> maskIterator;
    
    if (_roiIterator.get() != NULL) {
      _roiIterator->restart();
      while( _roiIterator->isValid() ) {
        maskIterator = _roiIterator->maskIterator();
        tm = maskedmasscenter( *(maskIterator.get()) );
        _masscenterinfos[ _roiIterator->regionName() ] = tm;
        _roiIterator->next();
      }
    }
    else {
      maskIterator = 
      carto::rc_ptr< aims::MaskIterator>( new aims::MaskIteratorOf< AimsData<T> >( _data ) );
      tm = maskedmasscenter(*(maskIterator.get()));
      _masscenterinfos[ "0" ] = tm;
    }
  }
}

template <class T>
typename MassCenters<T>::RoiMassCenterInfo MassCenters<T>::infos() {
  if (!_init){
    doit();
  }
  
  return _masscenterinfos;
}

template <class T>
typename MassCenters<T>::TimedMassCenterInfo
MassCenters<T>::maskedmasscenter( aims::MaskIterator & maskIterator ){
  TimedMassCenterInfo tm;
  int s = _data.dimT();
  double  totsum = 0, totnp = 0;
  double  vol = _data.sizeX() * _data.sizeY() * _data.sizeZ();
  Point3df  pos, totpos = Point3df( 0, 0, 0 );
  
  std::vector<Point3df> masscenters;
  std::vector<double> np;
  std::vector<double> sum;

  masscenters.resize(s);
  np.resize(s);
  sum.resize(s);
  for (int i = 0; i < s; i++){
    masscenters[i] = Point3df(0., 0., 0.);
    np[i] = 0.;
    sum[i] = 0.;
  }
  
  std::vector< aims::Interpolator::Scalar_t > values;
  double val;
  
  while ( maskIterator.isValid() ) {
    // Get interpolated values
    _interpolator->values(maskIterator.valueMillimeters(), values);
    for (int i = 0; i < s; i++ ) {
      val = values[i];
      if ( _bin && (val != 0)) {
        val = 1;
      }

      sum[i] += val;
      if( val > 0 )
        np[i]++;
      masscenters[i] += Point3df(maskIterator.value()[0] * val,
                                  maskIterator.value()[1] * val,
                                  maskIterator.value()[2] * val);
    }
    maskIterator.next();
  }

  for (int t = 0; t < s; t++ ) {
    pos = masscenters[t];
    totpos += pos;
    pos /= sum[t];
    if (carto::verbose){
      std::cout << t << "\t\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] 
                << "\tMass: " << vol * sum[t] << "\tVol: " << vol * np[t] << std::endl;
    }
    
    // Add new mass center info
    //m = MassCenterInfo(pos, vol * sum[t]);
    tm[t] = MassCenterInfo(pos, vol * sum[t]);
    
    totsum += sum[t];
    totnp += np[t];
  }
  
  totpos /= totsum;
  if (carto::verbose){
    std::cout << "---\nGeneral:\t" << totpos[0] << "\t" << totpos[1] << "\t" 
              << totpos[2] << "\tMass: " << vol * totsum << "\tVol: " << vol * totnp 
              << std::endl;
  }
  
  return tm;
}

template class MassCenters<uint8_t>;
template class MassCenters<int8_t>;
template class MassCenters<uint16_t>;
template class MassCenters<int16_t>;
template class MassCenters<uint32_t>;
template class MassCenters<int32_t>;
template class MassCenters<float>;
template class MassCenters<double>;
