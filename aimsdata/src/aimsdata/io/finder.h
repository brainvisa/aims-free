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
 *  Finder class
 */
#ifndef AIMS_IO_FINDER_H
#define AIMS_IO_FINDER_H

#include <aims/config/aimsdata_config.h>
#include <cartobase/object/object.h>
#include <memory>


namespace aims
{
  class Finder;
  struct Finder_Private;
  class Header;
  class FinderFormat;


  /*!	\class Finder finder.h aims/io/finder.h
	\brief	Generic finder / checker for all data objects and file formats
        This will replace the old AimsFinder.

        \b Warning: Finder will be replaced by carto::DataSourceInfo in 
        cartobase library. Avoid using it in new code.

	The Finder provides a plug-in system for new formats and data 
	types. Formats are hidden in the plugins and should never be 
	accessed directly.

	Usage: check() the file and then process according to the 
	object type ("Volume", "Mesh", "Texture", "Bucket", ...) and 
	its data type (if any) ("S16" : short, "U8": unsigned byte, 
	"FLOAT", "DOUBLE", ...) \n
	Use the generic Reader once the data type is known, the right format 
	will be selected automatically.

	Avoiding manually switching on objects and data types is now possible  
	using the new Process interface and presumably, template functions.

	See the \link AimsFileInfo/fileInfo.cc AimsFileInfo \endlink command 
	for an example of how to use it. 

	Here is an example of using the Finder class:

	\code
	#include <aims/io/finder.h>
	#include <aims/io/reader.h>
	#include <aims/data/data.h>

	string fname = "toto.ima";
	Finder f;
	if( ! f.check( fname ) )
	  cerr << "could not load " << fname << endl;
	else
	  {
	    if( f.objectType() == "Volume" )
	      {
	        if( f.dataType() == "S16" )
		  {
		    Reader<AimsData<short> >	vr( fname );
		    AimsData<short>	vol;
		    if( vr.read( vol )
		      cout << "volume read\n";
		    else
		      cerr << "Error loading " << fname << endl;
		  }
		else
		  {
		    cerr << fname << " is not a volume of short\n";
		  }
	      }
	    else if( f.objectType() == "Mesh" )
	      {
	        // ...
	      }
	    else
	      cerr << fname << " is not a volume or a mesh\n";
	  }
	\endcode

	\see Reader Process
  */
  class AIMSDATA_API Finder
  {
  public:
    enum State
      {
	Unchecked, 
	Ok, 
	Error, 
      };

    Finder();
    ~Finder();

    static void registerFormat( const std::string & fmtid, 
				FinderFormat* format, 
				const std::vector<std::string> & extensions,
                                const std::string & before = "" );
    ///	Finds the FinderFormat associated with the given string ID
    static FinderFormat* finderFormat( const std::string & format );
    /** Determines object associated to file name and returns true if success.
	If successful, appropriate infos are stored into the Finder, including 
	the full format header */
    bool check( const std::string & filename );
    std::string objectType() const { return( _objectType ); }
    void setObjectType( const std::string & obj ) { _objectType = obj; }
    std::string dataType() const { return( _dataType ); }
    void setDataType( const std::string & dat ) { _dataType = dat; }
    /*! Some formats (like VIDA and ECAT) can be read as different data types 
      for the same file (for ex. if using shorts resampled for each slice). 
      By asking all the possible data types you can choose the one you prefer 
      for your application. Possible types may be sorted by preference. */
    std::vector<std::string> possibleDataTypes() const
    { return( _dataTypes ); }
    void setPossibleDataTypes( const std::vector<std::string> & dt )
    { _dataTypes = dt; }
    std::string format() const { return( _format ); }
    void setFormat( const std::string & fmt ) { _format = fmt; }
    const Header* header() const;
    void setHeader( Header* hdr );
    carto::Object headerObject() const { return _header; }
    void setHeader( carto::Object hdr );
    State state() const { return _state; }
    std::string errorMessage() const { return _errormsg; }
    /// creates and throws an exception corresponding to the error state
    void launchException();

    static void postProcessHeader( carto::Object hdr );

  private:
    static void initPrivate();

    std::string			_objectType;
    std::string			_dataType;
    std::vector<std::string>	_dataTypes;
    std::string			_format;
    carto::Object		_header;
    State			_state;
    std::string			_errormsg;
    int				_errorcode;
    static std::auto_ptr<Finder_Private>	pd;
  };

}


#endif


