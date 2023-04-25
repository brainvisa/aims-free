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
    AimsFlip() : _shared_view( true ), _update_transforms( true ),
      _flip_memory( false ) { }
    virtual ~AimsFlip() { }
    
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
    /** returns the flipped data in the given orientation

        Orientation conventions are those of the carto::Referential class.
    */
    inline carto::VolumeRef<T> flip(
      const carto::rc_ptr<carto::Volume<T> > &thing,
      const std::string & orient );

    bool useSharedView() const { return _shared_view; }
    void setUseSharedView( bool x ) { _shared_view = x; }
    bool updateTransforms() const { return _update_transforms; }
    void setUpdateTransforms( bool x ) { _update_transforms = x; }
    bool flipMemory() const { return _flip_memory; }
    void setFlipMemory( bool x ) { _flip_memory = x; }
    std::string centerRefertential() const { return _center_ref; }
    void setCenterReferential( const std::string & x ) { _center_ref = x; }

protected:
    bool _shared_view;
    bool _update_transforms;
    bool _flip_memory;
    std::string _center_ref;
};


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::flip(
  const carto::rc_ptr<carto::Volume<T> > &thing, const std::string & orient )
{
  carto::VolumeRef<T> res;
  if( _shared_view )
    // build a view inside thing
    res.reset( new carto::Volume<T>( thing, std::vector<int>( 4, 0 ),
                                     thing->getSize() ) );
  else
    res.reset( new carto::Volume<T>( *thing ) );

  res.copyHeaderFrom( thing->header() );

  carto::Object trans;
  if( !_update_transforms )
  {
    try
    {
      trans = res->header().getProperty( "transformations" );
    }
    catch( ... )
    {
    }
  }

  // flip L/R
  if( !_shared_view && _flip_memory )
    res->flipToOrientation( orient, orient );
  else
    res->flipToOrientation( orient );
  // and pretend we are back to LPI
  res->referential().setOrientation( "LPI" );

  if( trans )
    // restore initial transforms
    res->header().setProperty( "transformations", trans );

  return res;
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXX(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "RPI" );
}

template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "LAI" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "LPS" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "RPS" );
}

template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXYY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "RAI" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYYZZ( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "LAS" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXY( const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "PLI" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "IPL" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doYZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "LIP" );
}


template <class T> inline
carto::VolumeRef<T> AimsFlip<T>::doXXYYZZ(
  const carto::rc_ptr<carto::Volume<T> > &thing )
{
  return this->flip( thing, "RAS" );
}


#endif
