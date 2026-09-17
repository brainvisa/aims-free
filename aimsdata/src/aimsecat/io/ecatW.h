#ifndef AIMS_IO_ECATW_H
#define AIMS_IO_ECATW_H

#include <string>

template <class T> class AimsData;

namespace aims
{

  class EcatWriter;

  EcatWriter& operator << ( EcatWriter& writer, const AimsData<short>& thing);
  EcatWriter& operator << ( EcatWriter& writer, const AimsData<float>& thing);

  /** The class for VIDA data write operation.
   */
  class EcatWriter
  {
  public:
    EcatWriter(const std::string& name) : _name(name) { }
    virtual ~EcatWriter() { }

    /** Write the data to disk with "name" file name
        @param thing const reference to the data to write
    */
    void write(const AimsData<short>& thing);
    void write(const AimsData<float>& thing);

    /// Return a name without .vimg, .vhdr or .vinfo extension
    std::string removeExtension(const std::string& name);

  private:
    std::string   _name;
  };

}

#endif

