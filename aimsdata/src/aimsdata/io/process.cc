// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <cartobase/exception/format.h>
#include <iostream>
#include <algorithm>

using namespace aims;
using namespace carto;
using namespace std;


Process::Process()
{
}


Process::~Process()
{
}


void Process::registerProcessType( const string & objType, 
				   const string & dataType, ProcFunc procFunc )
{
  _execs[ objType ][ dataType ] = procFunc;
}


bool Process::execute( const string & filename )
{
  //cout << "Process::execute( " << filename << " )\n";
  Finder	f;
  if( !f.check( filename ) )
    {
      f.launchException();
      throw format_error( filename );
      return( false );
    }
  return( execute( f, filename ) );
}


bool Process::execute( Finder & f, const string & filename )
{
  string	otype = f.objectType();
  string	dtype = f.dataType();

  vector<string>    posstypes = f.possibleDataTypes();
  unsigned          i, n = posstypes.size();

  if( _options )
    try
    {
      Object pdt = _options->getProperty( "preferred_data_type" );
      if( pdt )
      {
        string pdts = pdt->getString();
        vector<string>::const_iterator ipt
          = find( posstypes.begin(), posstypes.end(), pdts );
        if( ipt != posstypes.end() )
          // preferred_data_type is available: try to use it first.
          dtype = pdts;
      }
    }
    catch( ... )
    {
    }

  map<string, map<string, ProcFunc> >::const_iterator
    ip = _execs.find( otype );
  if( ip == _execs.end() )
  {
    throw datatype_format_error( string( "unsupported data type " )
                                          + otype + " / " + dtype, filename );
    return( false );
  }

  map<string, ProcFunc>::const_iterator  ip2 = (*ip).second.find( dtype );
  if( ip2 == (*ip).second.end() )
    {
      // Try alternate data types

      for( i=0; i<n; ++i )
	if( posstypes[i] != dtype )
	  {
	    ip2 = (*ip).second.find( posstypes[i] );
	    if( ip2 != (*ip).second.end() )
	      {
		// force new datatype into finder
		f.setDataType( posstypes[i] );
		break;
	      }
	  }
      if( i == n )
	{
	  throw datatype_format_error( string( "unsupported data type " ) 
				       + otype + " / " + dtype, filename );
	  return( false );
	}
    }

  //	execute process function
  return( (*ip2).second( *this, filename, f ) );
}


