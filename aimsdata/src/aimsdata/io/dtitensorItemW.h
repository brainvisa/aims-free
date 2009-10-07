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
 *  DtiTensor item writer class
 */
#ifndef AIMS_IO_DTITENSORITEMW_H
#define AIMS_IO_DTITENSORITEMW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/math/dtitensor.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemWriter<DtiTensor> 
    : public ItemWriter<DtiTensor>
  {
  public:
    DefaultItemWriter() : ItemWriter<DtiTensor>() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
    virtual void write( std::ostream & is, const DtiTensor * pitem, 
			size_t n ) const;
    virtual ItemWriter<DtiTensor>* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemWriter<DtiTensor> 
    : public DefaultItemWriter<DtiTensor>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<DtiTensor>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemWriter<DtiTensor> 
    : public DefaultItemWriter<DtiTensor>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<DtiTensor>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}	// namespace aims


#endif
