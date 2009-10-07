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

#ifndef CARTOBASE_IO_WRITER_H
#define CARTOBASE_IO_WRITER_H

#include <cartobase/object/object.h>


namespace carto
{
  class DataSource;

  class GenericWriter
  {
  public:
    GenericWriter();
    GenericWriter( rc_ptr<DataSource> ds );
    GenericWriter( const std::string & filename );
    GenericWriter( std::ostream & stream );
    virtual ~GenericWriter();

    /*!	\brief Finds the correct format and writes the object

    Options can be passed to specify some writing parameters. The most 
    important is the format (\c "format" property): if it is specified, this 
    format is tried first, so you can use it to force the format, otherwise it 
    will be determined from the filename extension (if available). If no 
    extension and no format are given, the first working format will be used. 
    */
    template <typename T>
    bool write( const T & obj, Object options = none() );

    virtual std::string writtenObjectType() const = 0;

    const rc_ptr<DataSource> dataSource() const;
    rc_ptr<DataSource> dataSource();
    void attach( rc_ptr<DataSource> ds );
    void attach( const std::string & filename );
    void attach( std::ostream & stream );

    /// flush the writing DataSource (if still open)
    void flush();
    /// close the writing DataSource
    void close();
    
  protected:
    rc_ptr<DataSource>	_datasource;
  };


  /**	Generic writer for *every* format of Aims object. 
	The Writer classes are built on the same model as the Reader classes
	and share the same FormatDictionary with them to store all 
	known file formats.

	\see FormatDictionary Reader
  */
  template<class T> class Writer : public GenericWriter
  {
  public:
    inline Writer() {};
    inline Writer( rc_ptr<DataSource> ds ) :
      GenericWriter( ds ) {}
    inline Writer( const std::string& filename ) :
      GenericWriter( filename ) {}
    inline Writer( std::ostream & stream ) :
      GenericWriter( stream ) {}
    virtual ~Writer() {}

    /*!	\brief Finds the correct format and writes the object
 
    Options can be passed to specify some writing parameters. The most 
    important is the format (\c "format" property): if it is specified, this 
    format is tried first, so you can use it to force the format, otherwise it 
    will be determined from the filename extension (if available). If no 
    extension and no format are given, the first working format will be used. 
    */
    virtual bool write( const T & obj, Object options = none() );

    virtual std::string writtenObjectType() const;
  };

}


template <class T>
inline carto::GenericWriter &
operator << ( carto::GenericWriter & writer, const T & thing )
{
  writer.write( thing );
  return writer;
}


template <class T>
inline carto::Writer<T> &
operator << ( carto::Writer<T> & writer, const T & thing )
{
  writer.write( thing );
  return writer;
}


#endif

