/*
 *  SPM reader class
 */
#ifndef AIMS_IO_SPMR_H
#define AIMS_IO_SPMR_H

#include <aims/memmap/allocator.h>
#include <cartobase/object/object.h>
#include <fstream>
#include <string>


template <typename T> class AimsData;

namespace aims
{
class AffineTransformation3d;
}
typedef aims::AffineTransformation3d Motion;

namespace aims 
{

  template <typename T> class SpmReader;
  class SpmHeader;


  template <class T>
  SpmReader<T>&
  operator >> (SpmReader<T>& reader,
	       AimsData<T>& thing);


  /** The template class for SPM read operation.
      The template argument gives the type of the elements stored
      in the container.
  */
  template <typename T>
  class SpmReader
  {
  public:
    SpmReader(const std::string& name) 
      : _name( name ) { }
    virtual ~SpmReader() { }
  
    /** Read the data with "name" file name from disk
	@param thing reference to the data to read
	@param border border width (useful for image processing)
    */
    void read(AimsData<T>& thing, const carto::AllocatorContext & context, 
              carto::Object options );
  
    /// Return a name without .hdr or .img extension
    std::string removeExtension(const std::string& name);
  
    /// Input stream operator
    friend
    SpmReader<T>& 
    operator >> <> (SpmReader<T>& reader,
		    AimsData<T>& thing);
  
  private:
    void readFrame( std::ifstream &, AimsData<T> & data, int t, 
                    const std::string & type, SpmHeader* hdr, bool bswap, 
                    const Motion & storageToMemory );
    template<typename U> 
    void readScaledFrame( std::ifstream &, AimsData<T> & data, int t, 
                          SpmHeader* hdr, bool bswap,
                          const Motion & storageToMemory );
    std::string	_name;
  };


  template <typename T>
  inline
  SpmReader<T>&
  operator >> (SpmReader<T>& reader,
	       AimsData<T>& thing)
  {
    reader.read( thing );
    return reader;
  }

}

#endif
