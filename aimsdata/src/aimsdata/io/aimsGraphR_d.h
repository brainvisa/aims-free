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
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
	  &defaultInsertFunction<T> );
#else
	  &AimsGraphReader::defaultInsertFunction<T> );
#endif

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
