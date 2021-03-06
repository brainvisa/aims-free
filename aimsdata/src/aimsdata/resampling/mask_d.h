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

#ifndef AIMS_RESAMPLING_MASK_D_H
#define AIMS_RESAMPLING_MASK_D_H

#include <aims/resampling/mask.h>

namespace aims {

  template <typename T>
  BucketMap<Void>* maskWithVolume( const BucketMap<Void> & src,
                                   const carto::rc_ptr<carto::Volume<T> > m,
                                   const T & val, bool insideval )
  {
    BucketMap<Void>::const_iterator ib, eb = src.end();
    BucketMap<Void>::Bucket::const_iterator i, e;
    BucketMap<Void> *out = new BucketMap<Void>;
    std::vector<float> vs;
    src.header().getProperty( "voxel_size", vs );
    while( vs.size() < 4 )
      vs.push_back( 1. );
    out->setSizeXYZT( vs[0], vs[1], vs[2], vs[3] );
    out->setHeader( src.header() ); // maybe a bad idea ?
    int t;
    for( ib=src.begin(); ib!=eb; ++ib )
    {
      // find timestep
      t = ib->first;
      if( t < m->getSizeT() )
      {
        const BucketMap<Void>::Bucket & bk = ib->second;
        BucketMap<Void>::Bucket & bkout = (*out)[ ib->first ];

        if( insideval )
        {
          for( i=bk.begin(), e=bk.end(); i!=e; ++i )
          {
            const Point3d & pos = i->first;
            if( m->at( pos[0], pos[1], pos[2], t ) == val )
              bkout[ i->first ] = Void();
          }
        }
        else
        {
          for( i=bk.begin(), e=bk.end(); i!=e; ++i )
          {
            const Point3d & pos = i->first;
            if( m->at( pos[0], pos[1], pos[2], t ) != val )
              bkout[ i->first ] = Void();
          }
        }
      }
    }
    return out;
  }

}

#endif
