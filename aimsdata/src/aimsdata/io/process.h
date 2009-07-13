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
 *  ...
 */
#ifndef AIMS_IO_PROCESS_H
#define AIMS_IO_PROCESS_H

#include <cstdlib>
#include <aims/config/aimsdata_config.h>
#include <string>
#include <map>


namespace aims
{
  class Finder;


  /*!	\class Process process.h aims/io/process.h
	\brief Link mechanism between the Finder and a process operating on 
	arbitrary data types

	Process finally solves the data-type problem for every AIMS 
	commandline (I hope...). It allows to hide both the file format 
	of processed data (using Finder) and the data type dependency.
	Process allows to call a process operation function depending on the 
	correct data type given by its file info (using a functions map).\n

	No type switching need to be hard-coded anymore.
	Process uses a plugin mechanism for new data types and the types list 
	can be dynamically extended when needed.

	Usage: Subclass Process to your own class with either an external 
	\code
	template<class T> bool doMyProcess( Process & p, 
					    const string & filename, 
					    Finder & finder );
	\endcode
	or a static member function:
	\code
	template<class T> static bool
	MyProcess::doMyProcess( Process & p, const string & filename, 
				Finder & finder );
	\endcode
	Instantiate such a process class, and register every needed 
	type with the registerProcessType() function (you can do it in your 
	process constructor).
	Then call the execute() function with your file name as an 
	argument. The process function registered for the correct data type 
	will be called, if this type can be guessed by the Finder, or return 
	false if it fails.

	In fact if your process doesn't need other information (no data in 
	your Process subclass), you don't even really need to subclass it: 
	manually registering processing functions should be enough.

	An example of it: 
	\link AimsFileInfo/fileInfo.cc the AimsFileInfo command \endlink

	For quick use, another small example:

	\code
	#include <aims/io/process.h>
	#include <aims/io/finder.h>
	#include <aims/io/reader.h>
	#include <aims/data/data.h>
	#include <aims/mesh/surface.h>
	#include <aims/bucket/bucket.h>

	class MyProcess : public Process
	{
	public:
	  MyProcess()
	  {
	    // here are all types known by our process
	    registerProcessType( "Volume", "S16", &exec<AimsData<short> > );
	    registerProcessType( "Volume", "FLOAT", &exec<AimsData<float> > );
	    // use for instance another function for meshes and buckets
	    registerProcessType( "Mesh", "VOID", &exec2<AimsSufaceTriangle> );
	    registerProcessType( "Bucket", "VOID", &exec2<AimsBucket> );
	  }

	private:
	  // template processing function, used for some of the possible types
	  template<class T> static bool 
	  exec( Process& p, const string & filename, Finder & finder )
	  {
	    MyProcess	& mp = (MyProcess &) p;
	    // we are sure p is a MyProcess since this -private- function has 
	    // been registered and called only within the MyProcess class
	    // if you're still not sure, use a dynamic_cast()...

	    // instantiate the data
	    T	data;
	    //	read it
	    Reader<T>	r( filename );
	    string	format = finder.format();
	    if( !r.read( data, 0, &format ) )
	      return( false );

	    // now use your volume as you like
	    // ...
	    mp.otherFunction( data );	// etc.

	    return( true );	// success
	  }

	  // another processing function that we use on some other types
	  template<class T> static bool
	  exec2( Process &, const string & filename, Finder & finder )
	  {
	    T		data;
	    Reader<T>	r( filename );
	    string	format = finder.format();
	    if( !r.read( data, 0, &format ) )
	      return( false );

	    // other processing stuff
	    // ...

	    return( true );
	  }

	};


	// main function

	int main( int argc, char** argv )
	{
	  if( argc < 2 )
	    exit( 1 );

	  string		fname( argv[1] );
	  MyProcess		proc;
	  if( !proc.execute( fname ) )
	    {
	      cerr << "couldn't process file " << fname << endl;
	      exit( 1 );
	    }
	  cout << "OK\n";

	  return( 0 );
	}
	\endcode

	\par Comments
	The "trick" is that processing functions are "neutral" functions and 
	do not have any mark of the data type in their signature (nor in their 
	return type). However they can be template functions, one for each 
	type, with identical parameters.

	\par
	This approach of binding IO to objects and corresponding processes 
	requires to manually register every possible data type to process, 
	but so has the advantage to allow compilation of processes only on 
	these types, and not on others which might cause problems. For instance
	you can use different process functions for volumes of scalars 
	(\c AimsData of \c float or \c int) and on volumes of complex, and use 
	specific operations for the former (ie ordering, comparisons 
	operations or casting to \c float or \c double) which would not work 
	on the latter.

	\see Finder FileFormatDictionary Reader Writer
  */
  class AIMSDATA_API Process
  {
  public:
    /**	Process function type. These functions are stored in a type-to-function
	map and called by the execute() function. They are called with the 
	file name of the object and the corresponding Finder object, the 
	Finder::check() has already been successfully called by execute(), 
	so the file header is available in the Finder. Each function must 
	be specialized for a given object and data type (possibly using 
	template functions). Unfortunately these functions pointers can not 
	be pointer to member functions as we will need access to derived 
	classes members (not allowed in C++ spec), so we also provide a 
	reference to \c this Process object to the process function 
	(callback-style) */
    typedef bool (*ProcFunc)( Process &, const std::string &, Finder & );
    /**	Attempts to read the header of \c filename and, if successful, 
	calls the \c operator() of the process \process */

    Process();
    virtual ~Process();

    /**	Registers the process to call on a given (object type, data type) 
	couple (just fills the map) */
    void registerProcessType( const std::string & objType, 
			      const std::string & dataType, 
			      ProcFunc procFunc );
    /**	Executes the process on the object / data type found in the given 
	file. Uses Finder to figure out which data should be used. This 
	will either call the registered ProcFunc or return \c false if no 
	process function has been registered on that type */
    bool execute( const std::string & filename );
    /**	Same as above but the header has already been read (or hand-made to 
	fake it!) */
    bool execute( Finder & f, const std::string & filename );
    /// Query registered process types
    const std::map<std::string, std::map<std::string, ProcFunc> > & 
    processTypes() const
    { return( _execs ); }

  protected:
    std::map<std::string, std::map<std::string, ProcFunc> >	_execs;
  };


  /*!	\example AimsFileInfo/fileInfo.cc
	Example of the use of the Process class: 
	the the AimsFileInfo command
  */
}


#endif

