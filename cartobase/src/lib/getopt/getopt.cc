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

#include <stdexcept>
#include <cartobase/getopt/getopt.h>
#include <cartobase/config/info.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/config/verbose.h>
#include <cartobase/config/paths.h>
#include <cartobase/stream/fileutil.h>
#include <fstream>

using namespace std;
using namespace carto;


  //////////////////////
 //  missign_option  //
//////////////////////

//-----------------------------------------------------------------------------
missing_option::missing_option( const string &optionName ) :
  runtime_error( string("value missing for option \"") + optionName + "\"" )
{
}


  /////////////////////////
 //  unexpected_option  //
/////////////////////////

//-----------------------------------------------------------------------------
unexpected_option::unexpected_option( const string &optionName ) :
  runtime_error( string("unexpected option \"") + optionName + "\"" )
{
}


  ///////////////////////////////
 //  unexpected_option_value  //
///////////////////////////////

//-----------------------------------------------------------------------------
unexpected_option_value::unexpected_option_value( const string &optionName,
						  const string &value ) :
  runtime_error( string("unexpected value for option \"") + optionName +
	       "\" (" + value + ")" )
{
}


  /////////////////////////
 //  user_interruption  //
/////////////////////////

user_interruption::user_interruption() :
  runtime_error( "user interruption" )
{
}


  //////////////////
 //  OptionBase  //
//////////////////

//-----------------------------------------------------------------------------
OptionBase::OptionBase( const string &name, const string &info ) :
  _info( info )
{
  addName( name );
}

//-----------------------------------------------------------------------------
string OptionBase::names() const
{
  string result = *_names.begin();
  for( vector<string>::const_iterator i = _names.begin() + 1;
       i != _names.end(); ++i ) {
    result = result + " | " + *i;
  }
  return result;
}


  /////////////////////
 //  OptionsParser  //
/////////////////////

namespace
{

  void parseOptionsFile( const string & fname, vector<string> & fopts )
  {
    ifstream	f( fname.c_str() );
    if( !f )
      return;
    string	line;
    int		c;
    while( !f.eof() )
      {
        line.clear();
        while( true )
          {
            c = f.get();
            if( f.eof() || c == '\0' || c == '\n' )
              break;
            if( ( c != ' ' && c != '\t' ) || !line.empty() )
              line += c;
          }
        while( !line.empty() &&
               ( ( c = line[ line.length() - 1 ] ) == ' ' || c == '\t' ) )
          line.erase( line.length() - 1, 1 );
        if( !line.empty() )
          fopts.push_back( line );
      }
  }

}


//-----------------------------------------------------------------------------
OptionsParser::OptionsParser( int argc, const char **argv ) :
  insertIndex( 0 )
{
  for( int i = 0; i < argc; ++i ) {
    cla.push_back( argv[ i ] );
  }
}

//-----------------------------------------------------------------------------
OptionsParser::~OptionsParser()
{
  for( OptionList::iterator i = options.begin(); i != options.end(); ++i ) {
    delete *i;
  }
}


//-----------------------------------------------------------------------------
void OptionsParser::pushOption( OptionBase *opt )
{
  if ( options.empty() ) {
    options.push_back( opt );
  } else {
    options.insert( options.begin() + insertIndex, opt );
  }
  ++insertIndex;
}


//-----------------------------------------------------------------------------
void OptionsParser::parse()
{
  string	optionFile;
  addOption( optionFile, "--optionsfile", "Read additional commandline "
             "options from the specified file (one switch or value per line)",
             true );

  // Parse command line
  OptionBase *currentOption = NULL;
  CommandLineArguments::iterator arg = cla.begin();
  ++arg;
  while( arg != cla.end() ) {
    // Try to match an option name
    OptionList::const_iterator i;
    for( i = options.begin(); i != options.end(); ++i ) {
      if ( (*i)->recognizeName( *arg ) ) break;
    }
    if ( i == options.end() ) {
      // Not an option name
      if ( currentOption ) {
	if ( currentOption->feed( *arg ) ) {
	  ++arg;
	  continue;
	} else {
	  currentOption = NULL;
	}
      }
      for( i = options.begin(); i != options.end(); ++i ) {
	if ( (*i)->feed( *arg ) ) break;
      }
      if ( i == options.end() ) {
	throw unexpected_option( *arg );
      } else {
	++arg;
      }
    } else {
      // Option name
      currentOption = *i;
      ++arg;
      if ( arg != cla.end() ) {
	if ( currentOption->feed( *arg ) ) {
	  ++arg;
          if( currentOption->name() == "--optionsfile" )
            {
              vector<string>	fopts;
              unsigned		n = arg - cla.begin();
              parseOptionsFile( optionFile, fopts );
              cla.insert( arg, fopts.begin(), fopts.end() );
              arg = cla.begin() + n;
            }
	} else {
	  currentOption = NULL;
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
void OptionsParser::check()
{
  // Check
  for( OptionList::iterator i = options.begin(); i != options.end(); ++i ) {
    (*i)->check();
  }
}


//-----------------------------------------------------------------------------
void OptionsParser::alias( const string &to, const string &from )
{
  OptionList::iterator i;
  for( i = options.begin(); i != options.end(); ++i ) {
    if ( (*i)->_nameInList( from ) ) break;
  }
  if ( i == options.end() ) {
    throw logic_error( string( "\"" ) + to + "\" is not a valid option name" );
  } else {
    (*i)->addName( to );
  }
}



  //////////////////////////
 //  SingleOption<bool>  //
//////////////////////////

//-----------------------------------------------------------------------------
template <>
bool SingleOption<bool>::recognizeName( const std::string &n )
{
  if( _nameInList( n ) ) {
    if ( _valueRead ) {
      throw unexpected_option( n );
    }
    _value = true;
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
template <>
bool SingleOption<bool>::feed( const std::string &value )
{
  if ( _valueRead ) {
    return false;
  } else {
    try {
      stringTo< bool >( value, _value );
    }
    catch( invalid_boolean &e ) {
      return false;
    }
    _valueRead = true;
    return true;
  }
}



  /////////////////////////
 //  BoolOrNumber<int>  //
/////////////////////////


//-----------------------------------------------------------------------------
static BoolOrNumber<int> verboseParameter( verbose );




  ///////////////////////
 //  CartoApplication  //
///////////////////////

//-----------------------------------------------------------------------------
CartoApplication::CartoApplication( int argc, const char **argv,
                                    const string &documentation ) :
  OptionsParser( argc, argv ),
  _documentation( documentation )
{
  _name = FileUtil::basename( argv[ 0 ] );
  Paths::setArgv0( argv[0] );
  addOption( verboseParameter, "--verbose",
             "Set verbosity level (default = 0, without value = 1)", true );
  --insertIndex;
  addOption( carto::debugMessageLevel, "--debugLevel",
             "Set debug information level (default = 0)", true );
  --insertIndex;
}


//-----------------------------------------------------------------------------
CartoApplication::~CartoApplication()
{
}


//-----------------------------------------------------------------------------
static void printInfo()
{
  Info::print();
}

namespace
{

  //---------------------------------------------------------------------------
  string formatted( const string & in, unsigned indent = 4 )
  {
    string::size_type	pos = 0, cols = 80, sz = in.size(), p2, ccols = cols,
      i, skip;
    string	out, prefix;
    char	c;
    bool	first = true;

    while( pos < sz )
      {
        skip = 1;
        p2 = in.find( '\n', pos );
        if( p2 == string::npos )
          p2 = sz;
        if( p2 - pos >= ccols )
          {
            p2 = pos + ccols;
            // try to find a cesure
            for( i=p2-1; i>pos; --i )
              {
                c = in[i];
                if( c == ' ' )
                  {
                    p2 = i;
                    break;
                  }
                if( c == ',' )
                  {
                    p2 = i+1;
                    skip = 0;
                    break;
                  }
              }
            while( p2 > pos && in[p2-1] == ' ' )
              {
                --p2;
                ++skip;
              }
          }
        out += prefix + in.substr( pos, p2 - pos );
        if( first )
          {
            first = false;
            for( i=0; i<indent; ++i )
              prefix += ' ';
            ccols = cols - indent;
          }
        while( p2+skip < sz && in[p2+skip] == ' ' )
          ++skip;
        pos = p2 + skip;
        if( pos < sz )
          out += '\n';
      }
    return out;
  }

}

//-----------------------------------------------------------------------------
void CartoApplication::initialize()
{

  bool help = false, version = false, info = false;

  insertIndex = options.size();
  addOption( help, "-h", "show help message", true );
  alias( "--help", "-h" );
  addOption( version, "--version", "show Cartograph version", true );
  addOption( info, "--info", "show libraries information (install, plugins, "
	     "paths, etc.)", true );
  parse();

  PluginLoader::load();

  if ( help )
    {
      cout << endl << "    " << _name << endl
           << "    ";
      unsigned	i, n = _name.size();
      for( i=0; i<n; ++i )
        cout << '-';
      cout << endl << endl << formatted( _documentation, 0 ) << endl << endl;
      if ( ! options.empty() ) {
        cout << "Options :" << endl << endl;
        for( OptionList::const_iterator i = options.begin();
             i != options.end(); ++i ) {
          cout << formatted( (*i)->info() ) << endl << endl;
        }
      }
      throw user_interruption();
    }

  if( version )
    Info::printVersions();

  if( info )
    printInfo();
  if( version || info )
    throw user_interruption();

  check();
}


