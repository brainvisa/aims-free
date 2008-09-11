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

#include <aims/utility/boundingbox.h>
#include <aims/resampling/motion.h>

using namespace aims;

BoundingBox::BoundingBox( const Motion* motion )
  : _npoints( 0 ), _motion( motion ? new Motion( *motion ) : 0 )
{
}


BoundingBox::~BoundingBox()
{
  delete _motion;
}


void BoundingBox::reset()
{
  _npoints = 0;
  _minimum = Point3df( 0, 0, 0 );
  _maximum = Point3df( 0, 0, 0 );
}


void BoundingBox::setMotion( const Motion* motion )
{
  delete _motion;
  if( motion )
    _motion = new Motion( *motion );
  else
    _motion = 0;
}


void BoundingBox::add( const Point3df & pos )
{
  if( _npoints == 0 )
    {
      ++_npoints;
      if( _motion )
        {
          Point3df	mpos;
          mpos = _motion->transform( pos );
          _minimum = mpos;
          _maximum = mpos;
        }
      else
        {
          _minimum = pos;
          _maximum = pos;
        }
      return;
    }
  ++_npoints;
  if( _motion )
    {
      Point3df	mpos;
      mpos = _motion->transform( pos );
      if( mpos[0] < _minimum[0] )
        _minimum[0] = mpos[0];
      if( mpos[1] < _minimum[1] )
        _minimum[1] = mpos[1];
      if( pos[2] < _minimum[2] )
        _minimum[2] = mpos[2];
      if( mpos[0] > _maximum[0] )
        _maximum[0] = mpos[0];
      if( mpos[1] > _maximum[1] )
        _maximum[1] = mpos[1];
      if( mpos[2] > _maximum[2] )
        _maximum[2] = mpos[2];
    }
  else
    {
      if( pos[0] < _minimum[0] )
        _minimum[0] = pos[0];
      if( pos[1] < _minimum[1] )
        _minimum[1] = pos[1];
      if( pos[2] < _minimum[2] )
        _minimum[2] = pos[2];
      if( pos[0] > _maximum[0] )
        _maximum[0] = pos[0];
      if( pos[1] > _maximum[1] )
        _maximum[1] = pos[1];
      if( pos[2] > _maximum[2] )
        _maximum[2] = pos[2];
    }
}


void BoundingBox::add( const BucketMap<Void> & bck )
{
  if( bck.empty() )
    return;
  const BucketMap<Void>::Bucket	& bk = bck.begin()->second;
  float	vx = bck.sizeX();
  float	vy = bck.sizeY();
  float	vz = bck.sizeZ();
  BucketMap<Void>::Bucket::const_iterator	ib, eb = bk.end();

  for( ib=bk.begin(); ib!=eb; ++ib )
    add( Point3df( ib->first[0] * vx, ib->first[1] * vy, ib->first[2] * vz ) );
}


