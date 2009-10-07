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

#ifndef CARTOBASE_IO_READERALGORITHM_H
#define CARTOBASE_IO_READERALGORITHM_H

#include <cstdlib>
#include <cartobase/config/cartobase_config.h>
#include <cartobase/algorithm/algorithm.h>
#include <string>
#include <map>


namespace carto
{
  class DataSourceInfo;
  class DataSource;

  /*!	\brief Link mechanism between a data source or file and an algorithm 
	operating on arbitrary data types

	ReaderAlgorithm solves the data-type problem for any Cartograph 
	commandline algorithm. It allows to hide both the file format 
	of processed data (using DataSourceInfo) and the data type dependency.
	ReaderAlgorithm allows to call an algorithm operation function 
        depending on the correct data type given by its file info (using a 
        functions map).

	No type switching needs to be hard-coded anymore.
	ReaderAlgorithm uses a plugin mechanism for new data types and the 
        types list can be dynamically extended when needed.

        Usage: The idea is to plug into a ReaderAlgorithm (generally a 
        subclass) some pointers to type-specialized processing functions. 
        These functions can be either external functions (generally template):
	\code
	template <typename T>
	bool doMyReaderAlgorithm( ReaderAlgorithm & algo, 
				  Object header, 
				  rc_ptr<DataSource> source );
	\endcode
	or static member functions:
	\code
	template <typename T> static 
	bool
	MyReaderAlgorithm::doMyReaderAlgorithm( ReaderAlgorithm & algo, 
						Object header, 
						rc_ptr<DataSource> source );
	\endcode
	Instantiate such an algorothm class, and register every needed 
	type with the registerAlgorithmType() function (you can do it in your 
	algo constructor).
	Then call the execute() function with the file name (or other 
        DataSource) as an argument. The algo function registered for the 
        correct data type will be called, if this type can be guessed by the 
        DataSourceInfo, or an exception will be thrown if it fails.

	In fact if your algo doesn't need other information (no data in 
	your ReaderAlgorithm subclass), you don't even really need to subclass 
	it: manually registering algo functions should be enough.

	A small example:

	\code
	#include <cartobase/io/readingalgorithm.h>
	#include <cartobase/io/datasourceinfo.h>
	#include <cartobase/io/reader.h>
	#include <cartodata/volume/volume.h>
	#include <cartobase/getopt/getopt.h>
	#include <aims/mesh/surface.h>
	#include <aims/bucket/bucket.h>

	using namespace carto;
	using namespace std;

	class MyAlgo : public ReaderAlgorithm
	{
	public:
	  MyAlgo()
	    : ReaderAlgorithm( "MyAlgo" )
	  {
	    // here are all types known by our algo
	    registerAlgorithmType( "Volume of S16", &exec<Volume<short> > );
	    registerAlgorithmType( "Volume of FLOAT", &exec<AimsData<float> > );
	    // use for instance another function for meshes and buckets
	    registerAlgorithmType( DataTypeCode<AimsSurfaceTriangle>::name(), 
				   &exec2<AimsSufaceTriangle> );
	    registerAlgorithmType( DataTypeCode<AimsBucket>::name(), 
				   &exec2<AimsBucket> );
	  }

	private:
	  // template processing function, used for some of the possible types
	  template <typename T> static bool 
	  exec( ReaderAlgorithm& a, Object hdr, rc_ptr<DataTypeCode> src )
	  {
	    MyAlgo	& ma = (MyAlgo &) a;
	    // we are sure p is a MyReaderAlgorithm since this -private- 
	    // function has been registered and called only within the 
	    // MyReaderAlgorithm class if you're still not sure, use a 
	    // dynamic_cast()...

	    //	read it
	    Reader<T>	r( src );
	    // it is a good habit to hold object that must be destroyed in 
	    // a smart pointer (auto_ptr, rc_ptr or scoped_ptr)
	    auto_ptr<T> data;
	    data = r.read( hdr );

	    // now use your data as you like
	    // ...
	    ma.otherFunction( data );	// etc.

	    return true;	// success
	  }

	  // another processing function that we use on some other types
	  template <typename T> static bool
	  exec2( ReaderAlgorithm &, Object hdr, rc_ptr<DataSource> src )
	  {
	    T		data;
	    Reader<T>	r( src );
	    auto_ptr<T> data;
	    data = r.read( hdr );

	    // other processing stuff
	    // ...

	    return true;
	  }

	};


	// main function

	int main( int argc, const char** argv )
	{
	  try
	    {
	      CartoApplication app( argc, argv, "MyAlgo" );
	      string		fname;
	      app.addOption( fname, "-i", "input data file" );

	      app.initialize();

	      MyAlgo		proc;
	      if( !proc.execute( fname ) )
	        {
	          cerr << "couldn't process file " << fname << endl;
	          return EXIT_FAILURE;
	        }
	      cout << "OK\n";
	    }
	  catch( user_interruption & )
	    {
	    }
	  catch( exception & e )
	    {
	      cerr << e.what() << endl;
	      return EXIT_FAILURE;
	    }

	  return EXIT_SUCCESS;
	}
	\endcode

	\par Comments
	The "trick" is that processing functions are "neutral" functions and 
	do not have any mark of the data type in their signature (nor in their 
	return type). However they can be template functions, one for each 
	type, with identical parameters.

	\par
	This approach of binding IO to objects and corresponding algorithms 
	requires to manually register every possible data type to process, 
	but so has the advantage to allow compilation of algorithms only on 
	these types, and not on others which might cause problems. For instance
	you can use different algo functions for volumes of scalars 
	(\c Volume of \c float or \c int) and on volumes of complex, and use 
	specific operations for the former (ie ordering, comparisons 
	operations or casting to \c float or \c double) which would not work 
	on the latter.

	\see DataSourceInfo FormatDictionary Reader Writer
  */
  class ReaderAlgorithm : public Algorithm
  {
  public:
    /**	Algorithm function type. These functions are stored in a 
       type-to-function map and called by the execute() method. They are 
       called with the DataSource containing the object to read and the 
       corresponding header: here DataSourceInfo::check() has already been 
       successfully called by execute(). Each function must 
       be specialized for a given object and data type (possibly using 
       template functions). Unfortunately these functions pointers can not 
       be pointer to member functions as we will need access to derived 
       classes members (not allowed in C++ spec), so we also provide a 
       reference to \c this ReaderAlgorithm object to the algo function 
       (callback-style)
    */
    typedef bool (*ProcFunc)( ReaderAlgorithm &, Object header, 
                              rc_ptr<DataSource> source );

    ReaderAlgorithm( const std::string & algoname );
    virtual ~ReaderAlgorithm();

    /**	Registers the algo function to call on a given object type
	(just fills the map) */
    void registerAlgorithmType( const std::string & objectType, 
                                ProcFunc procFunc );
    /**	Executes the algo on the object type found in the given 
	file. Uses DataSourceInfo to figure out which data should be used. 
        This will either call the registered ProcFunc or throw a 
        datatype_format_error exception if no algo function has been 
        registered on that type
    */
    bool execute( const std::string & filename );
    /// same as above but uses a DataSource as input
    bool execute( rc_ptr<DataSource> stream );
    /**	Same as above but the header has already been read (or hand-made to 
	fake it!) */
    bool execute( Object header, rc_ptr<DataSource> source );
    /// Query registered process types
    const std::map<std::string, ProcFunc> & algorithmTypes() const
    { return( _execs ); }

  protected:
    std::map<std::string, ProcFunc>	_execs;
  };

}


#endif

