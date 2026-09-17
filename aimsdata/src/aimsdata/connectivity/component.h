#ifndef AIMS_CONNECTIVITY_COMPONENT_H
#define AIMS_CONNECTIVITY_COMPONENT_H


#include <aims/config/aimsdata_config.h>
#include <aims/connectivity/connectivity.h>
#include <cartodata/volume/volume.h>

template <typename T> class AimsBucket;
class Void;


namespace aims
{
  
  template <typename T> class BucketMap;
  
  template <typename T, typename O>
  class ConnectedComponentEngine
  {

    private :
      ConnectedComponentEngine();
  };
  
  // Specialization
  template <typename T, typename O>
  class ConnectedComponentEngine<carto::VolumeRef<T>, carto::VolumeRef<O> >
  {

    public :
      static void connected( const carto::VolumeRef<T>& data,
                             carto::VolumeRef<O> out,
                             aims::Connectivity::Type connectivity,
                             std::map<O, size_t>& valids,
                             const T & backgrnd = 0, bool bin = true,
                             size_t minSize = 0, size_t maxSize = 0,
                             size_t numMax = 0,
                             bool verbose = true );


      static void connected( const carto::VolumeRef<T>& data,
                             carto::VolumeRef<O> out,
                             aims::Connectivity::Type connectivity,
                             const T & backgrnd = 0, bool bin = true,
                             size_t minSize = 0, size_t maxSize = 0,
                             size_t numMax = 0,
                             bool verbose = true );

      static void filterInFrame( const carto::VolumeRef<T>& cc,
                                 carto::VolumeRef<O> out,
                                 std::map<O, size_t>& valids,
                                 int t,
                                 bool verbose = true );
      static void connectedInFrame( const carto::VolumeRef<T>& data,
                                    carto::VolumeRef<O> out,
                                    Connectivity::Type connectivity,
                                    std::multimap<size_t, O>& compSizes,
                                    int t,
                                    const T & backgrnd = 0, bool bin = true,
                                    bool verbose = true );

  private:
      ConnectedComponentEngine();

  };

  // inline

  template<typename T, typename O>
  inline
  void ConnectedComponentEngine<carto::VolumeRef<T>,
                                carto::VolumeRef<O> >::connected(
    const carto::VolumeRef<T>& data,
    carto::VolumeRef<O> out,
    aims::Connectivity::Type connectivity,
    const T & backgrnd, bool bin,
    size_t minSize, size_t maxSize, size_t numMax,
    bool verbose )
  {
    std::map<O, size_t> valids;
    connected(data,
              out,
              connectivity,
              valids,
              backgrnd, bin, minSize, maxSize,
              numMax, verbose);
  }

  template<typename T>
  inline
  void AimsConnectedComponent( carto::VolumeRef<T> data,
                               aims::Connectivity::Type connectivity,
                               std::map<T, size_t>& valids, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t maxSize = 0,
                               size_t numMax = 0,
                               bool verbose = true )
  {
    ConnectedComponentEngine<carto::VolumeRef<T>,
                             carto::VolumeRef<T> >::connected(
      data,
      data,
      connectivity,
      valids,
      backgrnd, bin, minSize, maxSize,
      numMax, verbose);
  }

  template<typename T>
  inline
  void AimsConnectedComponent( carto::VolumeRef<T> data,
                               aims::Connectivity::Type connectivity,
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t maxSize = 0,
                               size_t numMax = 0,
                               bool verbose = true )
  {
      std::map<T, size_t> valids;
    
      ConnectedComponentEngine<carto::VolumeRef<T>,
                               carto::VolumeRef<T> >::connected(
        data,
        data,
        connectivity,
        valids,
        backgrnd, bin, minSize, maxSize,
        numMax, verbose);
  }

  template<typename T>
  void AimsConnectedComponent( BucketMap<T>& data,
                               aims::Connectivity::Type connectivity,
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t maxSize = 0,
                               size_t numMax = 0,
                               bool verbose = true );

  template <typename T>
  void AimsConnectedComponent( AimsBucket<Void>& component,
                               const carto::VolumeRef<T>& data,
                               aims::Connectivity::Type connectivity, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minsize = 0, size_t maxSize = 0,
                               size_t maxcomp = 0,
                               bool verbose = true );
    
  template <typename T>
  void AimsConnectedComponent( AimsBucket<Void>& component,
                               const BucketMap<T>& data,
                               Connectivity::Type connectivity, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minsize = 0, size_t maxSize = 0,
                               size_t maxcomp = 0,
                               bool verbose = true );
                               
  template <typename T>
  carto::VolumeRef<int16_t> AimsLabeledConnectedComponent(
    AimsBucket<Void>& component,
    const carto::VolumeRef<T>& data,
    aims::Connectivity::Type connectivity,
    const T & backgrnd = 0, bool bin = true,
    size_t minsize = 0, size_t maxSize = 0, size_t maxcomp = 0,
    bool verbose = true );
                                                   
}

#endif
