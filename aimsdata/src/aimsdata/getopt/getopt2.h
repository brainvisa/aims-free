#ifndef AIMS_GETOPT_GETOPT2_H
#define AIMS_GETOPT_GETOPT2_H

#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/datatypecode.h>
#include <cartobase/getopt/getopt.h>

namespace carto {


  /////////////////////////////////
 //  SingleOption< Reader<T> >  //
/////////////////////////////////

template <class T>
class SingleOption< aims::Reader<T> > : public OptionBase
{
  aims::Reader<T> &_value;
  bool _optional;
  bool _valueRead;

public:

  inline SingleOption( aims::Reader<T> &value, const std::string &name, 
		       const std::string &info, bool optional );
  virtual inline ~SingleOption();

  virtual bool recognizeName( const std::string & );
  virtual bool feed( const std::string & );
  virtual void check();
  std::string info() const;
};

//-----------------------------------------------------------------------------
template <class T>
inline 
SingleOption< aims::Reader<T> >::SingleOption( aims::Reader<T> &value, 
					       const std::string &name, 
					       const std::string &info, 
					       bool optional ) :
  OptionBase( name, info ),
  _value( value ),
  _optional( optional ),
  _valueRead( false ) 
{
}

//-----------------------------------------------------------------------------
template <class T>
inline SingleOption< aims::Reader<T> >::~SingleOption() {}

//-----------------------------------------------------------------------------
template <class T> inline 
bool SingleOption< aims::Reader<T> >::recognizeName( const std::string &n )
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
template <class T> inline 
void SingleOption< aims::Reader<T> >::check()
{
  if ( ! _optional && ! _valueRead ) {
    throw missing_option( name() );
  }
}

//-----------------------------------------------------------------------------
template <class T> inline 
std::string SingleOption< aims::Reader<T> >::info() const
{
  if ( _optional ) {
    return std::string( "[ " ) + names() + " <file name (read only): " + 
      DataTypeCode<T>::name() + "> ]\n" + _info;
  } else {
    return names() + " <file name (read only): " 
      + DataTypeCode<T>::name() 
      + ">\n" + _info;
  }
}

//-----------------------------------------------------------------------------
template <class T> inline 
bool SingleOption< aims::Reader<T> >::feed( const std::string &value )
{
  if ( _valueRead ) {
    return false;
  } else {
    _value.setFileName( value );
    _valueRead = true;
    return true;
  }
}


  /////////////////////////////////
 //  SingleOption< Writer<T> >  //
/////////////////////////////////

template <class T>
class SingleOption< aims::Writer<T> > : public OptionBase
{
  aims::Writer<T> &_value;
  bool _optional;
  bool _valueRead;

public:

  inline SingleOption( aims::Writer<T> &value, const std::string &name, 
		       const std::string &info, bool optional );
  virtual inline ~SingleOption();

  virtual bool recognizeName( const std::string & );
  virtual bool feed( const std::string & );
  virtual void check();
  std::string info() const;
};

//-----------------------------------------------------------------------------
template <class T>
inline 
SingleOption< aims::Writer<T> >::SingleOption( aims::Writer<T> &value, 
					       const std::string &name, 
					       const std::string &info, 
					       bool optional ) :
  OptionBase( name, info ),
  _value( value ),
  _optional( optional ),
  _valueRead( false ) 
{
}

//-----------------------------------------------------------------------------
template <class T>
inline SingleOption< aims::Writer<T> >::~SingleOption() {}

//-----------------------------------------------------------------------------
template <class T> inline 
bool SingleOption< aims::Writer<T> >::recognizeName( const std::string &n )
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
template <class T> inline 
void SingleOption< aims::Writer<T> >::check()
{
  if ( ! _optional && ! _valueRead ) {
    throw missing_option( name() );
  }
}

//-----------------------------------------------------------------------------
template <class T> inline 
std::string SingleOption< aims::Writer<T> >::info() const
{
  if ( _optional ) {
    return std::string( "[ " ) + names() + " <filename: " 
      + DataTypeCode<T>::name() + "> ]\n" + _info;
  } else {
    return names() + " <filename: " 
      + DataTypeCode<T>::name() + ">\n" + _info;
  }
}

//-----------------------------------------------------------------------------
template <class T> inline 
bool SingleOption< aims::Writer<T> >::feed( const std::string &value )
{
  if ( _valueRead ) {
    return false;
  } else {
    _value.setFileName( value );
    _valueRead = true;
    return true;
  }
}

} // namespace carto

namespace aims
{

  ///////////////////////
 //  AimsApplication  //
///////////////////////

class AimsApplication : public carto::CartoApplication
{
public:

  AimsApplication( int argc, const char **argv, 
		   const std::string &documentation );
  ~AimsApplication();

  static AimsApplication *globalApplication();

  virtual void initialize();

  int output_4d_volumes;
};

} // namespace aims


#endif // ifndef AIMS_GETOPT_GETOPT2_H
