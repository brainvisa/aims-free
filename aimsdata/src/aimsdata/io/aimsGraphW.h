/*
 *  Region Of Interest
 */
#ifndef AIMS_IO_AIMSGRAPHW_H
#define AIMS_IO_AIMSGRAPHW_H

#include <aims/io/process.h>
#include <cartobase/object/attributed.h>

class Graph;

namespace aims
{
  struct AimsGraphWriter_Private;

  namespace internal
  {
    struct AimsGraphWriter_ElemStruct;
  }


  class AimsGraphWriter : public Process
  {
  public:
    enum SavingMode
    {
      Keep, 
      Global, 
      Local
    };

    struct Wrapper
    {
      friend class AimsGraphWriter;

      virtual ~Wrapper() {}
      virtual void destroy() {}

      virtual std::string objectType() const = 0;
      virtual std::string dataType() const = 0;

    private:
      virtual int freeindex() const = 0;
      virtual bool write( const AimsGraphWriter & ) const = 0;
    };

    template<typename T> struct ObjectWrapper : public Wrapper
    {
      friend class AimsGraphWriter;

      ObjectWrapper( T* x ) : data( x ) {}
      virtual ~ObjectWrapper() {}
      virtual void destroy() { delete data; }

      virtual std::string objectType() const;
      virtual std::string dataType() const;

      T	*data;

    private:
      virtual int freeindex() const;
      virtual bool write( const AimsGraphWriter & ) const;
    };

    struct ElementInfo
    {
      Graph			*graph;
      carto::AttributedObject	*element;
      std::string		attribute;
      Wrapper			*object;
    };

    template<typename T> friend struct ObjectWrapper;

    AimsGraphWriter( const std::string & filename );
    virtual ~AimsGraphWriter();

    /** Saves elements contained in the graph. Elements must be referred by the
        aims_objects_table map. Input graph \c g is not const since some of
        the graph attributed are updated. Hence you must always call
        writeElements() \e before saving the graph structure (using a regular
        GraphWriter)
        \todo save only some objects
        \todo save only modified objects
        \par newmode saving mode of "new" objects (those created after loading)
        \par oldmode saving mode of already existing (loaded) objects
        \par saveOnlyModified : if true, saves only objects in graph elements
        with a modification flag (attribute "modified" set to true)
    */
    virtual void writeElements( Graph & g, SavingMode newmode = Keep, 
                                SavingMode oldmode = Keep,
                                bool saveOnlyModified = false );

    template<class T> 
    static bool defaultTakeObject( Process &, const std::string &, Finder & );
    template <typename T> static bool isEmpty( const T & obj );

    const ElementInfo & elementInfo() const { return( info ); }
    ElementInfo & elementInfo() { return( info ); }

  private:
    template<class T> bool write( const T & ) const;
    void writeElement( carto::AttributedObject *ao, 
		       internal::AimsGraphWriter_ElemStruct & es );

    mutable ElementInfo			info;
    mutable AimsGraphWriter_Private	*d;
  };

}


#endif
