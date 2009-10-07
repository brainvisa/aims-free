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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/object/awriter.h>
#include <cartobase/object/attributed.h>
#include <cartobase/object/achecker.h>
#include <cartobase/exception/file.h>
#include <cartobase/object/pythonwriter.h>
//#include <cartobase/io/minfXML2.h> // not ready yet
#include <vector>
#include <iomanip>
#include <assert.h>

using namespace carto;
using namespace std;

//=============================================================================
//	F U N C T I O N S
//=============================================================================

namespace
{

  void
  intHelper(const GenericObject& object,
            const string& semantic, ostream& os)
  {
    int i;
    try
    {
      i = object.getProperty(semantic)->GenericObject::value<int>();
    }
    catch( ... )
    {
      // fallback with conversion to double
      i = (int) object.getProperty(semantic)->getScalar();
    }
    os << i;
  }


  void
  floatHelper(const GenericObject& object,
              const string& semantic, ostream& os)
  {
    float f;
    try
    {
      f = object.getProperty(semantic)->GenericObject::value<float>();
    }
    catch( ... )
    {
      // fallback with conversion to double
      f = (float) object.getProperty(semantic)->getScalar();
    }
    os << f;
  }


  void
  stringHelper(const GenericObject& object,
               const string& semantic, ostream& os)
  {
    string s 
        = object.getProperty(semantic)->getString();
    os << s;
  }


  void
  int_vectorHelper(const GenericObject& object,
                   const string& semantic, ostream& os)
  {
    Object	o = object.getProperty( semantic );
    Object	i = o->objectIterator();
    if( i->isValid() )
      {
        int x;
        try
        {
          x = i->currentValue()->GenericObject::value<int>();
        }
        catch( ... )
        {
          x = (int) i->currentValue()->getScalar();
        }
        os << x;
        for( i->next(); i->isValid(); i->next() )
        {
          try
          {
            x = i->currentValue()->GenericObject::value<int>();
          }
          catch( ... )
          {
            x = (int) i->currentValue()->getScalar();
          }
          os << ' ' << x;
        }
      }
  }


  void
  float_vectorHelper(const GenericObject& object,
                     const string& semantic, ostream& os)
  {
    Object	o = object.getProperty( semantic );
    Object	i = o->objectIterator();
    float x;
    try
    {
      x = i->currentValue()->GenericObject::value<float>();
    }
    catch( ... )
    {
      x = (float) i->currentValue()->getScalar();
    }
    os << x;
    for( i->next(); i->isValid(); i->next() )
    {
      try
      {
        x = i->currentValue()->GenericObject::value<float>();
      }
      catch( ... )
      {
        x = (float) i->currentValue()->getScalar();
      }
      os << ' ' << x;
    }
  }


  void
  genericobjectHelper( const GenericObject& object,
                       const string& semantic, ostream& os )
  {
    Object	o = object.getProperty( semantic );
    PythonWriter	pw;
    pw.attach( os );
    pw.write( o, false, false );
  }

}

//=============================================================================
//	M E T H O D S
//=============================================================================

AttributedWriter::AttributedWriter(const string& filename,
			 const SyntaxSet& rules,
			 const HelperSet& helpers)
	: _rules(rules),
	  _stream( new ofstream( filename.c_str() ) ), 
	  _ownStream( true ), 
	  _filename(filename),
	  _helpers(helpers)
{
  // are files opened?
  if (!*_stream) 
    throw file_error( _filename );

  init();
}


AttributedWriter::AttributedWriter(const SyntaxSet& rules,
				   const HelperSet& helpers)
	: _rules(rules), 
	  _stream( 0 ), 
	  _ownStream( false ), 
	  _helpers(helpers)
{
	init();
}


void AttributedWriter::init()
{
	if (_helpers.find("int") == _helpers.end())
		_helpers["int"] = &intHelper;
	if (_helpers.find("float") == _helpers.end())
		_helpers["float"] = &floatHelper;
	if (_helpers.find("string") == _helpers.end())
		_helpers["string"] = &stringHelper;
	if (_helpers.find("int_vector") == _helpers.end())
		_helpers["int_vector"] = &int_vectorHelper;
	if (_helpers.find("float_vector") == _helpers.end())
		_helpers["float_vector"] = &float_vectorHelper;
        if (_helpers.find("dictionary") == _helpers.end())
		_helpers["dictionary"] = &genericobjectHelper;
}


AttributedWriter::~AttributedWriter()
{
  close();
}


void
AttributedWriter::open(const string& filename)
{
  close();
  _stream = new ofstream( filename.c_str() );
  _ownStream = true;
  _filename = filename;

  // are files opened?
  if (!_stream) 
    throw file_error(_filename);
}


void
AttributedWriter::close()
{
  _filename = "";
  if( _ownStream )
    delete _stream;
  _stream = 0;
}


void AttributedWriter::attach( ostream & s )
{
  close();
  _stream = &s;
  _ownStream = false;
}


string
AttributedWriter::name() const
{
	return _filename;
}


bool
AttributedWriter::operator!() const
{
  return !*_stream;
}


bool
AttributedWriter::is_open() const
{
  if( !_stream )
    return( false );
  ofstream	*fs = dynamic_cast<ofstream *>( _stream );
  if( fs )
    return fs->is_open();
  return( true );	// non-file streams are considered always open
}


void
AttributedWriter::writeAttributes(const GenericObject& object)
{
  const SyntaxedInterface	*si = object.getInterface<SyntaxedInterface>();
  if( !si || !si->hasSyntax() )
    throw range_error( "cannot write a GenericObject with no syntax" );


  // get the syntactic attribute
  SyntaxSet::const_iterator syntax = _rules.find( si->getSyntax() );
  if (syntax == _rules.end())
    {
      string s("unknown syntactic attribute ");
      s+= si->getSyntax();
      throw range_error(s.c_str());
    }

  // make sure all mandatory semantic attributes are here
  AttributedChecker checker(_rules);
  set<string> tmp = checker.check(object);
  if (!tmp.empty())
    {
      Object it = object.objectIterator();
      while( it->isValid() )
        {
          cout << "prop: " << it->key() << endl;
          it->next();
        }
      string s("missing semantic attribute");
      if( tmp.size() > 1 )
        s += "s (";
      set<string>::iterator	i, e = tmp.end();
      for( i=tmp.begin(); i!=e; ++i )
        s += string( " " ) + *i;
      if( tmp.size() > 1 )
        s += " )";
      s += string( " in " ) + si->getSyntax();
      throw range_error(s.c_str());
    }

  // separate mandatory and optional attributes
  set<string> set1;
  set<string> set2;
  string::size_type size = 0, asz;
  Object	a;
  for( a=object.objectIterator(); a->isValid(); a->next() )
    {
      map<string,Semantic>::const_iterator tmp = syntax->second.find(a->key());
      map<string,Semantic>::const_iterator tmp2;
      if (tmp == syntax->second.end())
        tmp2 = syntax->second.find("__fallback__");
      else tmp2 = tmp;
      if (tmp2 != syntax->second.end())
	{
          asz = a->key().size();
	  if (asz > size)
	    size = asz;
	  if (tmp->second.needed)
	    set1.insert(a->key());
	  else
	    set2.insert(a->key());
	}
    }

  // first write mandatory attributes...
  writeAttributes(object, set1, size);

  // ...then write optional attributes
  writeAttributes(object, set2, size);

}


void
AttributedWriter::writeAttributes(const GenericObject& object,
                                  const set<string>& attributes, 
                                  string::size_type justify)
{
  const SyntaxedInterface	*si = object.getInterface<SyntaxedInterface>();
  if( !si || !si->hasSyntax() )
    throw range_error( "cannot write a GenericObject with no syntax" );

  // if this semantic attribute does have associated I/O properties...
  SyntaxSet::const_iterator syntax = _rules.find( si->getSyntax() );
  assert(syntax != _rules.end());


  for (set<string>::const_iterator s = attributes.begin();
       s != attributes.end(); ++s)
    {
      std::string	type;


      _stream->setf(ios::left);
      (*_stream) << setw(justify) << s->c_str() << ' ';
      _stream->unsetf(ios::left);

      SemanticSet::const_iterator p = syntax->second.find(*s);
      SemanticSet::const_iterator p2;
      if (p == syntax->second.end())
	     p2 = syntax->second.find("__fallback__");
      else p2 = p;
      assert(p2 != syntax->second.end());
      type = p2->second.type;
      HelperSet::const_iterator h = _helpers.find(type);
      if (h != _helpers.end() && h->second != 0)
        {
          (h->second)(object, *s, *_stream);
          (*_stream) << endl;
          continue;
        }
      else if( object.isScalar() )
        type = "double";
      if( object.isString() )
        type = "string";
      else if( object.isDictionary() )
        type = "dictionary";
      else if( object.isIterable() )
        type = "list";

      h = _helpers.find(type);
      if (h != _helpers.end())
        {
          (h->second)(object, *s, *_stream);
          (*_stream) << endl;
        }
      else
        // no helper found
        throw runtime_error( string( "Can't write data of type " ) + type
                       + " in " + name() );
    }
}


void
AttributedWriter::writeAttributes(const GenericObject* object)
{
	assert(object != 0);
	writeAttributes(*object);
}


void
AttributedWriter::writeAttributes(const GenericObject* object,
                                  const set<string>& attributes, 
                                  string::size_type size)
{
	assert(object != 0);
	writeAttributes(*object, attributes, size);
}
