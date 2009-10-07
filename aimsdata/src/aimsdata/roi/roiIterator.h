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
  
    VolumeROILabelMap( AimsData< int32_t > & );
    virtual ~VolumeROILabelMap() {}

    virtual int32_t at( const Point3df & ) const;
  
  private:
    AimsData< int32_t > _volume;
  };
  

  //---------------------------------------------------------------------------
  class MotionedVolumeROILabelMap : public VolumeROILabelMap
  {
  public:
  
    MotionedVolumeROILabelMap( AimsData< int32_t > &, const Motion & );
    virtual ~MotionedVolumeROILabelMap() {}
    
    virtual int32_t at( const Point3df & ) const;
  
  private:

    const Motion _motion;
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
    virtual std::string regionName() const = 0;
    
    virtual carto::rc_ptr< ROILabelMap > createLabelMap();
  };


  //---------------------------------------------------------------------------
  class MotionedRoiIterator : public RoiIterator
  {
  public:
    MotionedRoiIterator( const carto::rc_ptr< RoiIterator > &,
                         const Motion & );
    virtual ~MotionedRoiIterator();

    virtual bool isValid() const;
    virtual void restart();
    virtual size_t count() const;

    virtual void next();
    virtual carto::rc_ptr< MaskIterator > maskIterator() const;
    inline const Point3d volumeDimension() const;
    inline const Point3df voxelSize() const;
    virtual std::string regionName() const;

    virtual carto::rc_ptr< ROILabelMap > createLabelMap();

  protected:
    const carto::rc_ptr< RoiIterator > _roiIterator;
    const Motion _motion;
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

    virtual std::string regionName() const;

    virtual carto::rc_ptr< MaskIterator > maskIterator() const;
    virtual void next();
  };



  //---------------------------------------------------------------------------
  template <class T>
  class RoiIteratorOf< AimsData<T> > : public RoiIterator
  {
    mutable AimsData<T> *_data;
    bool _freeData;
    Point3d _current;
    typename std::map< T, specifiedLabels > _labels;
    typename std::map< T, specifiedLabels >::const_iterator _label;
    carto::rc_ptr< VoxelSampler > _voxelSampler;

    void buildLabels();

  public:

    RoiIteratorOf( const AimsData<T> &data,
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
  RoiIteratorOf< AimsData<T> >::RoiIteratorOf( const AimsData<T> &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( const_cast< AimsData<T> *>( &data ) ),
    _freeData( false ),
    _voxelSampler( voxelSampler )
  {
    buildLabels();
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  RoiIteratorOf< AimsData<T> >::RoiIteratorOf( const std::string &fileName,
                   carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( new AimsData<T> ),
    _freeData( true ),
    _voxelSampler( voxelSampler )
  {
    Reader< AimsData<T> > reader( fileName );
    reader.read( *_data );
    buildLabels();
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< AimsData<T> >::buildLabels()
  {
    Point3d p;
    typename std::map< T, specifiedLabels >::iterator it;
    
    for( p[2] = 0; p[2] < _data->dimZ(); ++p[2] ) {
      for( p[1] = 0; p[1] < _data->dimY(); ++p[1] ) {
        for( p[0] = 0; p[0] < _data->dimX(); ++p[0] ) {
          const T v = (*_data)( p );
          if ( v ) {
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
  RoiIteratorOf< AimsData<T> >::~RoiIteratorOf()
  {
    if ( _freeData ) delete _data;
  }


  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator > RoiIteratorOf< AimsData<T> >::
  maskIterator() const
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< AimsData<T> >( *_data, _label->second,
                                           _voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< AimsData<T> >::next()
  {
    ++_label;
  }


  //---------------------------------------------------------------------------
  template <class T>
  bool RoiIteratorOf< AimsData<T> >::isValid() const
  {
    return _label != _labels.end();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void RoiIteratorOf< AimsData<T> >::restart()
  {
    _label = _labels.begin();
  }

  //---------------------------------------------------------------------------
  template <class T>
  size_t  RoiIteratorOf< AimsData<T> >::count() const
  {
    return _labels.size();
  }

  //---------------------------------------------------------------------------
  template <class T>
  std::string RoiIteratorOf< AimsData<T> >::regionName() const
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
  getRoiIterator( const AimsData< T > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() )
  {
    return carto::rc_ptr< RoiIterator >
      ( new RoiIteratorOf< AimsData<T> >( data, voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > 
  getRoiIterator( const Graph &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler =
                  carto::rc_ptr< VoxelSampler >() ) ;

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > getRoiIterator( const std::string &fileName,
                                               const Motion & );

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > 
  getRoiIterator( const std::string &fileName,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const Motion & );
  
  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator > getRoiIterator( const AimsData< T > &data,
                                               const Motion &motion )
  {
    return carto::
      rc_ptr< RoiIterator >( new MotionedRoiIterator( getRoiIterator( data ), 
                                                      motion ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< RoiIterator > getRoiIterator( const Graph &data,
                                               const Motion &motion ) ;

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< RoiIterator > 
  getRoiIterator( const AimsData< T > &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler,
                  const Motion &motion )
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
                  const Motion &motion ) ;

} // namespace aims

#endif
