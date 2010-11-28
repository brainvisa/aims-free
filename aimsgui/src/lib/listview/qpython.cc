/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
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

#include <aims/listview/qpython.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/object/property.h>
#include <iomanip>
#include <math.h>
#include <aims/qtcompat/qlistview.h>

using namespace aims::gui;
using namespace carto;
using namespace std;

QPythonPrinter::QPythonPrinter( Q3ListViewItem* parent, 
				const SyntaxSet & syntax, 
				const HelperSet & helpers )
  : _rules( syntax ), _helpers( helpers ), _lview( 0 ), _lvitem( parent ), 
    _valcol( 2 ), _attcol( 0 ), _typcol( 1 )
{
  initHelpers();
}


QPythonPrinter::QPythonPrinter( Q3ListView* parent, 
				const SyntaxSet & syntax, 
				const HelperSet & helpers )
  : _rules( syntax ), _helpers( helpers ), _lview( parent ), _lvitem( 0 ), 
    _valcol( 2 ), _attcol( 0 ), _typcol( 1 )
{
  initHelpers();
}


QPythonPrinter::~QPythonPrinter()
{
}


namespace
{

  template<typename T>
  inline QString tostring( const T & x )
  {
    return( QString::number( x ) );
  }


#if QT_VERSION < 0x030300
  template<>
  inline QString tostring( const long long & x )
  {
    return( QString::number( (long) x ) );
  }


  template<>
  inline QString tostring( const unsigned long long & x )
  {
    return( QString::number( (unsigned long) x ) );
  }
#endif


  template<>
  inline QString tostring( const string & x )
  {
    return( x.c_str() );
  }


  template<>
  inline QString tostring( const bool & x )
  {
    if( x )
      return( "true" );
    return( "false" );
  }


  template<>
  inline QString tostring( const Object & x )
  {
    ostringstream	s;
    PythonWriter	pw;
    pw.attach( s );
    pw.setSingleLineMode( true );
    pw.write( *x, false, false );
    return s.str().c_str();
  }


  template<typename T> 
  Q3ListViewItem* 
  genericHelper( const GenericObject & obj, Q3ListViewItem* parent, 
		 QPythonPrinter & p, const QString & attname, 
		 const QString & attype, bool )
  {
    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    item->setText( p.typeColumn(), attype );

    try
      {
	// using GenericObject:: avoids a bug in gcc-2.96
        const T	& x = obj.GenericObject::value<T>();
        item->setText( p.valueColumn(), tostring( x ) );
      }
    catch( exception & e )
      {
	item->setText( p.valueColumn(), QString(  "<error: " ) + e.what() 
		       + ">" );
      }
    return( item );
  }


  template<> 
  Q3ListViewItem* 
  genericHelper<string>( const GenericObject & obj, Q3ListViewItem* parent, 
                         QPythonPrinter & p, const QString & attname,
                         const QString & attype, bool )
  {
    Q3ListViewItem      *item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    item->setText( p.typeColumn(), attype );

    try
      {
        // using GenericObject:: avoids a bug in gcc-2.96
        string x = obj.getString();
        item->setText( p.valueColumn(), tostring( x ) );
      }
    catch( exception & e )
      {
        item->setText( p.valueColumn(), QString(  "<error: " ) + e.what() 
                       + ">" );
      }
    return( item );
  }


  template<typename T> 
  Q3ListViewItem* 
  genericSequenceHelper( const GenericObject & obj, Q3ListViewItem* parent, 
			 QPythonPrinter & p, const QString & attname, 
			 const QString & attype, bool )
  {
    QString			str;
    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    item->setText( p.typeColumn(), attype );

    try
      {
	// using GenericObject:: avoids a bug in gcc-2.96
	const T				& x = obj.GenericObject::value<T>();
	typename T::const_iterator	ix;
	typename T::const_iterator	ex = x.end();
	bool				first = true;

	str = "[ ";
	for( ix=x.begin(); ix!=ex; ++ix )
	  {
	    if( first )
	      {
		first = false;
		str += ' ';
	      }
	    else
	      str += ", ";
            try
              {
                str += tostring( *ix );
              }
            catch( exception & e )
              {
                str += QString( "<error: " ) + e.what() + ">";
              }
	  }
	str += " ]";
      }
    catch( exception & e )
      {
	str = QString( "<error: " ) + e.what() + ">";
      }
    item->setText( p.valueColumn(), str );
    return( item );
  }


  template<typename T> 
  Q3ListViewItem* dictHelper( const GenericObject & obj, 
			     Q3ListViewItem* parent, QPythonPrinter & p, 
			     const QString & attname, 
			     const QString & attype, bool writeInternals )
  {
    typedef map<T, Object> U;
    const U			& x = obj.GenericObject::value<U>();
    typename U::const_iterator	im, em = x.end();

    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    if( attype == QString::null )
      item->setText( p.typeColumn(), "dictionary" );
    else
      item->setText( p.typeColumn(), attype );

    for( im=x.begin(); im!=em; ++im )
      {
        stringstream	id;
        PythonWriter	pw;
        pw.attach( id );
        pw.write( *Object::value( im->first ), writeInternals, false );

        p.write( *im->second, item, "", id.str(), writeInternals );
      }
    return( item );
  }


  template<> 
  Q3ListViewItem* 
  dictHelper<string>( const GenericObject & obj, 
                      Q3ListViewItem* parent, QPythonPrinter & p, 
                      const QString & attname, 
                      const QString & attype, bool writeInternals )
  {
    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    if( attype == QString::null )
      item->setText( p.typeColumn(), "dictionary" );
    else
      item->setText( p.typeColumn(), attype );

    Object	im;
    for( im=obj.objectIterator(); im->isValid(); im->next() )
      if( writeInternals || !p.isInternal( "", im->key() ) )
	p.write( *im->currentValue(), item, "", im->key(), writeInternals );

    return( item );
  }


  template<>
  Q3ListViewItem* 
  dictHelper<Object>( const GenericObject & obj, 
                      Q3ListViewItem* parent, QPythonPrinter & p, 
                      const QString & attname, 
                      const QString & attype, bool writeInternals )
  {
    typedef map<Object, Object> U;
    const U		& x = obj.GenericObject::value<U>();
    U::const_iterator	im, em = x.end();

    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    if( attype == QString::null )
      item->setText( p.typeColumn(), "dictionary" );
    else
      item->setText( p.typeColumn(), attype );

    for( im=x.begin(); im!=em; ++im )
      {
        stringstream	id;
        PythonWriter	pw;
        pw.attach( id );
        pw.write( *im->first, writeInternals, false );

        p.write( *im->second, item, "", id.str(), writeInternals );
      }
    return( item );
  }


  Q3ListViewItem* listHelper( const GenericObject & obj, 
			     Q3ListViewItem* parent, 
			     QPythonPrinter & p, const QString & attname, 
			     const QString & attype, bool writeInternals )
  {
    const ObjectVector		& x = obj.value<ObjectVector>();
    ObjectVector::const_iterator	im, em = x.end();

    Q3ListViewItem	*item = new Q3ListViewItem( parent );
    item->setText( p.attributeColumn(), attname );
    if( attype == QString::null )
      item->setText( p.typeColumn(), "list" );
    else
      item->setText( p.typeColumn(), attype );

    unsigned        i = 0, 
        n = x.empty() ? 1 : (unsigned) ceil( log10( (float) x.size() + 1 ) );
    for( im=x.begin(); im!=em; ++im, ++i )
    {
      ostringstream   ss;
      ss << setfill( '0' ) << setw( n ) << i;
      p.write( **im, item, "", ss.str(), writeInternals );
    }
    return( item );
  }

}

void QPythonPrinter::initHelpers()
{
  if( _helpers.find( "double" ) == _helpers.end() )
    _helpers[ "double" ] = &genericHelper<double>;
  {
    ValueObject<double>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<double>;
  }
  if( _helpers.find( "float" ) == _helpers.end() )
    _helpers[ "float" ] = &genericHelper<float>;
  {
    ValueObject<float>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<float>;
  }
  if( _helpers.find( "int" ) == _helpers.end() )
    _helpers[ "int" ] = &genericHelper<int>;
  {
    ValueObject<int>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<int>;
  }
  if( _helpers.find( "uint" ) == _helpers.end() )
    _helpers[ "uint" ] = &genericHelper<unsigned>;
  {
    ValueObject<unsigned>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned>;
  }
  if( _helpers.find( "short" ) == _helpers.end() )
    _helpers[ "short" ] = &genericHelper<short>;
  {
    ValueObject<short>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<short>;
  }
  if( _helpers.find( "ushort" ) == _helpers.end() )
    _helpers[ "ushort" ] = &genericHelper<unsigned short>;
  {
    ValueObject<unsigned short>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned short>;
  }
  if( _helpers.find( "long" ) == _helpers.end() )
    _helpers[ "long" ] = &genericHelper<long>;
  {
    ValueObject<long>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<long>;
  }
  if( _helpers.find( "ulong" ) == _helpers.end() )
    _helpers[ "ulong" ] = &genericHelper<unsigned long>;
  {
    ValueObject<unsigned long>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned long>;
  }
  if( _helpers.find( "longlong" ) == _helpers.end() )
    _helpers[ "longlong" ] = &genericHelper<long long>;
  {
    ValueObject<long long>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<long long>;
  }
  if( _helpers.find( "ulonglong" ) == _helpers.end() )
    _helpers[ "ulonglong" ] 
      = &genericHelper<unsigned long long>;
  {
    ValueObject<unsigned long long>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned long long>;
  }
  if( _helpers.find( "char" ) == _helpers.end() )
    _helpers[ "char" ] = &genericHelper<char>;
  {
    ValueObject<char>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<char>;
  }
  if( _helpers.find( "uchar" ) == _helpers.end() )
    _helpers[ "uchar" ] = &genericHelper<unsigned char>;
  {
    ValueObject<unsigned char>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned char>;
  }
  if( _helpers.find( "schar" ) == _helpers.end() )
    _helpers[ "schar" ] = &genericHelper<signed char>;
  {
    ValueObject<signed char>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<signed char>;
  }
  if( _helpers.find( "bool" ) == _helpers.end() )
    _helpers[ "bool" ] = &genericHelper<bool>;
  {
    ValueObject<bool>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<bool>;
  }
  if( _helpers.find( "string" ) == _helpers.end() )
    _helpers[ "string" ] = &genericHelper<string>;
  {
    ValueObject<string>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<string>;
  }
  if( _helpers.find( "dictionary" ) == _helpers.end() )
    _helpers[ "dictionary" ] = &dictHelper<string>;
  {
    ValueObject<Dictionary>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<string>;
  }
  {
    ValueObject<PropertySet>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<string>;
  }
  {
    ValueObject<map<int, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<int>;
  }
  {
    ValueObject<map<float, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<float>;
  }
  {
    ValueObject<map<Object, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<Object>;
  }
  if( _helpers.find( "list" ) == _helpers.end() )
    _helpers[ "list" ] = &listHelper;
  {
    ValueObject<ObjectVector>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &listHelper;
  }
  if( _helpers.find( "int_vector" ) == _helpers.end() )
    _helpers[ "int_vector" ] = &genericSequenceHelper<vector<int> >;
  {
    ValueObject<vector<int> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericSequenceHelper<vector<int> >;
  }
  if( _helpers.find( "float_vector" ) == _helpers.end() )
    _helpers[ "float_vector" ] = &genericSequenceHelper<vector<float> >;
  {
    ValueObject<vector<float> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericSequenceHelper<vector<float> >;
  }
  if( _helpers.find( "string_vector" ) == _helpers.end() )
    _helpers[ "string_vector" ] = &genericSequenceHelper<vector<string> >;
  {
    ValueObject<vector<string> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericSequenceHelper<vector<string> >;
  }
}


Q3ListViewItem* QPythonPrinter::write( const GenericObject & object, 
				      bool writeInternals )
{
  Q3ListViewItem	*item;
  if( _lview )
    item = new Q3ListViewItem( _lview, "Python object" );
  else
    item = _lvitem; // new Q3ListViewItem( _lvitem );
  write( object, item, "", "attributes", writeInternals );
  return( item );
}


void QPythonPrinter::write( const GenericObject & object, 
			    Q3ListViewItem* parent, const string & syntax, 
			    const string & semantic, bool writeInternals )
{
  string	type = object.type();
  bool		spectype = false;

  //	try to find type in syntax if semantic / syntaxic atts are provided
  if( !semantic.empty() )
    {
      SyntaxSet::const_iterator	is;
      if( syntax.empty() )
	is = _rules.find( "generic" );
      else
	is = _rules.find( syntax );
      if( is != _rules.end() )
	{
	  Syntax::const_iterator	ie = is->second.find( semantic );
	  if( ie != is->second.end() )
	    {
	      /*cout << "syntax found, syntax: " << syntax << ", semantic: " 
		<< semantic << " -> " << type << endl;*/
	      type = ie->second.type;
	      spectype = true;
	    }
	}
    }

  //	try to find type in helpers
  HelperSet::const_iterator	ih = _helpers.find( type );
  if( ih != _helpers.end() )
    {
      QString	t = QString::null;
      if( spectype )
	t = type.c_str();
      else
	{
	  static	map<string, QString>	ttrans;
	  if( ttrans.empty() )
	    {
	      {
		ValueObject<double>	x;
		ttrans[ x.type() ] = "double";
	      }
	      {
		ValueObject<float>	x;
		ttrans[ x.type() ] = "float";
	      }
	      {
		ValueObject<int>	x;
		ttrans[ x.type() ] = "int";
	      }
	      {
		ValueObject<unsigned>	x;
		ttrans[ x.type() ] = "uint";
	      }
	      {
		ValueObject<short>	x;
		ttrans[ x.type() ] = "short";
	      }
	      {
		ValueObject<unsigned short>	x;
		ttrans[ x.type() ] = "ushort";
	      }
	      {
		ValueObject<char>	x;
		ttrans[ x.type() ] = "char";
	      }
	      {
		ValueObject<unsigned char>	x;
		ttrans[ x.type() ] = "uchar";
	      }
	      {
		ValueObject<bool>	x;
		ttrans[ x.type() ] = "bool";
	      }
	      {
		ValueObject<string>	x;
		ttrans[ x.type() ] = "string";
	      }
	      {
		ValueObject<vector<int> >	x;
		ttrans[ x.type() ] = "int_vector";
	      }
	      {
		ValueObject<vector<float> >	x;
		ttrans[ x.type() ] = "float_vector";
	      }
	      {
		ValueObject<vector<string> >	x;
		ttrans[ x.type() ] = "string_vector";
	      }
	    }
	  map<string,QString>::iterator	it = ttrans.find( type );
	  if( it != ttrans.end() )
	    t = it->second;
	}
      (ih->second)( object, parent, *this, semantic.c_str(), t, 
		    writeInternals );
      return;	// OK
    }

  // no helper found
  if( object.isScalar() )
    type = "double";
  else if( object.isDictionary() )
    type = "dictionary";
  else if( object.isIterable() )
    type = "list";
  else if( object.isString() )
    type = "string";

  ih = _helpers.find( type );
  if( ih != _helpers.end() )
  {
    (ih->second)( object, parent, *this, semantic.c_str(), type.c_str(), writeInternals );
    return; // OK
  }

  // really no helper at all
  Q3ListViewItem	*item = new Q3ListViewItem( parent );
  item->setText( attributeColumn(), semantic.c_str() );
  if( spectype )
    item->setText( typeColumn(), type.c_str() );
  item->setText( valueColumn(), "<can't print required data type>" );
}


bool QPythonPrinter::isInternal( const string & syntax, 
				 const string & semantic )
{
  if( !semantic.empty() )
    {
      SyntaxSet::const_iterator	is;
      if( syntax.empty() )
	is = _rules.find( "generic" );
      else
	is = _rules.find( syntax );
      if( is != _rules.end() )
	{
	  Syntax::const_iterator	ie = is->second.find( semantic );
	  if( ie != is->second.end() 
	      && ie->second.internal )
	    return( true );
	}
    }
  return( false );
}
