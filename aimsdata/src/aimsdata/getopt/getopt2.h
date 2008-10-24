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
