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
 *  Flip data.
 */
#ifndef AIMS_UTILITY_FLIP_H
#define AIMS_UTILITY_FLIP_H

#include <aims/config/aimsdata_config.h>
#include <cartodata/volume/volume.h>
#include <cartobase/containers/nditerator.h>


/** The template object function for flipping.
    This object function returns the flipped data.
    The template argument is the basic data type.
*/
template <class T>
class AimsFlip
{
public:
    /**@name Constructor and Destructor*/
    //@{
    /// Constructor does nothing
    AimsFlip() { }
    /// Destructor does nothing
    virtual ~AimsFlip() { }
    //@}
    
    /**@name Methods*/
    //@{
    /// function which returns the XX flipped data
    inline carto::VolumeRef<T> doXX(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the YY flipped data
    inline carto::VolumeRef<T> doYY(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the ZZ flipped data
    inline carto::VolumeRef<T> doZZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the XXZZ flipped data
    inline carto::VolumeRef<T> doXXZZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the XXYY flipped data
    inline carto::VolumeRef<T> doXXYY(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the YYZZ flipped data
    inline carto::VolumeRef<T> doYYZZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the XXYYZZ flipped data
    inline carto::VolumeRef<T> doXXYYZZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the XY flipped data
    inline carto::VolumeRef<T> doXY(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the XZ flipped data
    inline carto::VolumeRef<T> doXZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    /// function which returns the YZ flipped data
    inline carto::VolumeRef<T> doYZ(
      const carto::rc_ptr<carto::Volume<T> > &thing );
    //@}
};


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXX(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSize(), thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = thing->getSizeX() -  1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeY(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[1] = thing->getSizeY() - pos[1] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), ++pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeY(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[2] = thing->getSizeZ() - pos[2] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), ++pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeY(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = thing->getSizeX() -  1;
    pos[2] = thing->getSizeZ() - pos[2] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[0] )
      *p = thing->at( pos );
  }

  return res;
}

template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXYY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeY(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = thing->getSizeX() -  1;
    pos[1] = thing->getSizeY() - pos[1] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYYZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeY(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[1] = thing->getSizeY() - pos[1] - 1;
    pos[2] = thing->getSizeZ() - pos[2] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), ++pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeY(), thing->getSizeX(),
                           thing->getSizeZ(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );
  std::vector<float> vs = thing->getVoxelSize();
  float t = vs[0];
  vs[0] = vs[1];
  vs[1] = t;
  res.setVoxelSize( vs );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = res->getSizeY() - pos[1] - 1;
    pos[1] = res->getSizeX() - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[1] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeZ(), thing->getSizeY(),
                           thing->getSizeX(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );
  std::vector<float> vs = thing->getVoxelSize();
  float t = vs[0];
  vs[0] = vs[2];
  vs[2] = t;
  res.setVoxelSize( vs );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = res->getSizeZ() - pos[2] - 1;
    pos[2] = res->getSizeX() - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[2] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSizeX(), thing->getSizeZ(),
                           thing->getSizeY(), thing->getSizeT(),
                           thing->getBorders() );
  res.copyHeaderFrom( thing->header() );
  std::vector<float> vs = thing->getVoxelSize();
  float t = vs[1];
  vs[1] = vs[2];
  vs[2] = t;
  res.setVoxelSize( vs );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;
  int x;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    x = pos[1];
    pos[1] = res->getSizeZ() - pos[2] - 1;
    pos[2] = res->getSizeY() - x - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), ++pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXYYZZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  carto::VolumeRef<T> res( thing->getSize(), thing->getBorders() );
  res.copyHeaderFrom( thing->header() );

  carto::line_NDIterator<T> it( &res->at( 0 ), res->getSize(),
                                res->getStrides() );
  T *p, *pp;
  std::vector<int> pos;

  for( ; !it.ended(); ++it )
  {
    p=&*it;
    pos = it.position();
    pos[0] = thing->getSizeX() - 1;
    pos[1] = thing->getSizeY() - pos[1] - 1;
    pos[2] = thing->getSizeZ() - pos[2] - 1;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ), --pos[0] )
      *p = thing->at( pos );
  }

  return res;
}


#endif
