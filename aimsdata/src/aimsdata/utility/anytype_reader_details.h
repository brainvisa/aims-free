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

