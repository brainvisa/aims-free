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

#include <aims/sino/sino_d.h>
#include <cartobase/exception/format.h>
#include <iostream>


using namespace std;
using namespace carto;

namespace aims {

ScannerConf::ScannerConf( int ring, int numSeg, int ringDiff, int span, int numAngles, int numBin, 
			  float zResolution, float binResolution, float /*faceRadius*/ ) :
  _ring(ring), _ringDiff(ringDiff), _span(span), _numSeg(numSeg), _numAngles(numAngles), _numBins(numBin),
  _zResolution(zResolution), _binResolution(binResolution)
{
  _numZ   = vector<int>( _numSeg ) ;
  
  int i;
  _numZ[0] = _ring;
//   std::cout << "ring = " <<_ring << "numZ 0 = " <<  _numZ[0] << std::endl ;
  for (i = 1; i< _numSeg; ++i)
    {
      int abs_seg_num = int( float(i)/2.0 + .5);
      _numZ[i] = (_ring -(2*(abs_seg_num*_span - int(_span/2)))); 
    }
}

ScannerConf::ScannerConf( const ScannerConf &s)
{
  _numZ.clear() ;
  _numZ.reserve( s._numZ.size() ) ;
  for( size_t i = 0 ; i < s._numZ.size() ; ++i )
    _numZ.push_back( s._numZ[i]) ;
  

//   cout << "Copy constructor :  s._ring = " <<  s._ring << endl ;
  _ring = s._ring;
  _ringDiff = s._ringDiff;
  _span = s._span;
  _numSeg = s._numSeg;
  _numAngles = s._numAngles ;
  _numBins = s._numBins ;
  _zResolution = s._zResolution ;
  _binResolution = s._binResolution ;
//   cout << "Copy constructor :  _ring = " <<  _ring << endl ;
}

bool 
ScannerConf::operator!=(const ScannerConf& s ) const
{
  if( _ring != s._ring )
    return true ;
  if( _ringDiff != s._ringDiff )
    return true ;
  if( _span != s._span)
    return true ;
  if( _numSeg != s._numSeg)
    return true ;
  if( _numAngles != s._numAngles )
    return true ;
  if( _numBins != s._numBins)
    return true ;
  if( _zResolution != s._zResolution )
    return true ;
  if( _binResolution != s._binResolution )
    return true ;
  if( _numZ != s._numZ )
    return true ;
  return false ;
}

ScannerConf& 
ScannerConf::operator=(const ScannerConf& s)
{  
//   cout << "operator = " << endl ;
  if( !(*this != s) )
    return *this ;
  
//   cout << "is different" << endl ;
  _ring = s._ring;
//   cout << "new ring = " << _ring << endl ;
  _ringDiff = s._ringDiff;
  _span = s._span;
  _numSeg = s._numSeg;
  _numAngles = s._numAngles ;
  _numBins = s._numBins ;
  _zResolution = s._zResolution ;
  _binResolution = s._binResolution ;

  //cout << "size = " <<  endl ;
  //cout << s._numZ.size() << endl ;
  _numZ.clear() ;
  //cout << "numz cleared" << endl ;
  _numZ.reserve( s._numZ.size() ) ;
  //cout << "numz reserved" << endl ;
  for( size_t i = 0 ; i < s._numZ.size() ; ++i ){
    //cout << i << " := " << s._numZ[i] << endl ;
    //_numZ.push_back( 0 ) ;
    _numZ.push_back( s._numZ[i]) ;
    //cout << "pushed" << endl ;
  }
  return *this ;
}


ScannerConf::ScannerConf( UnifiedEcatSinoInfo *u)
{
  // Coherency test

  _span              = u->axial_compression;
  _ringDiff          = u->ring_difference;
  switch( u->system_type )
    {
    case 2500:
      _ring = 95;
      break;
    case 962:
      _ring = 63;
      break;
    case 932:
      _ring = 31;
    default:
      break;
    }
  if ( u->num_z_elements[0] )
    {
      _numSeg = 1;
      int i;
      for(i=1;i< 64;++i)
	{
	  if (u->num_z_elements[i]) _numSeg+=2;
	}
    }
  else
    {
      // Mettre un throw
      cerr << "Cannot initialize properly a sinogram" << endl;
    }
  _numZ   = vector<int>(_numSeg);
  int i;
  _numZ[0] = _ring;
//   cout << "num_z_elements[ " << 0 << " ] = " << u->num_z_elements[0] 
//        << " vs _numZ[ " << 0 << " ] = " << _numZ[0] << endl ;
  for(i = 1; i< _numSeg; ++i)
    {
      int abs_seg_num = int( float(i)/2.0 + .5);
      _numZ[i] = (_ring -(2*(abs_seg_num*_span - int(_span/2)))); 
      
    if ( u->num_z_elements[abs_seg_num] != 2*_numZ[i] )
      {
// 	cout << "_ring = " << _ring << " & abs_seg_num " << abs_seg_num << " & _span = " << _span << endl ;
// 	cout << "num_z_elements[ " << abs_seg_num << " ] = " << u->num_z_elements[abs_seg_num] 
// 	     << " vs 2*_numZ[ " << i << " ] = " << 2*_numZ[i] << endl ;
	
	// Mettre un throw
 	cerr << "Cannot initialize properly a sinogram" << endl;
      }
    }
    
  _numAngles = u->num_angles ;
  _numBins = u->num_r_elements ;
  _zResolution = u->z_resolution ;
  _binResolution = u->x_resolution ;  
}

template class AIMSDATA_API Sinogram<int8_t>;
template class AIMSDATA_API Sinogram<int16_t>;
template class AIMSDATA_API Sinogram<int32_t>;
template class AIMSDATA_API Sinogram<float>;
template class AIMSDATA_API Sinogram<double>;

} // namespace aims

// compilation of some Volume classes on Aims types
#include <cartodata/volume/volume_d.h>

template class carto::Volume< AimsData<float> >;
template class carto::Volume< AimsData< AimsData<float> > >;
template class carto::Volume< AimsData< AimsData<double> > >;
template class carto::Volume< AimsData< AimsData<int8_t> > >;
template class carto::Volume< AimsData< AimsData<int16_t> > >;
template class carto::Volume< AimsData< AimsData<int32_t> > >;
