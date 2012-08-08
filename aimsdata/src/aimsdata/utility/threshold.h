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
 *  Threshold operators
 */
#ifndef AIMS_UTILITY_THRESHOLD_H
#define AIMS_UTILITY_THRESHOLD_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/mesh/texture.h>


enum threshold_t
{
  AIMS_LOWER_THAN,
  AIMS_LOWER_OR_EQUAL_TO,
  AIMS_GREATER_THAN,
  AIMS_GREATER_OR_EQUAL_TO,
  AIMS_EQUAL_TO,
  AIMS_DIFFER,
  AIMS_BETWEEN,
  AIMS_OUTSIDE, 
  AIMS_BETWEEN_EXCLUDE_LOWER_BOUND, 
  AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND, 
  AIMS_BETWEEN_EXCLUDE_BOUNDS, 
  AIMS_OUTSIDE_INCLUDE_LOWER_BOUND, 
  AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND, 
  AIMS_OUTSIDE_INCLUDE_BOUNDS, 
};

/** The template class to make thresholds.
    The first template argument provides the type of the item.
    The second template argument provides the type of the binary image.
    The programmer can implement single and double thresholds
    This class is useful to do binary or level thresholds. There are
    different types of thresholds :

    - AIMS_LOWER_THAN
    - AIMS_LOWER_OR_EQUAL_TO
    - AIMS_GREATER_THAN
    - AIMS_GREATER_OR_EQUAL_TO
    - AIMS_EQUAL_TO
    - AIMS_DIFFER
    - AIMS_BETWEEN
    - AIMS_OUTSIDE
    - AIMS_BETWEEN_EXCLUDE_LOWER_BOUND
    - AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND
    - AIMS_BETWEEN_EXCLUDE_BOUNDS
    - AIMS_OUTSIDE_INCLUDE_LOWER_BOUND
    - AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND
    - AIMS_OUTSIDE_INCLUDE_BOUNDS

    If bin is set to true, the threshold returns a binary short image
    with levels 0 and 32767.
*/ 
template <class T,class U>
class AimsThreshold
{
public:
  /** Volume thresholding
      \param type threshold type (AIMS_LOWER_THAN,...)
      \param level first threshold
      \param level2 second threshold [default=0]
      \param backgd value for voxels cut out by the thresholding
      \param foregd value for voxels cut in by the thresholding
  */
  inline AimsThreshold( threshold_t type,T level,T level2 = 0, 
                        T backgd = 0, float foregd = 32767  );
  virtual ~AimsThreshold() {}

  /// Return the multi-level thresholded image
  inline AimsData<T> operator () (const AimsData<T> &sqv);
  /// Return the binary thresholded image
  inline AimsData<U> bin(const AimsData<T> &sqv);

protected:
  /// Threshold type
  threshold_t _type;
  /// Lower level or unique level
  T    _level;
  /// Upper level
  T    _level2;
  T    _backgd;
  float    _foregd;
};

template <class T,class U>
class AimsTexThreshold
{
public:
  /** Texture thresholding
      \param type threshold type (AIMS_LOWER_THAN,...)
      \param level first threshold
      \param level2 second threshold [default=0]
      \param backgd value for voxels cut out by the thresholding
  */
  inline AimsTexThreshold( threshold_t type,T level,T level2=0, T backgd = 0 );
  virtual ~AimsTexThreshold() {}

  /// Return the multi-level thresholded texture
  inline TimeTexture<T> operator () (const TimeTexture<T> &sqv);
  /// Return the binary thresholded texture
  inline TimeTexture<U> bin(const TimeTexture<T> &sqv);

protected:
  /// Threshold type
  threshold_t _type;
  /// Lower level or unique level
  T    _level;
  /// Upper level
  T    _level2;
  T    _backgd;
};

template <class T,class U> inline
AimsThreshold<T,U>::AimsThreshold( threshold_t type, T level, T level2, 
                                   T backgd, float foregd )
  : _type( type ), _level( level ), _level2( level2 ), _backgd( backgd ), _foregd( foregd )
{
}

template <class T,class U> inline
AimsTexThreshold<T,U>::AimsTexThreshold( threshold_t type,T level,T level2, 
                                         T backgd )
  : _type( type ), _level( level ), _level2( level2 ), _backgd( backgd )
{
}

template <class T,class U> inline
AimsData<T> AimsThreshold<T,U>::operator () (const AimsData<T> &sqv)
{
  AimsData<T> res(sqv.dimX(),sqv.dimY(),sqv.dimZ(),sqv.dimT(),
                    sqv.borderWidth());
  res.setSizeX(sqv.sizeX());
  res.setSizeY(sqv.sizeY());
  res.setSizeZ(sqv.sizeZ());
  res.setSizeT(sqv.sizeT());

  res = T( 0 );

  typename AimsData<T>::iterator       it1;
  typename AimsData<T>::const_iterator it2;

  switch (_type)
  { case AIMS_LOWER_THAN :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level ? *it2 : _backgd);
      break;
    case AIMS_LOWER_OR_EQUAL_TO :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 <= _level ? *it2 : _backgd);
      break;
    case AIMS_GREATER_THAN :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level ? *it2 : _backgd);
      break;
    case AIMS_GREATER_OR_EQUAL_TO :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level ? *it2 : _backgd);
      break;
    case AIMS_EQUAL_TO :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 == _level ? *it2 : _backgd);
      break;
    case AIMS_DIFFER :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 != _level ? *it2 : _backgd);
      break;
    case AIMS_BETWEEN :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level && *it2 <= _level2 ? *it2 : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level && *it2 <= _level2 ? *it2 : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level && *it2 < _level2 ? *it2 : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level && *it2 < _level2 ? *it2 : _backgd);
      break;
    case AIMS_OUTSIDE :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level || *it2 > _level2 ? *it2 : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 <= _level || *it2 > _level2 ? *it2 : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level || *it2 >= _level2 ? *it2 : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level || *it2 > _level2 ? *it2 : _backgd);
      break;
  }

  if( sqv.header() )
    {
      res.setHeader( sqv.header()->cloneHeader() );
      aims::PythonHeader	*ph 
        = dynamic_cast<aims::PythonHeader *>( res.header() );
      if( ph && ph->hasProperty( "data_type" ) )
        ph->removeProperty( "data_type" );
    }
  return(res);
}


template <class T,class U> inline
TimeTexture<T> AimsTexThreshold<T,U>::operator () (const TimeTexture<T> &sqv)
{

  unsigned i, n = sqv.nItem(), t , tm = sqv.size();
  TimeTexture<T> res = sqv;
 
  switch (_type)
    { case AIMS_LOWER_THAN : 
	for (t = 0; t < tm ; ++t)
	  for (i = 0; i < n ; ++i)
	    res.item( i )  = ( res[t].item( i ) < _level ? res[t].item( i ) 
                               : _backgd);
	break;
    case AIMS_LOWER_OR_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) <= _level ?  res[t].item( i ) 
                             : _backgd);
      break;
    case AIMS_GREATER_THAN : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) > _level ?  res[t].item( i ) 
                             : _backgd);
      break;
    case AIMS_GREATER_OR_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) >= _level ?  res[t].item( i ) 
                             : _backgd);
      break;
    case AIMS_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) == _level ?  res[t].item( i ) 
                             : _backgd);
      break;
    case AIMS_DIFFER : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) != _level ?  res[t].item( i ) 
                             : _backgd);
      break;
    case AIMS_BETWEEN : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) >= _level && res[t].item( i ) 
			     <= _level2 ?  res[t].item( i ) : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) > _level && res[t].item( i ) 
			     <= _level2 ?  res[t].item( i ) : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) > _level && res[t].item( i ) 
			     < _level2 ?  res[t].item( i ) : _backgd);
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) > _level && res[t].item( i ) 
			     < _level2 ?  res[t].item( i ) : _backgd);
      break;
    case AIMS_OUTSIDE : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) < _level || res[t].item( i ) 
			     > _level2 ? (U) res[t].item( i ) : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) <= _level || res[t].item( i ) 
			     > _level2 ? (U) res[t].item( i ) : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) < _level || res[t].item( i ) 
			     >= _level2 ? (U) res[t].item( i ) : _backgd);
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  res.item( i )  = ( res[t].item( i ) <= _level || res[t].item( i ) 
			     >= _level2 ? (U) res[t].item( i ) : _backgd);
      break;
    }

  //No header for the texture yet...
  /*
    if( sqv.header() )
    {
    res.setHeader( sqv.header()->cloneHeader() );
    aims::PythonHeader	*ph 
    = dynamic_cast<aims::PythonHeader *>( res.header() );
    if( ph && ph->hasProperty( "data_type" ) )
    ph->removeProperty( "data_type" );
    }
  */
  return(res);
}

template <class T,class U> inline
AimsData<U> AimsThreshold<T,U>::bin(const AimsData<T> &sqv)
{
  AimsData<U> res(sqv.dimX(),sqv.dimY(),sqv.dimZ(),sqv.dimT(),
                    sqv.borderWidth());
  res.setSizeX(sqv.sizeX());
  res.setSizeY(sqv.sizeY());
  res.setSizeZ(sqv.sizeZ());
  res.setSizeT(sqv.sizeT());

  res = U( 0 );

  typename AimsData<U>::iterator       it1;
  typename AimsData<T>::const_iterator it2;
  
  switch (_type)
  { case AIMS_LOWER_THAN : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level ? _foregd : 0);
      break;
    case AIMS_LOWER_OR_EQUAL_TO : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 <= _level ? _foregd : 0);
      break;
    case AIMS_GREATER_THAN : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level ? _foregd : 0);
      break;
    case AIMS_GREATER_OR_EQUAL_TO : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level ? _foregd : 0);
      break;
    case AIMS_EQUAL_TO : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 == _level ? _foregd : 0);
      break;
    case AIMS_DIFFER : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 != _level ? _foregd : 0);
      break;
    case AIMS_BETWEEN :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level && *it2 <= _level2 ? _foregd : 0);
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level && *it2 <= _level2 ? _foregd : 0);
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 >= _level && *it2 < _level2 ? _foregd : 0);
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS :
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 > _level && *it2 < _level2 ? _foregd : 0);
      break;
    case AIMS_OUTSIDE : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level || *it2 > _level2 ? _foregd : 0);
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 <= _level || *it2 > _level2 ? _foregd : 0);
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 < _level || *it2 >= _level2 ? _foregd : 0);
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS : 
      for (it1=res.begin(),it2=sqv.begin();it2<sqv.end();it1++,it2++)
        *it1 = (*it2 <= _level || *it2 >= _level2 ? _foregd : 0);
      break;
  }

  if( sqv.header() )
    {
      res.setHeader( sqv.header()->cloneHeader() );
      aims::PythonHeader	*ph 
        = dynamic_cast<aims::PythonHeader *>( res.header() );
      if( ph && ph->hasProperty( "data_type" ) )
        ph->removeProperty( "data_type" );
    }
  return(res);
}

template <class T,class U> inline
TimeTexture<U> AimsTexThreshold<T,U>::bin(const TimeTexture<T> &sqv)
{
  unsigned i, n = sqv.nItem(), t , tm = sqv.size();
  TimeTexture<T> temp = sqv ;
  TimeTexture<U> res;
  
  switch (_type)
    { case AIMS_LOWER_THAN :     
	for (t = 0; t < tm ; ++t)
	  for (i = 0; i < n ; ++i)
	    if ( temp[t].item( i ) < _level) 
	      res[t].push_back( 1 );
	    else
	      res[t].push_back( 0 );
	
	break;
    case AIMS_LOWER_OR_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) <= _level) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_GREATER_THAN : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) > _level) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_GREATER_OR_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) >= _level) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_EQUAL_TO : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) == _level) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_DIFFER : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) != _level) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_BETWEEN : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) >= _level 
	       && temp[t].item( i ) <= _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) > _level 
	       && temp[t].item( i ) <= _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) >= _level 
	       && temp[t].item( i ) < _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) > _level 
	       && temp[t].item( i ) < _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_OUTSIDE : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) < _level 
	       && temp[t].item( i ) > _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) <= _level 
	       && temp[t].item( i ) > _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) < _level 
	       && temp[t].item( i ) >= _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS : 
      for (t = 0; t < tm ; ++t)
	for (i = 0; i < n ; ++i)
	  if ( temp[t].item( i ) <= _level 
	       && temp[t].item( i ) >= _level2 ) 
	    res[t].push_back( 1 );
	  else
	    res[t].push_back( 0 );
      break;
    }

  //No header for the texture yet...
  /*
    if( sqv.header() )
    {
    res.setHeader( sqv.header()->cloneHeader() );
    aims::PythonHeader	*ph 
    = dynamic_cast<aims::PythonHeader *>( res.header() );
    if( ph && ph->hasProperty( "data_type" ) )
    ph->removeProperty( "data_type" );
    }
  */
  return(res);
}

    
#endif
