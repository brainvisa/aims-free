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


#ifndef AIMS_ROI_ROIITERATOR_H
#define AIMS_ROI_ROIITERATOR_H

#include <aims/roi/maskIterator.h>
#include <cartobase/smart/rcptr.h>
#include <aims/io/reader.h>
#include <set>
#include <string>

namespace aims
{

  //---------------------------------------------------------------------------
  class ROILabelMap : public carto::RCObject
  {
  public:

    virtual ~ROILabelMap() {}

    virtual int32_t at( const Point3df & ) const = 0;
    std::vector< std::string > roiNames;
  };


  //---------------------------------------------------------------------------
  class VolumeROILabelMap : public ROILabelMap
  {
  public:

    VolumeROILabelMap( carto::VolumeRef< int32_t > & );
    virtual ~VolumeROILabelMap() {}

    virtual int32_t at( const Point3df & ) const;

  private:
    carto::VolumeRef< int32_t > _volume;
  };


  //---------------------------------------------------------------------------
  class MotionedVolumeROILabelMap : public VolumeROILabelMap
  {
  public:

    MotionedVolumeROILabelMap( carto::VolumeRef< int32_t > &, const AffineTransformation3d & );
    virtual ~MotionedVolumeROILabelMap() {}

    virtual int32_t at( const Point3df & ) const;

  private:

    const AffineTransformation3d _motion;
  };


  //---------------------------------------------------------------------------
  class RoiIterator : public carto::RCObject
  {
  public:
    virtual ~RoiIterator() {}

    virtual bool isValid() const = 0;
    virtual void restart() = 0;
    virtual size_t count() const = 0;

    virtual void next() = 0;
    virtual carto::rc_ptr< MaskIterator > maskIterator() const = 0;
    inline const Point3d volumeDimension() const;
    inline const Point3df voxelSize() const;
    inline float voxelVolume() const;
    virtual std::string regionName() const = 0;
    /** Set region name attribute in graph.
        Normally "name" or "label". If several values are provided, attributes
        are searched in each graph vertex, in that order.
        If the attributes list is empty, then the graph "label_property"
        attribute will be used, and if it is not specified there, the default
        search list ("name", "label") will be used.
    */
    virtual void setRegionNameAttributes( const std::vector<std::string>& ) {}
    /** Set region name attribute in graph.
        Same as above, for a single attribute.
    */
    virtual void setRegionNameAttributes( const std::string & );


    virtual carto::rc_ptr< ROILabelMap > createLabelMap();
  };


  //---------------------------------------------------------------------------
  class MotionedRoiIterator : public RoiIterator
  {
  public:
    MotionedRoiIterator( const carto::rc_ptr< RoiIterator > &,
                         const AffineTransformation3d & );
    virtual ~MotionedRoiIterator();

    virtual bool isValid() const;
    virtual void restart();
    virtual size_t count() const;

    virtual void next();
    virtual carto::rc_ptr< MaskIterator > maskIterator() const;
    inline const Point3d volumeDimension() const;
    inline const Point3df voxelSize() const;
    inline float voxelVolume() const;
    virtual std::string regionName() const;
    virtual void setRegionNameAttributes( const std::vector<std::string>& );

    virtual carto::rc_ptr< ROILabelMap > createLabelMap();

  protected:
    const carto::rc_ptr< RoiIterator > _roiIterator;
    const AffineTransformation3d _motion;
  };


  //---------------------------------------------------------------------------
  inline const Point3d RoiIterator::volumeDimension() const
  {
    return maskIterator()->volumeDimension();
  }

  //---------------------------------------------------------------------------
  inline const Point3df RoiIterator::voxelSize() const
  {
    return maskIterator()->voxelSize();
  }

  //---------------------------------------------------------------------------
  inline float RoiIterator::voxelVolume() const
  {
    return maskIterator()->voxelVolume();
  }

  //---------------------------------------------------------------------------
  template <class T>
  class RoiIteratorOf : public RoiIterator
  {
  };


  //---------------------------------------------------------------------------
  template <>
  class RoiIteratorOf< Graph > : public RoiIterator
  {
    const Graph *_roi;
    carto::rc_ptr< Graph > _roiLife;
    std::set< std::string > _names;
    std::set< std::string >::const_iterator _itNames;
    carto::rc_ptr< VoxelSampler > _voxelSampler;
    std::vector<std::string> _nameAttributes;

  public:

    RoiIteratorOf( const Graph &roi,
                   carto::rc_ptr< VoxelSampler > voxelSampler =
                   carto::rc_ptr< VoxelSampler >() );
    RoiIteratorOf( const carto::rc_ptr<Graph> &roi,
                   carto::rc_ptr< VoxelSampler > voxelSampler =
                   carto::rc_ptr< VoxelSampler >() );
    RoiIteratorOf( const std::string &fileName,
                   carto::rc_ptr< VoxelSampler > voxelSampler =
                   carto::rc_ptr< VoxelSampler >() );
    virtual ~RoiIteratorOf();

    virtual bool isValid() const;
    virtual void restart();
    virtual size_t count() const;

    /** Set region name attribute in graph.
        Normally "name" or "label". If several values are provided, attributes
        are searched in each graph vertex, in that order.
        If the attributes list is empty, then the graph "label_property"
        attribute will be used, and if it is not specified there, the default
        search list ("name", "label") will be used.
    */
    virtual void setRegionNameAttributes( const std::vector<std::string>
                                          & attributes );
    virtual std::string regionName() const;

    virtual carto::rc_ptr< MaskIterator > maskIterator() const;
    virtual void next();
  };



  //---------------------------------------------------------------------------
  template <class T>
  class RoiIteratorOf< carto::VolumeRef<T> > : public RoiIterator
  {
    mutable carto::VolumeRef<T> _data;
    Point3d _current;
    typename std::map< T, specifiedLabels > _labels;
    typename std::map< T, specifiedLabels >::const_iterator _label;
    carto::rc_ptr< VoxelSampler > _voxelSampler;

    void buildLabels();

  public:

    RoiIteratorOf( const carto::VolumeRef<T> &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler =
                   carto::rc_ptr< VoxelSampler >() );
    RoiIteratorOf( const std::string &fileName,
                   carto::rc_ptr< VoxelSampler > voxelSampler );
    virtual ~RoiIteratorOf();

    virtual bool isValid() const;
    virtual void restart();
    virtual size_t count() const;

    virtual std::string regionName() const;

    virtual carto::rc_ptr< MaskIterator > maskIterator() const;
    virtual void next();
  };




  //---------------------------------------------------------------------------
  template <class T>
  RoiIteratorOf< carto::VolumeRef<T> >::RoiIteratorOf(
    const carto::VolumeRef<T> &data,
    carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( data ),
    _voxelSampler( voxelSampler )
  {
    buildLabels();
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  RoiIteratorOf< carto::VolumeRef<T> >::RoiIteratorOf(
    const std::string &fileName, carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data(),
    _voxelSampler( voxelSampler )
  {
    Reader< carto::Volume<T> > reader( fileName );
    _data.reset( reader.read() );
    buildLabels();
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< carto::VolumeRef<T> >::buildLabels()
  {
    Point3d p;
    typename std::map< T, specifiedLabels >::iterator it;

    for( p[2] = 0; p[2] < _data->getSizeZ(); ++p[2] ) {
      for( p[1] = 0; p[1] < _data->getSizeY(); ++p[1] ) {
        for( p[0] = 0; p[0] < _data->getSizeX(); ++p[0] ) {
          T v = (*_data)( p );
          // NaN values are considered background like 0 (SPM does so)
          if ( v && !std::isnan( v ) ) {
            it = _labels.find( v );
            _labels[ v ].lastPoint = p;

            if ( it == _labels.end() ) {
              _labels[ v ].firstPoint = p;
            }
          }
        }
      }
    }
  }


  //---------------------------------------------------------------------------
  template <class T>
  RoiIteratorOf< carto::VolumeRef<T> >::~RoiIteratorOf()
  {
  }


  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator >
  RoiIteratorOf< carto::VolumeRef<T> >::
  maskIterator() const
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< carto::VolumeRef<T> >( _data, _label->second,
                                                   _voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< carto::VolumeRef<T> >::next()
  {
    ++_label;
  }


  //---------------------------------------------------------------------------
  template <class T>
  bool RoiIteratorOf< carto::VolumeRef<T> >::isValid() const
  {
    return _label != _labels.end();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< carto::VolumeRef<T> >::restart()
  {
    _label = _labels.begin();
  }

  //---------------------------------------------------------------------------
  template <class T>
  size_t  RoiIteratorOf< carto::VolumeRef<T> >::count() const
  {
    return _labels.size();
  }

  //---------------------------------------------------------------------------
  template <class T>
  std::string RoiIteratorOf< carto::VolumeRef<T> >::regionName() const
  {
    return carto::toString( _label->first );
  }

  //---------------------------------------------------------------------------
  // roiIterator factories (to be continued)

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const std::string &fileName,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() );

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const carto::VolumeRef< T > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() )
  {
    return carto::rc_ptr< RoiIterator >
      ( new RoiIteratorOf< carto::VolumeRef<T> >( data, voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const carto::rc_ptr<carto::Volume< T > > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() )
  {
    return carto::rc_ptr< RoiIterator >
      ( new RoiIteratorOf< carto::VolumeRef<T> >( data, voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const Graph &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() ) ;

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > getRoiIterator( const std::string &fileName,
                                               const AffineTransformation3d & );

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const std::string &fileName,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const AffineTransformation3d & );

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator > getRoiIterator(
    const carto::VolumeRef< T > &data, const AffineTransformation3d &motion )
  {
    return carto::
      rc_ptr< RoiIterator >( new MotionedRoiIterator( getRoiIterator( data ),
                                                      motion ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator > getRoiIterator(
    const carto::rc_ptr<carto::Volume< T > > &data, const AffineTransformation3d &motion )
  {
    return carto::
      rc_ptr< RoiIterator >( new MotionedRoiIterator( getRoiIterator( data ),
                                                      motion ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > getRoiIterator( const Graph &data,
                                               const AffineTransformation3d &motion ) ;

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const carto::VolumeRef< T > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const AffineTransformation3d &motion )
  {
    return carto::
      rc_ptr< RoiIterator >
      ( new MotionedRoiIterator( getRoiIterator( data, voxelSampler ),
                                 motion ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const carto::rc_ptr<carto::Volume< T > > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const AffineTransformation3d &motion )
  {
    return carto::
      rc_ptr< RoiIterator >
      ( new MotionedRoiIterator( getRoiIterator( data, voxelSampler ),
                                 motion ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator >
  getRoiIterator( const Graph &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const AffineTransformation3d &motion ) ;

  //---------------------------------------------------------------------------
  extern template class RoiIteratorOf<carto::VolumeRef<int16_t> >;
  extern template class RoiIteratorOf<Graph>;
  
} // namespace aims

#endif
