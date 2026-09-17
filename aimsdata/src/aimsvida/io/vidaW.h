#ifndef AIMS_IO_VIDAW_H
#define AIMS_IO_VIDAW_H

#include <string>

template <class T> class AimsData;

namespace aims
{
  class VidaWriter;

  VidaWriter& operator << (VidaWriter& writer, const AimsData<short>& thing);
  VidaWriter& operator << (VidaWriter& writer, const AimsData<float>& thing);

  /** The class for VIDA data write operation.
   */
  class VidaWriter
  {
  public:
    VidaWriter(const std::string& name) : _name(name) { }
    virtual ~VidaWriter() { }

    void write(const AimsData<short>& thing);
    void write(const AimsData<float>& thing);

    friend 
    VidaWriter& operator << (VidaWriter& writer,
			     const AimsData<short>& thing);
    friend 
    VidaWriter& operator << (VidaWriter& writer,
			     const AimsData<float>& thing);

  private:
    std::string   _name;
  };

}

#endif










