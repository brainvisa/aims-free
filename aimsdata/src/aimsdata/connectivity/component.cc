// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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
