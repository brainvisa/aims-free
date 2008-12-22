
class Tree : carto::GenericObject
{
%TypeHeaderCode
#include <graph/tree/tree.h>
%End

public:
  Tree( bool = true, const std::string & = "" );
  virtual ~Tree();

public:
  virtual std::string type() const ;
  virtual size_t size() const ;
%MethodCode
  sipRes = sipCpp->carto::AttributedObject::size();
%End
  virtual double getScalar() const ;
  virtual std::string getString() const ;
  virtual bool isArray() const ;
  virtual bool isDynArray() const ;
  virtual bool isDictionary() const ;
  virtual bool isIterable() const ;
  virtual carto::Object objectIterator() const ;
  virtual bool hasProperty( const std::string & ) const  /PyName=has_key/;
  virtual bool isIterator() const ;
  virtual bool isDictionaryIterator() const ;
  virtual bool isValid() const ;
  virtual carto::Object currentValue() const;
  virtual std::string key() const;

  void insert( Tree* child /Transfer/, int index=-1 );
  void remove( unsigned index );
  void remove( Tree* node /TransferBack/ );
  // void removeFromParent();
  virtual size_t childrenSize() const;
  SIP_PYTUPLE children() const;
%MethodCode
  PyObject *l = PyTuple_New( sipCpp->childrenSize() );
  Tree::const_iterator i, e = sipCpp->end();
  int			c = 0;
  Tree                  *t;
  for( i=sipCpp->begin(); i!=e; ++i, ++c )
  {
    t = dynamic_cast<Tree *>( *i );
    if( t )
#if SIP_VERSION >= 0x040400
      PyTuple_SetItem( l, c, sipConvertFromInstance( t, sipClass_Tree, 0 ) );
#else
      PyTuple_SetItem( l, c, sipMapCppToSelfSubClass( t, sipClass_Tree ) );
#endif
  }
  sipRes = l;
%End

private:
  virtual carto::GenericObject* clone() const;
};


class TreeReader
{
%TypeHeaderCode
#include <graph/tree/treader.h>
#include <pyaims/object/object.h>
%End

public:
  TreeReader( const std::string &, const carto::PySyntaxSet & );
  virtual ~TreeReader();
  virtual Tree* read();
%MethodCode
  try
  {
    sipRes = sipCpp->read();
  }
  catch( std::exception & e )
  {
    sipIsErr = 1;
    sipRes = 0;
    PyErr_SetString( PyExc_IOError, e.what() );
  }
%End
  virtual std::string name() const;

private:
  TreeReader( const TreeReader & );
};

