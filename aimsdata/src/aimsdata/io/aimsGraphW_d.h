#ifndef AIMS_IO_AIMSGRAPHW_D_H
#define AIMS_IO_AIMSGRAPHW_D_H

#include <aims/io/aimsGraphW.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/writer.h>
#include <aims/io/datatypecode.h>
#include <aims/io/finder.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <list>
#include <vector>

namespace aims
{

  struct AimsGraphWriter_Private
  {
    AimsGraphWriter_Private();
    ~AimsGraphWriter_Private();
    std::string					filename;
    std::string					directory;
    std::string					elemfname;
    int						elemindex;
    GraphElementCode				elemcode;
    AimsGraphWriter::SavingMode			mode;
    AimsGraphWriter::SavingMode			oldmode;
    AimsGraphWriter::SavingMode			newmode;
    AimsGraphWriter::SavingMode			prevmode;
    std::map<std::string,AimsGraphWriter::Wrapper *>	globalobjects;
    std::map<std::string, bool>			globalmodified;
    int						genindexcount;
    bool					delayindex;
    bool					modified;
    bool					saveall;
    carto::rc_ptr<std::map<std::string,std::vector<int> > >	colors;
  };


  AimsGraphWriter_Private::AimsGraphWriter_Private()
    : colors( new std::map<std::string,std::vector<int> > )
  {
  }


  AimsGraphWriter_Private::~AimsGraphWriter_Private()
  {
    std::map<std::string,AimsGraphWriter::Wrapper *>::iterator	i, e 
      = globalobjects.end();
    for ( i=globalobjects.begin(); i!=e; ++i )
      {
        // i->second->destroy(); ? no, object can be kept in graph
        delete i->second;
      }
  }


  template<typename T> bool 
  AimsGraphWriter::ObjectWrapper<T>::write( const AimsGraphWriter & wp ) const
  {
    return wp.write( *data );
  }


  template<typename T> int 
  AimsGraphWriter::ObjectWrapper<T>::freeindex() const
  {
    typename T::const_iterator i = data->begin(), e = data->end();
    int n = 1;
    if( i != e && i->first == 0 )
      n = 0;
    for ( ; i!=e && i->first==n; ++i, ++n ) {}
    return n;
  }


  template<typename T> std::string 
  AimsGraphWriter::ObjectWrapper<T>::objectType() const
  {
    return ::carto::DataTypeCode<T>().objectType();
  }


  template<typename T> std::string 
  AimsGraphWriter::ObjectWrapper<T>::dataType() const
  {
    return carto::DataTypeCode<T>().dataType();
  }


  template <typename T>
  bool AimsGraphWriter::isEmpty( const T & obj )
  {
    return obj.empty();
  }


  template <>
  bool AimsGraphWriter::isEmpty( const carto::Volume<short> & obj )
  {
    return !obj.any();
  }


  template <>
  bool AimsGraphWriter::isEmpty( const carto::Volume<int32_t> & obj )
  {
    return !obj.any();
  }


  template<class T>
  bool AimsGraphWriter::write( const T & obj ) const
  {
    // write in file or in global object
    if ( d->mode == Local )
      {
	if ( d->saveall || ( d->modified && !isEmpty( obj ) ) )
	  {
            std::string dir = carto::FileUtil::dirname( d->elemfname );
            if( carto::FileUtil::fileStat( dir ).find( '+' ) 
                == std::string::npos )
              {
                carto::Directory	cd( dir );
                cd.makedirs();
              }
	    Writer<T>	w( d->elemfname );
	    return w.write( obj );
	  }
	return true;
      }
    else
      {
	Wrapper	*& glob = d->globalobjects[ d->elemcode.global_filename ];
	if ( !glob )
	  {
	    //cout << "create " << d->elemcode.global_filename << endl;
	    carto::rc_ptr<T>	gobject;
	    if ( !d->elemcode.global_attribute.empty() 
                 && info.graph->getProperty( d->elemcode.global_attribute, 
					     gobject ) )
	      glob = new ObjectWrapper<T>( gobject.get() );
	    else
	      {
		glob = new ObjectWrapper<T>( new T );
                T* data = ((ObjectWrapper<T> *) glob)->data;
		DataManip::adjustVoxelSize( *data, obj );
                DataManip::setHeaderInfo( *data, *info.graph );
	      }
	  }
	if( !d->delayindex && d->mode == Global )
	  {
	    /*cout << "insert index " << d->elemindex << " in " 
	      << d->elemcode.global_filename << endl;*/
	    DataManip::insertElement( *((ObjectWrapper<T> *) glob)->data, 
				      d->elemindex, obj );
	  }
      }

    return true;
  }


  template<class T> 
  bool AimsGraphWriter::defaultTakeObject( Process & p, const std::string &, 
                                           Finder & f )
  {
    AimsGraphWriter	& wp = (AimsGraphWriter &) p;
    ElementInfo		& info = wp.info;
    carto::rc_ptr<T>	obj;

    try
    {
      if ( !info.element->getProperty( info.attribute, obj ) )
      {
        carto::Object o = info.element->getProperty( info.attribute );
        return false;
      }
      info.object = new ObjectWrapper<T>( obj.get() );
      return true;
    }
    catch( std::exception & e )
    {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

}

#endif
