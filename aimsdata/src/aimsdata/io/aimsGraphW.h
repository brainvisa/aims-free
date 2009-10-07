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

    /**	Saves elements contained in the graph. Elements must be referred by the
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
