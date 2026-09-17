#ifndef AIMS_IO_VIDAR_H
#define AIMS_IO_VIDAR_H


#include <aims/io/vidaheader.h>
#include <aims/data/data.h>
#include <soma-io/allocator/mappingcopy.h>

template <class T> class AimsData;


namespace aims
{
  template < class T > class VidaReader;

  template < class T > VidaReader< T >&
  operator >> ( VidaReader< T >& reader, AimsData< T >& thing);


  /** The class for VIDA data read operation.
   */
  template < class T >
  class VidaReader
  {
  public:
    VidaReader(const std::string& name)
      : _name(name)
    { }
    virtual ~VidaReader() { }

    /** Read the data with "name" file name from disk
        @param thing reference to the data to read
    */
    void read(AimsData< T >& thing, const carto::AllocatorContext & context, 
              carto::Object options );
    
    void frameRead(AimsData< T >& thing, 
                   const carto::AllocatorContext & context, int frame, 
                   int border = 0 ); 

    carto::AllocatorContext setupMemMode( const VidaHeader & hdr, 
                                          const carto::AllocatorContext & c, 
                                          int border );

    /// Input stream operator
    friend
    VidaReader<T>& operator >> <>( VidaReader<T>& reader,
				   AimsData<T>& thing);
  private:
    std::string					_name;
  };

  template< class T > inline
  VidaReader< T >&
  operator >> ( VidaReader< T >& reader, AimsData< T >& thing )
  {
    reader.read( thing );
    return( reader );
  }

}

#endif
