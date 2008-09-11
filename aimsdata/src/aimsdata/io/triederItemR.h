/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Trieder item reader class
 */
#ifndef AIMS_IO_TRIEDERITEMR_H
#define AIMS_IO_TRIEDERITEMR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/math/trieder.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemReader<Trieder> : public ItemReader<Trieder>
  {
  public:
    DefaultItemReader() : ItemReader<Trieder>() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
    virtual void read( std::istream & is, Trieder * pitem, size_t n ) const;
    virtual ItemReader<Trieder>* 
      reader( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemReader<Trieder> 
    : public DefaultItemReader<Trieder>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<Trieder>() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemReader<Trieder> 
    : public DefaultItemReader<Trieder>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<Trieder>() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}


#endif
