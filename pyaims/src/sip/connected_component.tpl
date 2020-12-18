
%ModuleHeaderCode
#include <aims/connectivity/component.h>
%End

namespace aims
{

/*
  class ConnectedComponentEngine_%Template1typecode%_%Template2typecode%
  {

    public :
      static void filterInFrame( const AimsData<T>& cc,
                                 AimsData<O>& out,
                                 std::map<O, size_t>& valids,
                                 int t,
                                 bool verbose = true );

      static void connectedInFrame( const AimsData<T>& data,
                                    AimsData<O>& out,
                                    Connectivity::Type connectivity,
                                    std::multimap<size_t, O>& compSizes,
                                    int t,
                                    const T & backgrnd = 0, bool bin = true,
                                    bool verbose = true );

      static void connected( const AimsData<T>& data,
                             AimsData<O>& out,
                             aims::Connectivity::Type connectivity,
                             std::map<O, size_t>& valids,
                             const T & backgrnd = 0, bool bin = true,
                             size_t minSize = 0, size_t maxSize = 0,
                             size_t numMax = 0, bool verbose = true );


      static void connected( const AimsData<T>& data,
                             AimsData<O>& out,
                             aims::Connectivity::Type connectivity,
                             const T & backgrnd = 0, bool bin = true,
                             size_t minSize = 0, size_t maxSize = 0,
                             size_t numMax = 0, bool verbose = true );

    private:
      ConnectedComponentEngine();
  };
*/

%#if SIP_VERSION < 0x04130e%
  void AimsConnectedComponent( AimsData_%Template1typecode% & data,
                               aims::Connectivity::Type connectivity,
                               std::map<%Template1%, size_t>& valids,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minSize = 0, size_t maxSize = 0,
                               size_t numMax = 0, bool verbose = true );
%Docstring
  void AimsConnectedComponent(data, connectivity, valids, backgrnd=0,
                              bin=True, minSize=0, maxSize=0, 
                              numMax=0, verbose=True)

  Connected components extraction.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: AimsData_%Template1typecode% instance
      data will be modified during the process.
  connectivity: Connectivity.Type enum code
  valids: dict %Template1%: size_t
  backgrnd: %Template1%
  bin: bool (optional)
  minsize: size_t (optional)
  maxsize: size_t (optional)
  numMax: size_t (optional)
  verbose: bool (optional)


%End
%#else%
  void AimsConnectedComponent( AimsData_%Template1typecode% & data,
                               aims::Connectivity::Type connectivity,
                               map_%Template1typecode%_SIZE_T & valids,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minSize = 0, size_t numMax = 0,
                               bool verbose = true );
%Docstring
  void AimsConnectedComponent(data, connectivity, valids, backgrnd=0,
                              bin=True, minSize=0, numMax=0, verbose=True)

  Connected components extraction.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: AimsData_%Template1typecode% instance
      data will be modified during the process.
  connectivity: Connectivity.Type enum code
  valids: dict %Template1%: size_t
  backgrnd: %Template1%
  bin: bool (optional)
  minsize: size_t (optional)
  numMax: size_t (optional)
  verbose: bool (optional)


%End
%#endif%

  void AimsConnectedComponent( AimsData_%Template1typecode% & data,
                               aims::Connectivity::Type connectivity,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minSize = 0, size_t maxSize = 0, 
                               size_t numMax = 0, bool verbose = true );
%Docstring
  void AimsConnectedComponent(data, connectivity, backgrnd=0, bin=True,
                              minSize=0, maxSize=0, numMax=0, verbose=True)

  Connected components extraction.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: AimsData_%Template1typecode% instance
      data will be modified during the process.
  connectivity: Connectivity.Type enum code
  backgrnd: %Template1%
  bin: bool (optional)
  minsize: size_t (optional)
  maxsize: size_t (optional)
  numMax: size_t (optional)
  verbose: bool (optional)


%End

  void AimsConnectedComponent( BucketMap_%Template1typecode% & data,
                               aims::Connectivity::Type connectivity,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minSize = 0, size_t maxSize = 0, 
                               size_t numMax = 0, bool verbose = true );
%Docstring
  void AimsConnectedComponent(data, connectivity, backgrnd=0, bin=True,
                              minSize=0, maxSize=0, numMax=0, verbose=True)

  Connected components extraction, Bucket version.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: BucketMap_%Template1typecode% instance
      data will be modified during the process.
  connectivity: Connectivity.Type enum code
  backgrnd: %Template1%
  bin: bool (optional)
  minsize: size_t (optional)
  maxsize: size_t (optional)
  numMax: size_t (optional)
  verbose: bool (optional)


%End


/*
  void AimsConnectedComponent( AimsBucket_%Template1typecode% & component,
                               const AimsData_%Template1typecode%& data,
                               aims::Connectivity::Type connectivity,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minsize = 0, size_t maxsize = 0, 
                               size_t maxcomp = 0, bool verbose = true );

  void AimsConnectedComponent( AimsBucket_VOID & component,
                               const BucketMap_%Template1typecode% & data,
                               Connectivity::Type connectivity,
                               const %Template1% & backgrnd = 0,
                               bool bin = true,
                               size_t minsize = 0, size_t maxsize = 0, 
                               size_t maxcomp = 0, bool verbose = true );

  AimsData_S16 AimsLabeledConnectedComponent(
    AimsBucket_VOID & component,
    const AimsData_%Template1typecode% & data,
    aims::Connectivity::Type connectivity,
    const %Template1% & backgrnd = 0,
    bool bin = true,
    size_t minsize = 0,
    size_t maxsize = 0,
    size_t maxcomp = 0,
    bool verbose = true );
*/
};

