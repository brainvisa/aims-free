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

#include <aims/resampling/mask_d.h>

using namespace aims;
using namespace carto;

BucketMap<Void>* aims::mask( const BucketMap<Void> & src, const BucketMap<Void> & m, bool intersect )
{
  BucketMap<Void>::const_iterator ib, eb = src.end(), ib2, eb2 = m.end();
  BucketMap<Void>::Bucket::const_iterator i, e, j, e2;
  BucketMap<Void> *out = new BucketMap<Void>;
  out->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
  out->setHeader( src.header() );
  for( ib=src.begin(); ib!=eb; ++ib )
  {
    // find timestep
    ib2 = m.find( ib->first );
    if( ib2 == eb2 )
      ib2 = m.find( m.rbegin()->first );
    if( ib2 != eb2 )
    {
      const BucketMap<Void>::Bucket & bk = ib->second;
      const BucketMap<Void>::Bucket & bk2 = ib2->second;
      BucketMap<Void>::Bucket & bkout = (*out)[ ib->first ];

      if( intersect )
      {
        for( i=bk.begin(), e=bk.end(), e2=bk2.end(); i!=e; ++i )
          if( bk2.find( i->first ) != e2 )
            bkout[ i->first ] = Void();
      }
      else
        for( i=bk.begin(), e=bk.end(), e2=bk2.end(); i!=e; ++i )
          if( bk2.find( i->first ) == e2 )
            bkout[ i->first ] = Void();
    }
  }
  return out;
}


template BucketMap<Void>*
aims::maskWithVolume<int16_t>( const BucketMap<Void> & src,
                      const carto::rc_ptr<carto::Volume<int16_t> > m,
                      const int16_t & val, bool insideval );

