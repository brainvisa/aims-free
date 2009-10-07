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
 *  Data item reader class
 */
#ifndef AIMS_IO_ITEMR_H
#define AIMS_IO_ITEMR_H

#include <aims/config/aimsdata_config.h>
#include <string>
#include <iostream>
#include <cartobase/stream/sstream.h>

namespace aims
{

  /**	Low-level "small item" reader, used by higher-level file readers. 
	It can read either a single item, or an array of them. This 
	template class is inherited by specific item readers, esp. by 
	the DefaultItemReader classes.

	This new version differs from the one in Aims 2.7 and before in 
	several aspects:
	- the input stream is not stored in the ItemReader, but passed when 
	  used through the read() functions
	- the reading mode (ascii / binary / byte-swapped binary) is not 
	  passed each time the read() functions are called
	- the new ItemReader is a more specialized object than it was: it can
	  only read one reading mode: ascii / bin / byte-swapped
	- ItemReader objects have the ability to "switch" to another one 
	  depending on the reading mode by allocating a new objetc related to 
	  itself: that is the job of the reader() function. We could have 
	  written a small factory instead and it will maybe be done one day, 
	  but for now I was just too lazy to do it.

	As a result the new ItemReader is a lot faster than in Aims 2.7 since 
	it doesn't have to check the reading mode and switch to different 
	operating modes at each specific read.\n
	As a counterpart, you now generally have to allocate two readers: one 
	to use the correct item variant, and another one to switch on the 
	reading-mode specific ItemReader:

	\code
	#include <aims/io/defaultItemR.h>
	// Let's read a vector of signed 16 bits elements
	short				shortarray[ 100 ];
	DefaultItemReader<short>	ir1;
	string				openmode;	// "ascii" / "binar"
	bool				bswap;

	// get openmode and bswap from the header of the file to read
	// ...
	// now we assume it's done

	ItemReader<short>		*ir = ir1.reader( openmode, bswap );
	// we use ir in the following, we can forget ir1 now.

	std::ifstream	infile( "file.dat" );
	assert( infile );
	// read 100 elements
	ir->read( infile, shortarray, 100 );
	// don't forget to destroy the reader after use
	delete ir;
	\endcode
  */
  template<class T>
  class AIMSDATA_API ItemReader
  {
  public:
    virtual ~ItemReader() {}

    virtual void read( std::istream & is, T & item ) const
      { read( is, &item, 1 ); }
    virtual void read( std::istream & is, T* pitem, size_t n ) const = 0;
    void read( const std::string &istring, T &item ) const
      { read( istring, &item, 1 ); }
    void read( const std::string &istring, T *pitem, size_t n ) const;
    virtual ItemReader<T>* reader( const std::string & openmode = "binar", 
				   bool bswap = false ) const = 0;
  };

  template <class T>
  inline 
  void ItemReader<T>::read( const std::string &istring, 
			    T *pitem, size_t n ) const
  {
    std::istringstream is( istring.c_str() );
    read( is, pitem, n );
  }
}


#endif

