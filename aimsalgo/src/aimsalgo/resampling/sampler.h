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


#ifndef AIMS_RESAMPLING_SAMPLER_H
#define AIMS_RESAMPLING_SAMPLER_H

#include <aims/def/assert.h>
#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <aims/resampling/motion.h>
#include <aims/bucket/bucketMap.h>
#include <vector>


typedef struct {
  unsigned short x, y, z;
  int            offset;
} PVItem;


namespace carto
{
  extern template class Volume<PVItem>;
}


template <class T>
class Sampler
{
  public:

    Sampler( ) : _ref( 0 ) { };
    virtual ~Sampler() { }

    virtual void doit(
      const Motion& motion,
      carto::rc_ptr<carto::Volume<PVItem> >& thing ) const;


    void setRef( const carto::rc_ptr<carto::Volume<T> >& ref ) { _ref = &ref; }

  protected:

    void _sliceResamp( carto::rc_ptr<carto::Volume<PVItem> >& resamp,
		       PVItem* out,
		       const Point3df& start, int t,
		       const carto::rc_ptr<carto::Volume<float> >& Rinv )  const;
    const carto::rc_ptr<carto::Volume<T> >* _ref;
};


typedef struct {
  unsigned short  x, y, z ;
  int             offset ;
} PVVectorItem;

template <class T>
class BucketSampler
{
  public:

    BucketSampler( ) : _ref(0) { }
    virtual ~BucketSampler() { }

    virtual void doit( const Motion& motion, 
		       std::vector< PVVectorItem >& thing,
		       const aims::BucketMap<T>&  model ) const;

    void setRef( const carto::rc_ptr<carto::Volume<T> >& ref ) { _ref = &ref; }

  protected:
    const carto::rc_ptr<carto::Volume<T> >* _ref;
};




template <class T> inline
void 
Sampler<T>::doit( const Motion& motion, carto::rc_ptr<carto::Volume<PVItem> >& thing ) const
{


  ASSERT( _ref );
  ASSERT( thing->getSizeT() == (*_ref)->getSizeT() && thing->getBorders()[0] == 0 );

  Motion dirMotion = motion;
  Motion invMotion = motion.inverse();
  
  // scale motion
  Point3df sizeFrom( (*_ref)->getVoxelSize() );
  Point3df sizeTo( thing->getVoxelSize() );
  dirMotion.scale( sizeFrom, sizeTo );
  invMotion.scale( sizeTo, sizeFrom );


  PVItem *it;

#ifdef DEBUG
  cout << "Sampling volume [  1] / slice [  1]" << flush;
#endif
  for (int t = 1; t <= thing->getSizeT(); t++ )
  {
    Point3df start = invMotion.translation();
    for ( int s = 1; s <= thing->getSizeZ(); s++ )
    {
#ifdef DEBUG
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" 
           << setw( 3 ) << t << "] / slice ["
           << setw( 3 ) << s << "]" << flush;
#endif
      it = &thing->at( 0, 0, s, t );
      _sliceResamp( thing, it, start, t - 1, invMotion.rotation() );
      start[ 0 ] += invMotion.rotation()( 0, 2 );
      start[ 1 ] += invMotion.rotation()( 1, 2 );
      start[ 2 ] += invMotion.rotation()( 2, 2 );
    }
  }
  //cout << endl;


}


template <class T> inline
void Sampler<T>::_sliceResamp( carto::rc_ptr<carto::Volume<PVItem> >& resamp,
                               PVItem* out,
                               const Point3df& start, int t,
                               const carto::rc_ptr<carto::Volume<float> >& Rinv )
                                   const
{
  int dimX1 = (*_ref)->getSizeX();
  int dimX2 = resamp->getSizeX();
  int dimY2 = resamp->getSizeY();

  const int SIXTEEN = 16;
  const int TWO_THEN_SIXTEEN = 65536;

  int maxX = TWO_THEN_SIXTEEN * ( dimX1 - 1 );
  int maxY = TWO_THEN_SIXTEEN * ( (*_ref)->getSizeY() - 1 );
  int maxZ = TWO_THEN_SIXTEEN * ( (*_ref)->getSizeZ() - 1 );

  int xLinCurrent = ( int )( 65536.0 * start[ 0 ] );
  int yLinCurrent = ( int )( 65536.0 * start[ 1 ] );
  int zLinCurrent = ( int )( 65536.0 * start[ 2 ] );

  int xu = ( int )( 65536.0 * Rinv->at( 0, 0 ) );
  int yu = ( int )( 65536.0 * Rinv->at( 1, 0 ) );
  int zu = ( int )( 65536.0 * Rinv->at( 2, 0 ) );
  int xv = ( int )( 65536.0 * Rinv->at( 0, 1 ) );
  int yv = ( int )( 65536.0 * Rinv->at( 1, 1 ) );
  int zv = ( int )( 65536.0 * Rinv->at( 2, 1 ) );

  int xCurrent, yCurrent, zCurrent;
  const T *it;
  int   incr_vois[8] = { 0,
            int( &(*_ref)->at( 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 1, 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 0, 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 0, 0, 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 1, 0, 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 1, 1, 1 ) - &(*_ref)->at( 0 ) ),
            int( &(*_ref)->at( 0, 1, 1 ) - &(*_ref)->at( 0 ) ) };
  for ( int v = dimY2; v--; )
    {
    xCurrent = xLinCurrent;
    yCurrent = yLinCurrent;
    zCurrent = zLinCurrent;

    for ( int u = dimX2; u--; )
      {
      if ( xCurrent >= 0 && xCurrent < maxX &&
           yCurrent >= 0 && yCurrent < maxY &&
           zCurrent >= 0 && zCurrent < maxZ  )
	{
        it = &(*_ref)->at( xCurrent >> SIXTEEN, yCurrent >> SIXTEEN, zCurrent >> SIXTEEN, t );
  	
	if (*(it + incr_vois[0]) == -32768 ||
	    *(it + incr_vois[1]) == -32768 ||
	    *(it + incr_vois[2]) == -32768 ||
	    *(it + incr_vois[3]) == -32768 ||
	    *(it + incr_vois[4]) == -32768 ||
	    *(it + incr_vois[5]) == -32768 ||
	    *(it + incr_vois[6]) == -32768 ||
	    *(it + incr_vois[7]) == -32768 )
	  {
	    out->offset = -1;
	  }
	else
	  {
	    out->x =  xCurrent & 0xffff;
	    out->y =  yCurrent & 0xffff;
	    out->z =  zCurrent & 0xffff;
	    out->offset =  &(*_ref)->at( xCurrent >> SIXTEEN, yCurrent >> SIXTEEN,
                                     zCurrent >> SIXTEEN ) - &(*_ref)->at( 0 );
	  }
	}
      else
	{
	  out->offset = -1;
	}
      out++;
      xCurrent += xu;
      yCurrent += yu;
      zCurrent += zu;
      }
    xLinCurrent += xv;
    yLinCurrent += yv;
    zLinCurrent += zv;
    }
}

template <class T> inline
void 
BucketSampler<T>::doit( const Motion& motion, 
			std::vector< PVVectorItem >& thing, 
			const aims::BucketMap<T>&  model ) const
{
  const int TWO_THEN_SIXTEEN = 65536;


  thing.clear() ;

  ASSERT( _ref );
  
  Motion dirMotion; dirMotion  = motion;
  Motion invMotion; invMotion  = motion.inverse();

  
  // scale motion
  Point3df sizeFrom( (*_ref)->getVoxelSize() );
  Point3df sizeTo( model.sizeX(), model.sizeY(), model.sizeZ() );

  dirMotion.scale( sizeFrom, sizeTo );
  invMotion.scale( sizeTo, sizeFrom );

  typename aims::BucketMap<T>::Bucket::const_iterator 
    it = model.begin()->second.begin() ;

  PVVectorItem item ;
  int nbOfVoxels = model.begin()->second.size() ;

  thing.reserve(nbOfVoxels) ;
  thing.resize(nbOfVoxels) ;
  
  Point3df x, y, z;

  float transl0 = invMotion.translation()[0];
  float transl1 = invMotion.translation()[1];
  float transl2 = invMotion.translation()[2];

  invMotion.setTranslation( 0. );

  x = invMotion.transform( 1.F, 0.F, 0.F ) ;
  y = invMotion.transform( 0.F, 1.F, 0.F ) ;
  z = invMotion.transform( 0.F, 0.F, 1.F ) ;

  int maxX = (*_ref)->getSizeX() - 1;
  int maxY = (*_ref)->getSizeY() - 1;
  int maxZ = (*_ref)->getSizeZ() - 1;
  int oS= &(*_ref)->at( 0, 0, 1 ) - &(*_ref)->at( 0 ),
    dX = &(*_ref)->at( (*_ref)->getSizeX() ) - &(*_ref)->at( 0 );

  for(int i = 0 ; i < nbOfVoxels ; ++i , ++it )
    {  
      float current0, current1, current2;
      current0 = static_cast<float>( (it->first)[0] ) ;
      current1 = static_cast<float>( (it->first)[1] ) ;
      current2 = static_cast<float>( (it->first)[2] ) ;
      
      float tmp0 = current0 * x[0] + current1 * y[0] + current2 * z[0] + transl0 ;
      float tmp1 = current0 * x[1] + current1 * y[1] + current2 * z[1] + transl1 ;
      float tmp2 = current0 * x[2] + current1 * y[2] + current2 * z[2] + transl2 ;


      if (   tmp0 >= 0 && tmp0 < maxX &&
	     tmp1 >= 0 && tmp1 < maxY &&
	     tmp2 >= 0 && tmp2 < maxZ)
      {	
	item.x = (unsigned short) (( tmp0 - int(tmp0) ) * TWO_THEN_SIXTEEN);
	item.y = (unsigned short) (( tmp1 - int(tmp1) ) * TWO_THEN_SIXTEEN);
	item.z = (unsigned short) (( tmp2 - int(tmp2) ) * TWO_THEN_SIXTEEN);
	item.offset =   int(tmp2) * oS +
   	                int(tmp1) * dX +
	                int(tmp0);
      }
      else
	{
	  item.offset = -1;
	}
      
      thing[i] = item ;
    }
}

#endif
