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
 *  Data reader class
 */
#ifndef AIMS_IO_WRITER_H
#define AIMS_IO_WRITER_H

#include <string>


namespace aims
{

  class GenericWriter
  {
  public:
    inline GenericWriter() {};
    inline GenericWriter( const std::string& filename ) :
      _filename( filename ) {}
    virtual ~GenericWriter() {}

    /*!	\brief Finds the correct format and writes the object
 
    If \c format is specified, this format is tried first, so you can use it
    to force the format, otherwise it will be determined from the filename 
    extension. If no extension and no format are given, the first working 
    format will be used. */
    template <typename T>
    bool write( const T & obj, bool ascii = false, 
		const std::string* format = 0 );

    /// set output file name
    inline void setFileName( const std::string &fileName )
    {
      _filename = fileName;
    }
    /// get output file name
    inline const std::string &fileName() const { return _filename; }

    virtual std::string writtenObjectType() const = 0;
    virtual std::string writtenObjectDataType() const = 0;
    virtual std::string writtenObjectFullType() const = 0;
    
  protected:
    std::string _filename;

    
  };

  /**	Generic writer for *every* format of Aims object. 
	The Writer classes are built on the same model as the Reader classes
	and share the same FileFormatDictionary with them to store all 
	known file formats.

	\see FileFormatDictionary Reader
  */
  template<class T> class Writer : public GenericWriter
  {
  public:
    inline Writer() {};
    inline Writer( const std::string& filename ) :
      GenericWriter( filename ) {}
    virtual ~Writer() {}

    /*!	\brief Finds the correct format and writes the object
 
    If \c format is specified, this format is tried first, so you can use it
    to force the format, otherwise it will be determined from the filename 
    extension. If no extension and no format are given, the first working 
    format will be used. */
    virtual bool write( const T & obj, bool ascii = false, 
			const std::string* format = 0 );

    virtual std::string writtenObjectType() const;
    virtual std::string writtenObjectDataType() const;
    virtual std::string writtenObjectFullType() const;
  };

}


template <class T>
inline aims::GenericWriter &
operator << ( aims::GenericWriter & writer, const T & thing )
{
  writer.write( thing );
  return writer;
}


template <class T>
inline aims::Writer<T> &
operator << ( aims::Writer<T> & writer, const T & thing )
{
  writer.write( thing );
  return writer;
}


#endif


