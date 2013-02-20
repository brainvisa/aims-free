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

#ifndef CARTOBASE_IO_MINFXML2_H
#define CARTOBASE_IO_MINFXML2_H

#include <cartobase/object/object.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/object/syntax.h>

namespace carto {


//-----------------------------------------------------------------------------
// XML tags and attributes
//-----------------------------------------------------------------------------


extern const std::string minfTag;
extern const std::string noneTag;
extern const std::string trueTag;
extern const std::string falseTag;
extern const std::string numberTag;
extern const std::string stringTag;
extern const std::string listTag;
extern const std::string dictionaryTag;
extern const std::string factoryTag;
extern const std::string xhtmlTag;
extern const std::string referenceTag;

extern const std::string expanderAttribute;
extern const std::string lengthAttribute;
extern const std::string nameAttribute;
extern const std::string objectTypeAttribute;
extern const std::string identifierAttribute;


//-----------------------------------------------------------------------------
Object readDictionaryMinfXML( const std::string &fileName, 
                              rc_ptr<SyntaxSet> syntax = rc_ptr<SyntaxSet>() );
Object readDictionaryMinfXML( DataSource &source, 
                              rc_ptr<SyntaxSet> syntax = rc_ptr<SyntaxSet>() );
void readDictionaryMinfXML( const std::string &fileName, Object &dictionary, 
                            rc_ptr<SyntaxSet> syntax = rc_ptr<SyntaxSet>() );
void readDictionaryMinfXML( DataSource &source, Object &dictionary, 
                            rc_ptr<SyntaxSet> syntax = rc_ptr<SyntaxSet>() );
/** just to force initializing libxml2, needs to be called once from the
    main thread in a multi-threaded environment. Called from PluginLoader::load
*/
void init_libXML();


//-----------------------------------------------------------------------------
class ObjectListener : public virtual RCObject
{
public:
  virtual ~ObjectListener();

  virtual void nextObject( const Object & ) = 0;
  virtual void noMoreObject() = 0;
};


//-----------------------------------------------------------------------------
class MinfTreeExpander;
class MinfNodeExpander : public virtual RCObject
{
public:
  virtual ~MinfNodeExpander();

  virtual std::string startStructure( MinfTreeExpander &,
                               const std::string &nodeType,
                               const std::string &identifier,
                               Object attributes,
                               const std::string &parentSyntax=std::string(), 
                               const std::string &name=std::string() ) = 0;
  virtual void endStructure( MinfTreeExpander &, const std::string &nodeType ) = 0;
  virtual void atom( MinfTreeExpander &, Object, const std::string &identifier  ) = 0;
  virtual void reference( MinfTreeExpander &, const std::string &identifier ) = 0;
};


//-----------------------------------------------------------------------------
class MinfTreeExpander : public virtual RCObject
{
public:
  MinfTreeExpander();
  virtual ~MinfTreeExpander();

  void push( rc_ptr< MinfNodeExpander > );
  rc_ptr< MinfNodeExpander > pop();

  void addReference( const std::string &identifier, Object value );
  Object getReferencedObject( const std::string &identifier );

  virtual std::string startStructure( const std::string &nodeType,
                               const std::string &identifier,
                               Object attributes,
                               const std::string &parentSyntax=std::string(), 
                               const std::string &name=std::string() );
  virtual void endStructure( const std::string &nodeType );
  virtual void atom( Object, const std::string &identifier  );
  virtual void reference( const std::string &identifier );

private:
  std::list< rc_ptr< MinfNodeExpander > > _stack;
  std::map< std::string, Object > _references;
};


//-----------------------------------------------------------------------------
class DefaultMinfNodeExpander : public MinfNodeExpander
{
public:
  DefaultMinfNodeExpander( rc_ptr< ObjectListener > ol = rc_ptr< ObjectListener >(),
                           rc_ptr<SyntaxSet> syntax = rc_ptr<SyntaxSet>() );
  DefaultMinfNodeExpander( rc_ptr<SyntaxSet> syntax );
  virtual ~DefaultMinfNodeExpander();

  virtual std::string startStructure( MinfTreeExpander &,
                               const std::string &nodeType,
                               const std::string &identifier,
                               Object attributes,
                               const std::string &parentSyntax=std::string(), 
                               const std::string &name=std::string() );
  virtual void endStructure( MinfTreeExpander &, const std::string &nodeType );
  virtual void atom( MinfTreeExpander &, Object, const std::string &identifier  );
  virtual void reference( MinfTreeExpander &, const std::string &identifier );

private:

  rc_ptr< ObjectListener > _objectListener;

protected:

  rc_ptr<SyntaxSet> _syntax;
};


//-----------------------------------------------------------------------------
class SingleDictionaryExpander : public DefaultMinfNodeExpander
{
public:
  SingleDictionaryExpander( Object dictionary, rc_ptr<SyntaxSet> syntax );
  virtual ~SingleDictionaryExpander();

  virtual std::string startStructure( MinfTreeExpander &,
                               const std::string &nodeType,
                               const std::string &identifier,
                               Object attributes,
                               const std::string &parentSyntax=std::string(), 
                               const std::string &name=std::string() );

private:

  Object _dictionary;
};


//-----------------------------------------------------------------------------
class ListBuilder : public ObjectListener
{
public:
  ListBuilder();
  ListBuilder( Object );
  ListBuilder( rc_ptr< ObjectListener > );
  ListBuilder( Object, rc_ptr< ObjectListener > );
  virtual ~ListBuilder();

  virtual void nextObject( const Object & );
  virtual void noMoreObject();

  Object result;
private:

  rc_ptr< ObjectListener > _objectListener;
};


//-----------------------------------------------------------------------------
class DictionaryBuilder : public ObjectListener
{
public:
  DictionaryBuilder();
  DictionaryBuilder( Object );
  DictionaryBuilder( rc_ptr< ObjectListener > );
  DictionaryBuilder( Object, rc_ptr< ObjectListener > );
  virtual ~DictionaryBuilder();

  virtual void nextObject( const Object & );
  virtual void noMoreObject();

  Object result;

private:

  Object _key;
  rc_ptr< ObjectListener > _objectListener;
};



//-----------------------------------------------------------------------------
class MinfXMLReader : public virtual RCObject
{
public:
  MinfXMLReader( MinfTreeExpander &, rc_ptr<SyntaxSet> syntax );
  void read( const std::string & );
  void read( DataSource & );

private:

  MinfTreeExpander &_expander;
  rc_ptr< SyntaxSet > _syntax;
};


//-----------------------------------------------------------------------------
// class TestMinfTreeExpander : public virtual MinfNodeExpander
// {
// public:
//   TestMinfTreeExpander();
//   virtual ~TestMinfTreeExpander();
// 
//   virtual void startStructure( MinfTreeExpander &,
//                                const std::string &nodeType,
//                                const std::string &identifier,
//                                Object attributes );
//   virtual void endStructure( MinfTreeExpander &, const std::string &nodeType );
//   virtual void atom( MinfTreeExpander &, Object, const std::string &identifier  );
//   virtual void reference( MinfTreeExpander &, const std::string &identifier );
// 
// private:
// 
//   int _level;
// };


#endif // ifndef CARTOBASE_IO_MINFXML2_H

} // namespace carto
