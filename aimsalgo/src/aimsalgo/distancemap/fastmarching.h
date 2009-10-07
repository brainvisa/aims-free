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


#ifndef AIMS_DISTANCEMAP_FASTMARCHING_H
#define AIMS_DISTANCEMAP_FASTMARCHING_H

#include <cartodata/volume/volume.h>
#include <aims/bucket/sparsevolume.h>
#include <aims/connectivity/connectivity.h>
#include <float.h>

namespace aims
{

  namespace internal
  {
    namespace fastmarching
    {
      template <typename T>
      class StorageTrait
      {
      public:
        typedef T Type;
        typedef T FloatType;
        typedef T LowLevelType;
        typedef T VoxelType;
        typedef carto::rc_ptr<T> RCType;
      };
    }
  }

  template <typename T>
  class FastMarching
  {
  public:
    typedef typename internal::fastmarching::StorageTrait<T>::FloatType
      FloatType;
    typedef typename internal::fastmarching::StorageTrait<T>::RCType RCType;
    typedef typename internal::fastmarching::StorageTrait<FloatType>::RCType
        RCFloatType;

    /* The connectivity type is only used to build the interfaces between
       the work region and the seeds: the fast marching itself only uses
       6-connectivity.
    */
    FastMarching( Connectivity::Type c, bool mid_interface = false );
    ~FastMarching();
    void setVerbose( bool x );
    bool verbose() const;
    RCFloatType doit( const RCType & vol,
      int16_t worklabel, int16_t inlabel, int16_t outlabel);
    RCFloatType doit( const RCType & vol,
      const std::set<int16_t> & worklabels,
      const std::set<int16_t> & seedlabels);
    /* RCFloatType doit( const RCType & vol,
      const BucketMap<Void> & workbucket,
      const std::map<int16_t,BucketMap<Void>*> & seedbuckets ); */
    const BucketMap<float> & midInterface( int16_t label1,
                                           int16_t label2 ) const;
    carto::VolumeRef<float> midInterfaceVol( int16_t label1,
                                             int16_t label2 ) const;
    std::vector<std::pair<int16_t,int16_t> > midInterfaceLabels () const;
    RCType voronoiVol() const;
    /** sets an initialized speed map.
    The inverse speed will be deduced from it. */
    void setSpeedMap( RCFloatType speed );
    /** sets an initialized inverse speed map (overrides any previous speed
    map) */
    void setInvSpeedMap( RCFloatType invspeed );
    /// clear (inverse) speed map
    void clearSpeedMap();

  private:
    struct Private;
    Private *d;
  };


  namespace internal
  {
    namespace fastmarching
    {

      template <typename T>
      class StorageTrait<carto::Volume<T> >
      {
      public:
        typedef carto::Volume<T> Type;
        typedef carto::Volume<float> FloatType;
        typedef carto::Volume<T> LowLevelType;
        typedef T VoxelType;
        typedef carto::VolumeRef<T> RCType;
        static LowLevelType & lowlevel( RCType & x )
        { return *x; }
        static const LowLevelType & lowlevel( const RCType & x )
        { return *x; }
      };

      template <typename T>
      class StorageTrait<BucketMap<T> >
      {
      public:
        typedef BucketMap<T> Type;
        typedef BucketMap<float> FloatType;
        typedef typename BucketMap<T>::Bucket LowLevelType;
        typedef T VoxelType;
        typedef carto::rc_ptr<BucketMap<T> > RCType;
        static LowLevelType & lowlevel( RCType & x )
        { return x->begin().second; }
        static const LowLevelType & lowlevel( const RCType & x )
        { return x->begin().second; }
      };

    }
  }


  extern template class FastMarching<carto::Volume<int16_t> >;

}

#endif

