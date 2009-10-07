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

#ifndef CARTOBASE_IO_DATASOURCEINFO_H
#define CARTOBASE_IO_DATASOURCEINFO_H

#include <cartobase/object/syntax.h>
#include <cartobase/object/object.h>
#include <vector>
#include <string>

namespace carto
{
  class FormatChecker;
  class DataSource;

  /*! Generic information retreiver / checker for all data sources 
    and file formats

    This will replace the aims::Finder (of aims).

    DataSourceInfo provides a plug-in system for new formats and data 
    types. Formats are hidden in the plugins and should never be 
    accessed directly.

    Usage: check() the DataSource and then process according to the 
    object type ("Volume of S16", "Mesh", "Texture", "Bucket", ...)\n
    Use the generic Reader once the data type is known, the right format 
    will be selected automatically.

    Avoiding manually switching on objects and data types is possible  
    using the Process interface and presumably, template functions.

    Here is an example of how to use the DataSourceInfo class:
    (\b warning: some objects used in this example code do not exist yet)

    \code
    #include <cartobase/wip/io/datasourceinfo.h>
    #include <cartobase/wip/io/reader.h>
    #include <cartodata/volume/volume.h>

    using namespace carto;
    using namespace std;

    FileDataSource ds( "toto.ima" );
    DataSourceInfo f;
    Object info = f.check( ds );
    if( !info.get() )
      cerr << "could not load " << ds.filename() << endl;
    else
      {
        string object_type = info->getProperty( "object_type" )->getString();
        if( object_type == DataTypeCode<Volume<int16_t> >::name() )
          {
            Reader<Volume<short> >	vr( &ds, info );
            Volume<short>	vol;
            if( vr.read( vol ) )
              cout << "volume read\n";
            else
              cerr << "Error loading " << fname << endl;
          }
        else if( object_type == DataTypeCode<Mesh>::name() )
          {
            // ...
          }
        else
          cerr << ds.filaname() << " is of type " << object_type 
            << " which is not handled" << endl;
      }
    \endcode

    \see Reader Process
  */
  class DataSourceInfo
  {
  public:
    enum State
      {
	Unchecked, 
	Ok, 
	Error, 
      };

    DataSourceInfo();
    ~DataSourceInfo();

    static void registerFormat( const std::string & fmtid, 
				FormatChecker* format, 
				const std::vector<std::string> & extensions );
    ///	Finds the FormatChecker associated with the given string ID
    static FormatChecker* formatInfo( const std::string & format );
    /** Determines object information associated to the DataSource

    The data source is analyzed and a header is built and returned. 
    In case of failure, a none object is returned, and an error status and 
    message is kept.
    If successful, the header may contain arbitrary data (depending on the 
    specific format), and some "normalized" properties:
    - "object_type" (string) is the DataTypeCode name of the object to be read
    - "possible_types" (vector<string>), if present, is a list of codes of 
      different possible object representations for the contents of the data 
      source
    - "format" (string) if the format name of the low-level reader to use
    */
    Object check( DataSource & ds );
    State state() const;
    std::string errorMessage() const;
    /// creates and throws an exception corresponding to the error state
    void launchException();
    /** .minf meta-header reading.
        Normally the \c .minf contents is returned as a new Object. But if 
        \c base is provided, the \c .minf contents is appended to the \c base 
        object (and also returned).\n
        If the DataSource represents a file which doesn't end with \c ".minf", 
        then another file is read instead, with the \c ".minf" appended.
     */
    static Object readMinf( DataSource & ds, Object base = none() );
    /// internal use
    static SyntaxSet & minfSyntax();

  private:
    struct Private;
    Private	*d;
  };

}

#endif

