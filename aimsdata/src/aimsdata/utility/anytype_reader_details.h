#ifndef AIMS_ANYTYPE_READER_DETAILS_H
#define AIMS_ANYTYPE_READER_DETAILS_H

/// \file Belongs to anytype_reader package
/// Do not include directly, include "aims/utility/anytype_reader.h" instead

// includes from STL
#include <limits>

// includes from AIMS
#include "aims/io/finder.h"
#include "aims/utility/converter_volume.h"

namespace aims
{

  namespace
  {
    // TODO: should add traits to the whole scheme to call the convertor. Also to get some error
    // checking from converter -- right now, it always returns true.
    template < typename TCaller, typename TFile >
    bool anytype_reader(Process & p, const std::string & filein, Finder & f)
    {
      typedef typename TCaller::return_type                                               return_type;
      typedef typename TCaller::convertor_type::template Type<TFile, return_type>::type   convertor_type;

      // restore true caller type
      TCaller & atr = dynamic_cast<TCaller &>(p);

      // read data with original type
      TFile data;
      Reader<TFile> r(filein);
      std::string format = f.format();
      r.read(data, 0, &format);

      // converting to desired type
      convertor_type convert;
      convert.convert(data, atr.get());

      return true;
    }
  }

  //-----------------------------------------------------------------------------------

  
  template < typename T, typename TConvertorFamily >
  AnyTypeReaderProcess<T, TConvertorFamily>::AnyTypeReaderProcess(const T & init)
    : m_res(init)
  {
    this->registerProcesses();
  }


  template < typename T, typename TConvertorFamily >
  void AnyTypeReaderProcess<T, TConvertorFamily>::registerProcesses()
  {
    typedef AnyTypeReaderProcess<T, TConvertorFamily> Self;

    // TODO: other file types can be added for other needs
    registerProcessType("Volume", "S8",     &anytype_reader<Self, AimsData< int8_t   > >);
    registerProcessType("Volume", "U8",     &anytype_reader<Self, AimsData< uint8_t  > >);
    registerProcessType("Volume", "S16",    &anytype_reader<Self, AimsData< int16_t  > >);
    registerProcessType("Volume", "U16",    &anytype_reader<Self, AimsData< uint16_t > >);
    registerProcessType("Volume", "S32",    &anytype_reader<Self, AimsData< int32_t  > >);
    registerProcessType("Volume", "U32",    &anytype_reader<Self, AimsData< uint32_t > >);
    registerProcessType("Volume", "FLOAT",  &anytype_reader<Self, AimsData< float    > >);
    registerProcessType("Volume", "DOUBLE", &anytype_reader<Self, AimsData< double   > >);
  }


  //-----------------------------------------------------------------------------------

  template < typename T, typename TConvertorFamily >
  AnyTypeReader<T, TConvertorFamily>::AnyTypeReader(std::string filename)
    : Base((T()))
    , m_filename(filename)
  {}


  template < typename T, typename TConvertorFamily >
  void AnyTypeReader<T, TConvertorFamily>::read(T & out)
  {
    this->execute(m_filename);
    out = this->get();
  }



} // namespace aims

#endif

