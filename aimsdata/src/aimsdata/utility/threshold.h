/*
 *  Threshold operators
 */
#ifndef AIMS_UTILITY_THRESHOLD_H
#define AIMS_UTILITY_THRESHOLD_H

#include <iostream>
#include <limits>
#include <functional>

#include <aims/config/aimsdata_config.h>
#include <cartodata/volume/volume.h>
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
    with levels 0 and foregd (32767 by default).
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
                        T backgd = 0, 
                        U foregd = (U)(!std::numeric_limits<U>::is_specialized 
                                      || (std::numeric_limits<U>::max() >= 32767)
                                      ? 32767 : std::numeric_limits<U>::max() ) );
  virtual ~AimsThreshold() {}

  /// Return the multi-level thresholded image
  inline carto::VolumeRef<T> operator () (const carto::VolumeRef<T> &sqv);
  /// Return the multi-level thresholded image with clipped values (backgd
  /// ignored)
  inline carto::VolumeRef<T> clip(const carto::VolumeRef<T> &sqv);
  /// Return the binary thresholded image
  inline carto::VolumeRef<U> bin(const carto::VolumeRef<T> &sqv);

protected:
  /// Threshold type
  threshold_t _type;
  /// Lower level or unique level
  T    _level;
  /// Upper level
  T    _level2;
  T    _backgd;
  U    _foregd;
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
                                   T backgd, U foregd )
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
carto::VolumeRef<T> AimsThreshold<T,U>::operator () (const carto::VolumeRef<T> &sqv)
{
  carto::VolumeRef<T> res( sqv.getSize(), sqv.getBorders() );
  res.setVoxelSize( sqv.getVoxelSize() );

  res = T( 0 );

  switch (_type)
  { case AIMS_LOWER_THAN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x < level ? x : background; });
      break;
    case AIMS_LOWER_OR_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x <= level ? x : background; });
      break;
    case AIMS_GREATER_THAN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x > level ? x : background; });
      break;
    case AIMS_GREATER_OR_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x >= level ? x : background; });
      break;
    case AIMS_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x == level ? x : background; });
      break;
    case AIMS_DIFFER :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, background=_backgd](const auto &x) { return x != level ? x : background; });
      break;
    case AIMS_BETWEEN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x >= level1 and x<= level2 ? x : background; });
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x > level1 and x<= level2 ? x : background; });
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x >= level1 and x< level2 ? x : background; });
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x > level1 and x< level2 ? x : background; });
      break;
    case AIMS_OUTSIDE :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x < level1 || x > level2 ? x : background; });
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x <= level1 || x > level2 ? x : background; });
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x < level1 || x >= level2 ? x : background; });
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) { return x <= level1 || x >= level2 ? x : background; });
      break;
  }

  res.copyHeaderFrom( sqv.header() );
  return(res);
}


template <class T,class U> inline
carto::VolumeRef<T> AimsThreshold<T,U>::clip (const carto::VolumeRef<T> &sqv)
{
  carto::VolumeRef<T> res( sqv.getSize(), sqv.getBorders() );
  res.setVoxelSize( sqv.getVoxelSize() );

  res = T( 0 );

  typename carto::VolumeRef<T>::iterator       it1;
  typename carto::VolumeRef<T>::const_iterator it2;

  switch (_type)
  { case AIMS_LOWER_THAN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level](const auto &x) { return x < level ? x : level; });
      break;
    case AIMS_LOWER_OR_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level](const auto &x) { return x <= level ? x : level; });
      break;
    case AIMS_GREATER_THAN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level](const auto &x) { return x > level ? x : level; });
      break;
    case AIMS_GREATER_OR_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level](const auto &x) { return x >= level ? x : level; });
      break;
    case AIMS_BETWEEN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) 
        { return x < level1 ? level1 : ( x <= level2 ? x : level2); });
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) 
        { return x <= level1 ? level1 : ( x <= level2 ? x : level2); });
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) 
        { return x < level1 ? level1 : ( x < level2 ? x : level2); });
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, background=_backgd](const auto &x) 
        { return x <= level1 ? level1 : ( x < level2 ? x : level2); });
      break;
    case AIMS_OUTSIDE :
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND :
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND :
    case AIMS_OUTSIDE_INCLUDE_BOUNDS :
    case AIMS_EQUAL_TO :
    case AIMS_DIFFER :
      // Clipping has no real meaning in those cases, thus we do standard
      // thresholding instead.
      std::cerr << "Warning: AimsThreshold cannot use clipping with this mode, "
        "using standard clipping instead." << std::endl;
      return (*this)(sqv);
  }

  res.copyHeaderFrom( sqv.header() );
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
  return(res);
}

template <class T,class U> inline
carto::VolumeRef<U> AimsThreshold<T,U>::bin(const carto::VolumeRef<T> &sqv)
{
  carto::VolumeRef<U> res( sqv.getSize(), sqv.getBorders() );
  res.setVoxelSize( sqv.getVoxelSize() );

  res = U( 0 );

  typename carto::VolumeRef<U>::iterator       it1;
  typename carto::VolumeRef<T>::const_iterator it2;

  switch (_type)
  {

    case AIMS_LOWER_THAN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x < level ? foreground : 0; });
      break;
    case AIMS_LOWER_OR_EQUAL_TO : 
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x <= level ? foreground : 0; });
      break;
    case AIMS_GREATER_THAN : 
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x > level ? foreground : 0; });
      break;
    case AIMS_GREATER_OR_EQUAL_TO : 
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x >= level ? foreground : 0; });
      break;
    case AIMS_EQUAL_TO :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x == level ? foreground : 0; });
      break;
    case AIMS_DIFFER :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level=_level, foreground=_foregd](const auto &x) { return x != level ? foreground : 0; });
      break;
    case AIMS_BETWEEN :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x >= level1 and x<= level2 ? foreground : 0; });
      break;
    case AIMS_BETWEEN_EXCLUDE_LOWER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x > level1 and x<= level2 ? foreground : 0; });
      break;
    case AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x >= level1 and x< level2 ? foreground : 0; });
      break;
    case AIMS_BETWEEN_EXCLUDE_BOUNDS :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x > level1 and x< level2 ? foreground : 0; });
      break;
    case AIMS_OUTSIDE :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x < level1 || x > level2 ? foreground : 0; });
      break;
    case AIMS_OUTSIDE_INCLUDE_LOWER_BOUND : 
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x <= level1 || x > level2 ? foreground : 0; });
      break;
    case AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND : 
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x < level1 || x >= level2 ? foreground : 0; });
      break;
    case AIMS_OUTSIDE_INCLUDE_BOUNDS :
      carto::volumeutil::applyTowards(
        *sqv, *res,
        [level1=_level, level2=_level, foreground=_foregd](const auto &x) { return x <= level1 || x >= level2 ? foreground : 0; });
      break;
  }

  res.copyHeaderFrom( sqv.header() );
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
  return(res);
}

    
#endif
