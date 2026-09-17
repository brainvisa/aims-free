/*
 *  Region Of Interest
 */
#ifndef AIMS_IO_AIMSGRAPHR_D_H
#define AIMS_IO_AIMSGRAPHR_D_H

#include <aims/io/aimsGraphR.h>
#include <aims/io/reader.h>
#include <aims/io/datatypecode.h>
#include <aims/io/finder.h>
#include <aims/graph/graphmanip.h>
#include <cartobase/smart/rcptr.h>

namespace aims
{

  namespace internal
  {

  struct AimsGraphReader_Private
  {
    friend class aims::AimsGraphReader;

  private:
    static void 
    loadGlobalObjects( std::map<std::string, aims::AimsGraphReader::Wrapper*> 
		       & objects, 
		       const std::map<std::string, 
		       std::map<std::string, std::string> > & globals, 
		       const std::string & basepath, 
		       std::map<std::string, 
		       std::map<std::string, GraphElementCode> > & objmap, 
		       AimsGraphReader & agr );

    std::string				filename;
    std::set<std::string>		readfilter;
    std::set<std::string>		excludefilter;
    aims::AimsGraphReader::PostProcessor	postproc;
  };

  }

  //	Implementation

  template<typename T> AimsGraphReader::Wrapper * 
  AimsGraphReader::ObjectWrapper<T>::extract( int index )
  {
    typename T::const_iterator	io;
    io = data->find( index );
    T	*obj = new T;
    if( io != data->end() )
      (*obj)[0] = io->second;
    DataManip::adjustVoxelSize( *obj, *data );
    return( new ObjectWrapper<T>( obj ) );
  }


  template<typename T> std::string 
  AimsGraphReader::ObjectWrapper<T>::objectType() const
  {
    return( carto::DataTypeCode<T>().objectType() );
  }


  template<typename T> std::string 
  AimsGraphReader::ObjectWrapper<T>::dataType() const
  {
    return( carto::DataTypeCode<T>().dataType() );
  }


  template<class T> bool 
  AimsGraphReader::read( Process & p, const std::string & fname, Finder & f )
  {
    T		*obj = new T;
    Reader<T>	r( fname );
    try
    {
      r.read( *obj );
    }
    catch( std::exception & e )
    {
      std::cerr << e.what() << std::endl;
      delete obj;
      return false;
    }

    AimsGraphReader			& ap = (AimsGraphReader &) p;
    internal::AimsGraphReader_Private	& ps = *ap._priv;
    ElementInfo				& info = ps.postproc.info;

    // ensure postprocessor has this type registered
    const std::map<std::string, std::map<std::string, ProcFunc> > & pcs 
      = ps.postproc.processTypes();
    std::map<std::string, std::map<std::string, ProcFunc> >::const_iterator 
      ipcs = pcs.find( f.objectType() );
    if( ipcs == pcs.end() 
        || ipcs->second.find( f.dataType() ) == ipcs->second.end() )
      ps.postproc.registerProcessType
        ( f.objectType(), f.dataType(),
          &AimsGraphReader::defaultInsertFunction<T> );

    // also register my own type because Volume/CartoVolume may be mixed
    if( f.objectType() != carto::DataTypeCode<T>::objectType()
        || f.dataType() != carto::DataTypeCode<T>::dataType() )
    {
      ipcs = pcs.find( carto::DataTypeCode<T>::objectType() );
      if( ipcs == pcs.end()
          || ipcs->second.find( carto::DataTypeCode<T>::dataType() )
            == ipcs->second.end() )
        ps.postproc.registerProcessType(
          carto::DataTypeCode<T>::objectType(),
          carto::DataTypeCode<T>::dataType(),
          &AimsGraphReader::defaultInsertFunction<T> );
    }

    info.object = new AimsGraphReader::ObjectWrapper<T>( obj );
    return( true );
  }


  template<class T> bool
  AimsGraphReader::defaultInsertFunction( Process & p, const std::string &, 
                                          Finder & )
  {
    PostProcessor	& pp = (PostProcessor &) p;
    const ElementInfo	& info = pp.elementInfo();
    T			* obj = ((const ObjectWrapper<T> *) info.object)->data;
    if( obj )
      info.element->setProperty( info.attribute, carto::rc_ptr<T>( obj ) );
    return( true );
  }


}


#endif
