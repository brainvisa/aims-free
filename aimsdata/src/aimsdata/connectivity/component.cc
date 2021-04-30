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

/*
 *  Connected components
 */
#include <aims/connectivity/component_d.h>

using aims::Connectivity;

using namespace carto;
using namespace std;

namespace aims
{
  
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<uint8_t>, VolumeRef<double> >;
  
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<int8_t>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<uint16_t>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<int16_t>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<uint32_t>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<int32_t>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<float>, VolumeRef<double> >;

  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<uint8_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<int8_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<uint16_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<int16_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<uint32_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<int32_t> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<float> >;
  template class ConnectedComponentEngine<VolumeRef<double>, VolumeRef<double> >;

  template void 
  AimsConnectedComponent( BucketMap<Void>&, Connectivity::Type, 
                          const Void &, bool, size_t, size_t, size_t, bool );
  template void 
  AimsConnectedComponent( BucketMap<uint8_t>&, Connectivity::Type, 
                          const uint8_t &, bool, size_t, size_t,size_t,  bool );
  template void 
  AimsConnectedComponent( BucketMap<int16_t>&, Connectivity::Type, 
                          const int16_t &, bool, size_t, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<uint16_t>&, Connectivity::Type,
                          const uint16_t &, bool, size_t, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<int32_t>&, Connectivity::Type,
                          const int32_t &, bool, size_t, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<uint32_t>&, Connectivity::Type,
                          const uint32_t &, bool, size_t, size_t, size_t, bool );
  template void 
  AimsConnectedComponent( BucketMap<float>&, Connectivity::Type, 
                          const float &, bool, size_t, size_t, size_t, bool );

  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const VolumeRef<uint8_t> &,
                          Connectivity::Type, const uint8_t &, bool, size_t, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const VolumeRef<int16_t> &,
                          Connectivity::Type, const int16_t &, bool, size_t, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const VolumeRef<float> &,
                          Connectivity::Type, const float &, bool, size_t, size_t, 
                          size_t, bool );


  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<Void> &,
                          Connectivity::Type, const Void &, bool, size_t, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<uint8_t> &,
                          Connectivity::Type, const uint8_t &, bool, size_t, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<int16_t> &,
                          Connectivity::Type, const int16_t &, bool, size_t, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<float> &,
                          Connectivity::Type, const float &, bool, size_t, size_t, 
                          size_t, bool );
                          
  template VolumeRef<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const VolumeRef<uint8_t> &,
                                 Connectivity::Type, const uint8_t &, bool, size_t, size_t, 
                                 size_t, bool );
  template VolumeRef<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const VolumeRef<int16_t> &,
                                 Connectivity::Type, const int16_t &, bool, size_t, size_t, 
                                 size_t, bool );
  template VolumeRef<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const VolumeRef<float> &,
                                 Connectivity::Type, const float &, bool, size_t, size_t, 
                                 size_t, bool );  
}
