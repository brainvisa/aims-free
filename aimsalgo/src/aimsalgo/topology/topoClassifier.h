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


#ifndef AIMS_TOPOLOGY_TOPOCLASSIFIER_H
#define AIMS_TOPOLOGY_TOPOCLASSIFIER_H

#include <aims/data/data.h>
#include <aims/topology/topoClassif.h>
#include <aims/topology/classifMeaning.h>
#include <aims/bucket/bucketMap.h>
#include <aims/utility/converter_bucket.h>


template< typename T >
class TopologicalClassifier
{
public:
  class ResultRcType;
  class ResultType;

  TopologicalClassifier() { }
  virtual ~TopologicalClassifier() { }

  ResultRcType doit( const T & );
  ResultType & takeResult( ResultRcType & d ) { return d; }
};


template< typename T >
class TopologicalClassifier<AimsData<T> >
{
public:
  typedef AimsData<int16_t> ResultRcType;
  typedef AimsData<int16_t> ResultType;

  TopologicalClassifier() { }
  virtual ~TopologicalClassifier() { }

  ResultRcType doit( const AimsData< T >& );
  ResultType & takeResult( ResultRcType & d ) { return d; }
};


template< typename T >
class TopologicalClassifier< aims::BucketMap<T> >
{
public:
  typedef carto::rc_ptr< aims::BucketMap<int16_t> > ResultRcType;
  typedef aims::BucketMap<int16_t> ResultType;

  TopologicalClassifier() { }
  virtual ~TopologicalClassifier() { }

  ResultRcType doit( const aims::BucketMap< T >& );
  ResultType & takeResult( ResultRcType & d ) { return *d; }
};


template< typename T > inline
AimsData< short > TopologicalClassifier< AimsData<T> >::doit( const AimsData< T >& d )
{
  int x, y, z;
  T label;

  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();

  AimsData< short > res( dx, dy, dz );
  res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ() );

  TopologicalClassification< T > topo( d );
  TopologicalClassificationMeaning topoMean;

  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
      for ( x=0; x<dx; x++ )
	if( ( label = d( x, y, z ) ) )
	  {
	    Point3d pt( x, y, z );
	    topo.computeLocalCCNumbers( pt, (int)label );
	    res( x, y, z ) = (short)topoMean.classification( topo.Cstar(),
							     topo.Cbar() );
	  }

  return res;
}


template< typename T > inline
carto::rc_ptr< aims::BucketMap<int16_t> >
TopologicalClassifier< aims::BucketMap<T> >::doit(
  const aims::BucketMap< T >& d )
{
  typename aims::BucketMap<T>::const_iterator ibm, ebm = d.end();
  typename aims::BucketMap<T>::Bucket::const_iterator ib, eb;

  aims::BucketMap<int16_t> *res = new aims::BucketMap<int16_t>;
  res->setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), d.sizeT() );

  carto::VolumeRef<int16_t> *dc;
  carto::Converter< aims::BucketMap<T>, carto::VolumeRef<T> > conv;
  dc = conv( d );
  TopologicalClassification< T > topo( *dc );
  TopologicalClassificationMeaning topoMean;

  for( ibm=d.begin(); ibm!=ebm; ++ibm )
  {
    aims::BucketMap<int16_t>::Bucket & bk = (*res)[ ibm->first ];
    for( ib=ibm->second.begin(), eb=ibm->second.end(); ib!=eb; ++ib )
    {
      topo.computeLocalCCNumbers( ib->first, ib->second );
      bk[ ib->first ] = (short)topoMean.classification( topo.Cstar(),
                                                        topo.Cbar() );
    }
  }
  delete dc;

  return carto::rc_ptr< aims::BucketMap<int16_t> >( res );
}


template<> inline
carto::rc_ptr< aims::BucketMap<int16_t> >
TopologicalClassifier< aims::BucketMap<Void> >::doit(
  const aims::BucketMap<Void>& d )
{
  aims::BucketMap<Void>::const_iterator ibm, ebm = d.end();
  aims::BucketMap<Void>::Bucket::const_iterator ib, eb;

  aims::BucketMap<int16_t> *res = new aims::BucketMap<int16_t>;
  res->setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), d.sizeT() );

  carto::VolumeRef<int16_t> *dc;
  carto::Converter< aims::BucketMap<Void>, carto::VolumeRef<int16_t> > conv;
  dc = conv( d );
  TopologicalClassification<int16_t> topo( *dc );
  TopologicalClassificationMeaning topoMean;

  for( ibm=d.begin(); ibm!=ebm; ++ibm )
  {
    aims::BucketMap<int16_t>::Bucket & bk = (*res)[ ibm->first ];
    for( ib=ibm->second.begin(), eb=ibm->second.end(); ib!=eb; ++ib )
    {
      topo.computeLocalCCNumbers( ib->first, 1 );
      bk[ ib->first ] = (short)topoMean.classification( topo.Cstar(),
                                                        topo.Cbar() );
    }
  }
  delete dc;

  return carto::rc_ptr< aims::BucketMap<int16_t> >( res );
}

#endif
