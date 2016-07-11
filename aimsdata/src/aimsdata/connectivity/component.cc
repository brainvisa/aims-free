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

using namespace std;

namespace aims
{
  
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<uint8_t>, AimsData<double> >;
  
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<int8_t>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<uint16_t>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<int16_t>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<uint32_t>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<int32_t>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<float>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<float>, AimsData<double> >;

  template class ConnectedComponentEngine<AimsData<double>, AimsData<uint8_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<int8_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<uint16_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<int16_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<uint32_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<int32_t> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<float> >;
  template class ConnectedComponentEngine<AimsData<double>, AimsData<double> >;

  template void 
  AimsConnectedComponent( BucketMap<Void>&, Connectivity::Type, 
                          const Void &, bool, size_t, size_t, bool );
  template void 
  AimsConnectedComponent( BucketMap<uint8_t>&, Connectivity::Type, 
                          const uint8_t &, bool, size_t, size_t, bool );
  template void 
  AimsConnectedComponent( BucketMap<int16_t>&, Connectivity::Type, 
                          const int16_t &, bool, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<uint16_t>&, Connectivity::Type,
                          const uint16_t &, bool, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<int32_t>&, Connectivity::Type,
                          const int32_t &, bool, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<uint32_t>&, Connectivity::Type,
                          const uint32_t &, bool, size_t, size_t, bool );
  template void
  AimsConnectedComponent( BucketMap<float>&, Connectivity::Type, 
                          const float &, bool, size_t, size_t, bool );

  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const AimsData<uint8_t> &,
                          Connectivity::Type, const uint8_t &, bool, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const AimsData<int16_t> &,
                          Connectivity::Type, const int16_t &, bool, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const AimsData<float> &,
                          Connectivity::Type, const float &, bool, size_t, 
                          size_t, bool );


  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<Void> &,
                          Connectivity::Type, const Void &, bool, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<uint8_t> &,
                          Connectivity::Type, const uint8_t &, bool, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<int16_t> &,
                          Connectivity::Type, const int16_t &, bool, size_t, 
                          size_t, bool );
  template void 
  AimsConnectedComponent( AimsBucket<Void> &, const BucketMap<float> &,
                          Connectivity::Type, const float &, bool, size_t, 
                          size_t, bool );
                          
  template AimsData<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const AimsData<uint8_t> &,
                                 Connectivity::Type, const uint8_t &, bool, size_t, 
                                 size_t, bool );
  template AimsData<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const AimsData<int16_t> &,
                                 Connectivity::Type, const int16_t &, bool, size_t, 
                                 size_t, bool );
  template AimsData<int16_t>
  AimsLabeledConnectedComponent( AimsBucket<Void> &, const AimsData<float> &,
                                 Connectivity::Type, const float &, bool, size_t, 
                                 size_t, bool );  
}
