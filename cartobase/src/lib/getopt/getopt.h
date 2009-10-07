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

#ifndef CARTOBASE_GETOPT_GETOPT_H
#define CARTOBASE_GETOPT_GETOPT_H

#include <cartobase/type/types.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/exception/format.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace carto {


  //////////////////////
 //  missign_option  //
//////////////////////

class missing_option : public std::runtime_error
{
public:

  missing_option( const std::string &optionName );
  inline missing_option( const std::string &message, bool ) :
    std::runtime_error( message ) {}
};


  /////////////////////////
 //  unexpected_option  //
/////////////////////////

class unexpected_option : public std::runtime_error
{
public:

  unexpected_option( const std::string &optionName );
  inline unexpected_option( const std::string &message, bool ) :
    std::runtime_error( message ) {}
};


  ///////////////////////////////
 //  unexpected_option_value  //
///////////////////////////////

class unexpected_option_value : public std::runtime_error
{
public:

  unexpected_option_value( const std::string &optionName, 
			   const std::string &value );
  inline unexpected_option_value( const std::string &message, bool ) :
    std::runtime_error( message ) {}
};


  //////////////////
 //  OptionBase  //
//////////////////

class OptionBase
{
  friend class OptionsParser;

protected:

  std::vector<std::string> _names;
  std::string _info;
  

  virtual bool recognizeName( const std::string & ) = 0;
  virtual bool feed( const std::string & ) = 0;
  virtual void check() = 0;


  OptionBase( const std::string &name, const std::string &info );
  virtual inline ~OptionBase();

  inline void addName( const std::string &name );
  inline const std::string &name() const;
  std::string names() const;
  inline bool _nameInList( const std::string &name ) const;

public:

  virtual std::string info() const = 0;
};

//-----------------------------------------------------------------------------
inline OptionBase::~OptionBase() {}

//-----------------------------------------------------------------------------
inline void OptionBase::addName( const std::string &name ) {
    _names.push_back( name );
  }

//-----------------------------------------------------------------------------
inline const std::string &OptionBase::name() const
{
  return *_names.begin();
}

//-----------------------------------------------------------------------------
inline bool OptionBase::_nameInList( const std::string &name ) const
{
  return find( _names.begin(), _names.end(), name ) != _names.end();
}


  //////////////////////
 //  OptionSeries<T>  //
//////////////////////

template <typename T>
class OptionSeries : public OptionBase
{
  friend class OptionsParser;

  OptionSeries( T &serie, const std::string &name, const std::string &info,
                unsigned numMin, unsigned numMax );
  virtual inline ~OptionSeries();

  virtual bool recognizeName( const std::string & );
  virtual bool feed( const std::string & );
  virtual void check();
  virtual std::string info() const;

  T &_values;
  unsigned _numMin, _numMax;
  typename T::iterator _storeNextValue;
  unsigned _numValuesStored;
};

//-----------------------------------------------------------------------------
template <typename T>
inline OptionSeries<T>::~OptionSeries() {}

//-----------------------------------------------------------------------------
template <typename T> inline 
OptionSeries<T>::OptionSeries( T &serie, const std::string &name, 
                               const std::string &info,
                               unsigned numMin, unsigned numMax ) :
  OptionBase( name, info ),
  _values( serie ),
  _numMin( numMin ),
  _numMax( numMax ),
  _storeNextValue( _values.begin() ),
  _numValuesStored( 0 )
{
}

//-----------------------------------------------------------------------------
template <typename T> inline 
bool OptionSeries<T>::recognizeName( const std::string &name )
{
  if( _nameInList( name ) ) {
    if ( _numMax && _numValuesStored >= _numMax ) {
      throw unexpected_option( name );
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
template <typename T> inline 
bool OptionSeries<T>::feed( const std::string &value )
{
  if ( _numMax == 0 || _numValuesStored < _numMax ) {
    if ( _storeNextValue == _values.end() ) {
      _values.push_back( typename T::value_type() );
      stringTo< typename T::value_type >( value, *(_values.rbegin()) );
      _storeNextValue = _values.end();
    } else {
      stringTo< typename T::value_type >( value, *_storeNextValue );
      ++_storeNextValue;
    }
    ++_numValuesStored;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
template <typename T> inline 
void OptionSeries<T>::check()
{
  if ( _values.size() < _numMin ) throw missing_option( name() );
}

//-----------------------------------------------------------------------------
template <typename T> inline 
std::string OptionSeries<T>::info() const
{
  if ( _numMin ) {
    return names() + " <" + DataTypeCode<T>::name() + ">\n" + _info;
  } else {
    return std::string( "[ " ) + names() + " <" 
      + DataTypeCode<T>::name() 
      + "> ]\n" + _info;
  }
}




  ///////////////////////
 //  SingleOption<T>  //
///////////////////////

template <typename T>
class SingleOption : public OptionBase
{
protected:

  T &_value;
  bool _optional;
  bool _valueRead;

public:

  inline SingleOption( T &value, const std::string &name, 
		       const std::string &info, bool optional );
  virtual inline ~SingleOption();

  virtual bool recognizeName( const std::string & );
  virtual bool feed( const std::string & );
  virtual void check();
  std::string info() const;
};

//-----------------------------------------------------------------------------
template <typename T>
inline SingleOption<T>::~SingleOption() {}

//-----------------------------------------------------------------------------
template <typename T>
inline SingleOption<T>::SingleOption( T &value, const std::string &name, 
				      const std::string &info, 
				      bool optional ) :
  OptionBase( name, info ),
  _value( value ),
  _optional( optional ),
  _valueRead( false ) 
{
}

//-----------------------------------------------------------------------------
template <typename T>  
bool SingleOption<T>::recognizeName( const std::string &n )
{
  if( _nameInList( n ) ) {
    if ( _valueRead ) {
      throw unexpected_option( n );
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
template <typename T> 
bool SingleOption<T>::feed( const std::string &value )
{
  if ( _valueRead ) {
    return false;
  } else {
    stringTo< T >( value, _value );
    _valueRead = true;
    return true;
  }
}

//-----------------------------------------------------------------------------
template <typename T> inline 
void SingleOption<T>::check()
{
  if ( ! _optional && ! _valueRead ) {
    throw missing_option( name() );
  }
}

//-----------------------------------------------------------------------------
template <typename T> inline 
std::string SingleOption<T>::info() const
{
  if ( _optional ) {
    return  std::string( "[ " ) + names() + " <" 
      + DataTypeCode<T>::name() 
      + "> ]\n" + _info;
  } else {
    return  names() + " <" + DataTypeCode<T>::name() + ">\n" + _info;
  }
}


  //////////////////////////
 //  SingleOption<bool>  //
//////////////////////////

//-----------------------------------------------------------------------------
template <>
bool SingleOption<bool>::recognizeName( const std::string &n );


//-----------------------------------------------------------------------------
template <>
bool SingleOption<bool>::feed( const std::string &value );



  ///////////////////////
 //  BoolOrNumber<T>  //
///////////////////////

template <class NumberType>
class BoolOrNumber
{
public:

  inline BoolOrNumber( NumberType &v ) : value( v ) {}
  NumberType &value;
};



  ////////////////////////////////////
 //  SingleOption< BoolOrNumber >  //
////////////////////////////////////

template <class NumberType>
class SingleOption< BoolOrNumber<NumberType> > :
  public SingleOption<NumberType>
{

public:

  inline SingleOption( BoolOrNumber< NumberType > &value, 
                       const std::string &name, 
                       const std::string &info, bool optional );
  virtual inline ~SingleOption();
  
  virtual bool recognizeName( const std::string & );
  virtual bool feed( const std::string & );
  std::string info() const;
};

//-----------------------------------------------------------------------------
template <typename NumberType>
inline SingleOption< BoolOrNumber<NumberType> >::
SingleOption( BoolOrNumber< NumberType > &value, 
              const std::string &name, 
              const std::string &info, bool optional ) :
  SingleOption<NumberType>( value.value, name, info, optional ) {}

//-----------------------------------------------------------------------------
template <typename NumberType>
inline SingleOption< BoolOrNumber<NumberType> >::~SingleOption() {}

//-----------------------------------------------------------------------------
template <typename NumberType>
bool SingleOption< BoolOrNumber<NumberType> >::
recognizeName( const std::string &n )
{
  if( this->_nameInList( n ) ) {
    if ( this->_valueRead ) {
      throw unexpected_option( n );
    }
    this->_value = NumberType( 1 );
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
template <typename NumberType>
bool SingleOption< BoolOrNumber<NumberType> >::feed( const std::string &value )
{
  if ( this->_valueRead ) {
    return false;
  } else {
    try {
      stringTo< NumberType >( value, this->_value );
    }
    catch( invalid_number &e ) {
      return false;
    }
    this->_valueRead = true;
    return true;
  }
}

//-----------------------------------------------------------------------------
template <class NumberType> inline 
std::string SingleOption< BoolOrNumber< NumberType > >::info() const
{
  return  std::string( "[ " ) + this->names() + " [ <" 
    + DataTypeCode<NumberType>::name() 
    + "> ] ]\n" + this->_info;
}


  /////////////////////
 //  OptionsParser  //
/////////////////////

/** Commandline options parser.
    Usage: instantiate a parser, add parameters using addOption(),
    then call parse() and check().

    You generally do not use OptionsParser directly, but rather a subclass of it:
    \c CartoApplication, or \c AimsApplication in aims commands.

    example:
    \code
    std::string input, output;
    int param = 10;
    OptionsParser app( argc, argv );
    app.addOption( input, "-i", "input file" );
    app.addOption( output, "-o", "output file" );
    app.addOption( param, "-p", "an int parameter (default value: 10)", true );
    try
    {
      app.parse();
      app.check();
    }
    catch( std::exception & e )
    {
      std::cerr << e.what();
      throw;
    }
    \endcode
*/
class OptionsParser
{
public:
  typedef std::vector< std::string > CommandLineArguments;

protected:
  CommandLineArguments cla;

  typedef std::vector< OptionBase * > OptionList;
  OptionList options;

  int insertIndex;
  void pushOption( OptionBase * );

public:

  OptionsParser( int argc, const char **argv );
  ~OptionsParser();

  /** Adds a single parameter to be parsed on the commandline

  A single parameter can only appear once in a commandline: for instance 
  if the \c "-i" parameter is a single option, you cannot call the 
  commanline with the arguments <tt>"-i file1 -i file2"</tt> (this will raise 
  an exception).
  \param param the parameter variable (it will be filled automatically when 
  parse() is called)
  \param token option switch used to identify this parameter on the 
  commandline (\c "-i" or \c "-o" for instance). Any string is accepted so 
  they are not necessarily one-character switches, and they even do not need 
  to start by a \c '-', althrough it is strongly recommended for clarity.
  \param description the help string for this option (displayed when 
  invoking the commandline with the \c "--help" option)
  \param optional if true, the parameter is not mandatory, and if it is not 
  provided on the commandline, the corresponding variable will be left 
  untouched. So optional parameters variables must be initialized with a 
  default value.
  */
  template <typename T>
  void addOption( T & param, const std::string & token, 
                  const std::string & description, 
		  bool optional = false );
  /** Adds a series of parameters to be parsed on the commandline

  A series of parameters is a STL sequence of parameters of the same type, all 
  given by the same commandline option switch: for instance if \c "-i" is 
  a series of parameters of type string, the commandline will accept the 
  arguments <tt>"-i file1 -i file2"</tt>
  \param param the parameter variable (it will be filled automatically when 
  parse() is called). It must be a STL-compatible sequence (like a vector 
  or a list) with iterators (begin(), end(), rbegin() methods), a value_type 
  type, and a push_back() insertion method.
  \param token option switch used to identify this parameter on the 
  commandline
  \param description the help string for this option (displayed when 
  invoking the commandline with the \c "--help" option)
  \param minnum minimum admissible number of occurrences of this parameter: 0 
  means it it optional
  \param maxnum maximum admissible number of occurrences of this parameter, 
  the special value 0 means unlimited
  */
  template <typename T>
  void addOptionSeries( T & param, const std::string & token, 
			const std::string & description, unsigned minnum = 0, 
			unsigned maxnum = 0 );
  /// Gives an alternative name (token) \c to to the parameter of token \c from
  void alias( const std::string &to, const std::string &from );
  /** Parses the commandline arguments (argc, argv) and fills in the 
      registered parameters variables. This must be done after all addOption() 
      / addOptionSeries() calls.
  */
  void parse();
  /** Check all needed parameters are filled in, and series numbers are OK. 
      This method must be called after parse().
  */
  void check();
};

//-----------------------------------------------------------------------------
template <typename T> inline 
void OptionsParser::addOption( T &value, const std::string &name,
			       const std::string &info, bool optional )
{
  pushOption( new SingleOption<T>( value, name, info, optional ) );
}

//-----------------------------------------------------------------------------
template <typename T> inline 
void OptionsParser::addOptionSeries( T &values, const std::string &name, 
				     const std::string &info, 
				     unsigned numMin, unsigned numMax )
{
  pushOption( new OptionSeries<T>( values, name, info, numMin, numMax ) );
}


  ///////////////////////
 //  CartoApplication  //
///////////////////////

/** Cartograph options parser.
    Common options such as \c "--help", \c "--info" and \c "--version" are added.
    CartoApplication also requires a (short) documentation.

    Usage: instantiate CartoApplication, add options using addOption(),
    then call initialize() to parse commandline options.

    initialize() should always been called within a try..catch block, and
    the exception \c carto::user_interruption should be catched because it is
    thrown whenever the help is required (--help option).
*/
class CartoApplication : public OptionsParser
{
  std::string _documentation;
  std::string _name;

public:

  CartoApplication( int argc, const char **argv, 
                    const std::string &documentation );
  virtual ~CartoApplication();
  /** Parse and check all parameters, and handle common options (such as 
      the \c "--help" option).
      initialize() also triggers loading plugins because some functionalities 
      provided as plugins may be needed to correctly handle some options.
  */
  virtual void initialize();
};


  /////////////////////////
 //  user_interruption  //
/////////////////////////

/** A special exception which is thrown after a command help is requested and
    displayed in a CartoApplication. This exception is thrown to prevent the
    normal command to run, and exit after printing the help.
*/
class user_interruption : public std::runtime_error
{
public:

  user_interruption();
  inline user_interruption( const std::string &message ) :
    std::runtime_error( message ) {}
};

} // namespace carto


#endif // ifndef CARTO_GETOPT_GETOPT_H

