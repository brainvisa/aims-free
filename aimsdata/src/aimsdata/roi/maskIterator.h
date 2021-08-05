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


#ifndef AIMS_ROI_MASKITERATOR_H
#define AIMS_ROI_MASKITERATOR_H

#include <aims/io/reader.h>
#include <aims/roi/roi.h>
#include <aims/roi/voxelSampling.h>
#include <cartodata/volume/volume.h>
#include <cartobase/type/string_conversion.h>
#include <aims/transformation/affinetransformation3d.h>
#include <memory>
#include <string>

namespace aims
{

  struct specifiedLabels
  {
    Point3d firstPoint;
    Point3d lastPoint;
  };

  //---------------------------------------------------------------------------
  class MaskIterator : public carto::RCObject
  {
  public:
    virtual inline ~MaskIterator() {}

    // Returns the current point in voxel referential
    virtual const Point3d &value() const = 0;
    // Returns the current point in image or graph referential
    virtual const Point3df valueMillimeters() const = 0;
    virtual void next() = 0;
    virtual bool isValid() const = 0;
    virtual void restart() = 0;

    // Get voxel size
    virtual const Point3df voxelSize() const = 0;
    // Get voxel volume.
    // - In cases with a motion, returns the volume of a transform voxel
    // (which is a parallelepiped in the most general case)
    // - In other cases, returns vs[0]*vs[1]*vs[2]
    virtual float voxelVolume() const = 0;
    // Check if a point (in voxel referential) belongs to the mask
    virtual bool contains( const Point3d & ) const = 0;
    // Check if a point (in millimeter graph/image) belongs to the mask
    virtual bool contains( const Point3df & ) const = 0;
    // Get maximum coordinate
    virtual const Point3d volumeDimension() const = 0;
    virtual std::string regionName() const = 0;
  };


  //---------------------------------------------------------------------------
  class MotionedMaskIterator : public MaskIterator
  {
  public:
    MotionedMaskIterator( const carto::rc_ptr< MaskIterator > &,
                          const AffineTransformation3d & );
    virtual ~MotionedMaskIterator();

    virtual const Point3d &value() const;
    virtual const Point3df valueMillimeters() const;
    virtual void next();
    virtual bool isValid() const;
    virtual void restart();

    virtual const Point3df voxelSize() const;
    virtual float voxelVolume() const;
    virtual bool contains( const Point3d & ) const;
    virtual bool contains( const Point3df & ) const;
    virtual const Point3d volumeDimension() const;
    virtual std::string regionName() const;

  protected:
    carto::rc_ptr< MaskIterator > _maskIterator;
    AffineTransformation3d _motion;
    AffineTransformation3d _inverseMotion;
  };

  //---------------------------------------------------------------------------
  template <class T>
  class MaskIteratorOf : public MaskIterator
  {
  };


  //---------------------------------------------------------------------------
  class NodeFilter
  {
  public:

    inline virtual ~NodeFilter() {}

    virtual void setRegionNameAttributes(
      const std::vector<std::string> & ) {}
    virtual bool filter( const carto::AttributedObject & ) const;
  };


  //---------------------------------------------------------------------------
  class LabelNodeFilter : public NodeFilter
  {
    int _label;

  public:

    LabelNodeFilter( int label ) : _label( label ) {}
    inline virtual ~LabelNodeFilter() {}

    virtual bool filter( const carto::AttributedObject & ) const;
  };


  //---------------------------------------------------------------------------
  class NameNodeFilter : public NodeFilter
  {
    std::string _name;
    std::vector<std::string> _nameAttributes;

  public:

    NameNodeFilter( const std::string &name,
                    const std::vector<std::string> & name_attributes
                      = std::vector<std::string>() );
    inline virtual ~NameNodeFilter() {}

    virtual void setRegionNameAttributes(
      const std::vector<std::string> & attributes );
    virtual bool filter( const carto::AttributedObject & ) const;
  };


  //---------------------------------------------------------------------------
  template <>
  class MaskIteratorOf< Graph > : public MaskIterator
  {
    const Graph *_roi;
    carto::rc_ptr<Graph> _roiLife;
    Graph::const_iterator _itRoi;
    std::list<std::string> _attributeNames;
    std::list<std::string>::const_iterator _itAttributeNames;
    carto::rc_ptr< BucketMap<Void> > _bucketMap;
    const BucketMap<Void>::Bucket *_bucket;
    BucketMap<Void>::Bucket::const_iterator _itPoints;
    std::unique_ptr< NodeFilter > _nodeFilter;
    carto::rc_ptr< VoxelSampler > _voxelSampler;
    std::vector<std::string> _nameAttributes;

    static void
    _findBucketAttributeNames( const Graph &graph,
			       const Vertex &node,
			       std::list<std::string> &attributeNames );

    Point3df _voxelSize;
    void _getRequiredAttributes();

  public:

    MaskIteratorOf( const Graph &roi,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const Graph &roi, int label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const Graph &roi, const std::string &label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >(),
                    const std::vector<std::string> & regionNameAttributes =
                    std::vector<std::string>() );
    MaskIteratorOf( const carto::rc_ptr<Graph> &roi,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const carto::rc_ptr<Graph> &roi, int label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const carto::rc_ptr<Graph> &roi,
                    const std::string &label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >(),
                    const std::vector<std::string> & regionNameAttributes =
                    std::vector<std::string>() );
    MaskIteratorOf( const std::string &fileName,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const std::string &fileName, int label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const std::string &fileName, const std::string &label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >(),
                    const std::vector<std::string> & regionNameAttributes =
                    std::vector<std::string>() );
    virtual ~MaskIteratorOf();

    virtual const Point3df voxelSize() const { return _voxelSize; }
    virtual float voxelVolume() const { return _voxelSize[0]*_voxelSize[1]*_voxelSize[2]; }
    virtual const Point3d &value() const;
    virtual const Point3df valueMillimeters() const;
    virtual void next();
    virtual bool isValid() const;
    virtual void restart();

    virtual bool contains( const Point3d & ) const;
    virtual bool contains( const Point3df & ) const;
    virtual const Point3d volumeDimension() const;
    virtual std::string regionName() const;
    /** Set region name attribute in graph.
        Normally "name" or "label". If several values are provided, attributes
        are searched in each graph vertex, in that order.
        If the attributes list is empty, then the graph "label_property"
        attribute will be used, and if it is not specified there, the default
        search list ("name", "label") will be used.
    */
    virtual void setRegionNameAttributes( const std::vector<std::string>
                                          & attributes );
  };



  //---------------------------------------------------------------------------
  template <class T>
  class MaskIteratorOf< carto::VolumeRef<T> > : public MaskIterator
  {
    mutable carto::VolumeRef<T> _data;
    Point3d _current;
    T _label;
    Point3d _lastPoint;
    bool _useLabel;
    carto::rc_ptr< VoxelSampler > _voxelSampler;

  public:

    MaskIteratorOf( const carto::VolumeRef<T> &data,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const carto::VolumeRef<T> &data, const T &label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const carto::VolumeRef<T> &data,
                    const specifiedLabels &labels,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const std::string &fileName,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const std::string &fileName, const T &label,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    MaskIteratorOf( const std::string &fileName, const specifiedLabels &labels,
                    carto::rc_ptr< VoxelSampler > voxelSampler =
                    carto::rc_ptr< VoxelSampler >() );
    virtual ~MaskIteratorOf();

    virtual const Point3df voxelSize() const
    {
      Point3df vs( 3, 1. );
      try
      {
        carto::Object o = _data->header().getProperty( "voxel_size" );
        vs[0] = o->getArrayItem( 0 )->getScalar();
        vs[1] = o->getArrayItem( 1 )->getScalar();
        vs[2] = o->getArrayItem( 2 )->getScalar();
      }
      catch( ... )
      {
      }
      return vs;
    }
    virtual float voxelVolume() const
    {
      Point3df vs = voxelSize();
      return vs[0] * vs[1] * vs[2];
    }
    virtual const Point3d &value() const;
    virtual const Point3df valueMillimeters() const;
    virtual void next();
    virtual bool isValid() const;
    virtual void restart();

    virtual bool contains( const Point3d & ) const;
    virtual bool contains( const Point3df & ) const;
    virtual const Point3d volumeDimension() const;
    virtual std::string regionName() const;

  protected:

    virtual void restart( const T &label );
    virtual void restart( const specifiedLabels &labels );

  };




  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const carto::VolumeRef<T> &data,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( data ),
    _voxelSampler( voxelSampler )
  {
    _useLabel = false;
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const carto::VolumeRef<T> &data, const T  &label,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( data ),
    _voxelSampler( voxelSampler )
  {
    _useLabel = true;
    restart( label );
  }

  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const carto::VolumeRef<T> &data,
                  const specifiedLabels &labels,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data( data ),
    _voxelSampler( voxelSampler )
  {
    _useLabel = true;
    restart( labels );
  }

  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const std::string &fileName,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data(),
    _voxelSampler( voxelSampler )
  {
    Reader< carto::Volume<T> > reader( fileName );
    _data.reset( reader.read() );
    _useLabel = false;
    restart();
  }

  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const std::string &fileName, const T &label,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data(),
    _voxelSampler( voxelSampler )
  {
    Reader< carto::Volume<T> > reader( fileName );
    _data.reset( reader.read() );
    _useLabel = true;
    restart( label );
  }


  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::
  MaskIteratorOf( const std::string &fileName, const specifiedLabels &labels,
                  carto::rc_ptr< VoxelSampler > voxelSampler ) :
    _data(),
    _voxelSampler( voxelSampler )
  {
    Reader< carto::Volume<T> > reader( fileName );
    _data.reset( reader.read() );
    _useLabel = true;
    restart( labels );
  }

  //---------------------------------------------------------------------------
  template <class T>
  MaskIteratorOf< carto::VolumeRef<T> >::~MaskIteratorOf()
  {
  }


  //---------------------------------------------------------------------------
  template <class T>
  const Point3d &MaskIteratorOf< carto::VolumeRef<T> >::value() const
  {
    return _current;
  }

  //---------------------------------------------------------------------------
  template <class T>
  const Point3df MaskIteratorOf< carto::VolumeRef<T> >::valueMillimeters() const
  {
    Point3df vs = voxelSize();
    if ( _voxelSampler.isNull() )
    {
      return Point3df( _current[0] * vs[0],
                       _current[1] * vs[1],
                       _current[2] * vs[2] );
    } else {
      const Point3df &voxelSample = _voxelSampler->value();
      return Point3df( ( _current[0] + voxelSample[ 0 ] ) * vs[0],
                       ( _current[1] + voxelSample[ 1 ] ) * vs[1],
                       ( _current[2] + voxelSample[ 2 ] ) * vs[2] );
    }
  }

  //---------------------------------------------------------------------------
  template <class T>
  void MaskIteratorOf< carto::VolumeRef<T> >::next()
  {
    if ( (_useLabel && (*_data)( _current ) == (T) _label) ||
      (!_useLabel && (*_data)( _current  ) ) )
    {
      // Next sample in voxel
      if ( ! _voxelSampler.isNull() )
      {
        _voxelSampler->next();
        if ( _voxelSampler->isValid() )
          return;
      }
    }

    if ( ! _voxelSampler.isNull() )
    {
      _voxelSampler->restart();
    }

    if ( _current == _lastPoint ) // (to make it not valid )
    {
      _current[ 0 ] = _data->getSizeX();
      _current[ 1 ] = _data->getSizeY();
      _current[ 2 ] = _data->getSizeZ();
    }
    // Next point
    do {
      ++_current[ 0 ];
      if ( _current[ 0 ] == _data->getSizeX() ) {
        _current[ 0 ] = 0;
        ++_current[ 1 ];
        if ( _current[ 1 ] == _data->getSizeY() ) {
          _current[ 1 ] = 0;
          ++_current[ 2 ];
        }
      }
    } while( isValid() && ( _useLabel ?
                            (*_data)( _current ) != (T) _label :
                            ! (*_data)( _current  ) ) );
  }


  //---------------------------------------------------------------------------
  template <class T>
  bool MaskIteratorOf< carto::VolumeRef<T> >::isValid() const
  {
    return _current[ 2 ] < _data->getSizeZ();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void MaskIteratorOf< carto::VolumeRef<T> >::restart()
  {
    if ( ! _voxelSampler.isNull() )
    {
      _voxelSampler->restart();
    }

    _current[ 0 ] = _current[ 1 ] = _current[ 2 ] = 0;
    _lastPoint[ 0 ] = _data->getSizeX()-1;
    _lastPoint[ 1 ] = _data->getSizeY()-1;
    _lastPoint[ 2 ] = _data->getSizeZ()-1;

    if ( isValid() &&  ( _useLabel ?
                         (*_data)( _current ) != (T) _label :
                         ! (*_data)( _current  ) ) ) next();
  }

  //---------------------------------------------------------------------------
  template <class T>
  void MaskIteratorOf< carto::VolumeRef<T> >::restart( const T &label )
  {
    if ( ! _voxelSampler.isNull() )
    {
      _voxelSampler->restart();
    }

    _label = label;
    _useLabel = true;
    _current[ 0 ] = _current[ 1 ] = _current[ 2 ] = 0;
    _lastPoint[ 0 ] = _data->getSizeX()-1;
    _lastPoint[ 1 ] = _data->getSizeY()-1;
    _lastPoint[ 2 ] = _data->getSizeZ()-1;

    if ( isValid() && (  (*_data)( _current ) != _label ) )
    {
      next();
    }
  }

  //---------------------------------------------------------------------------
  template <class T>
  void MaskIteratorOf< carto::VolumeRef<T> >::restart(
    const specifiedLabels &labels )
  {
    if ( ! _voxelSampler.isNull() )
    {
      _voxelSampler->restart();
    }

    _label = (*_data)( labels.firstPoint );
    _current = labels.firstPoint;
    _lastPoint = labels.lastPoint;
  }


  //---------------------------------------------------------------------------
  template <class T>
  bool MaskIteratorOf< carto::VolumeRef<T> >::contains(
    const Point3d &p ) const
  {
    if ( p[ 0 ] >= 0 && p[ 0 ] < _data->getSizeX() &&
	 p[ 1 ] >= 0 && p[ 1 ] < _data->getSizeY() &&
	 p[ 2 ] >= 0 && p[ 2 ] < _data->getSizeZ() )
    {
      if ( _useLabel )
      {
        return (*_data)( p ) == _label;
      }
      else
      {
        return (*_data)( p );
      }
    }
    return false;
  }

  //---------------------------------------------------------------------------
  template <class T>
  bool MaskIteratorOf< carto::VolumeRef<T> >::contains(
    const Point3df &p ) const
  {
    Point3df vs = voxelSize();
    const Point3d pixel( (short) rint( p[ 0 ] / vs[0] ),
			 (short) rint( p[ 1 ] / vs[1] ),
			 (short) rint( p[ 2 ] / vs[2] ) );
    return contains( pixel );
  }


  //---------------------------------------------------------------------------
  template <class T>
  const Point3d MaskIteratorOf< carto::VolumeRef<T> >::volumeDimension() const
  {
    return Point3d( _data->getSizeX(), _data->getSizeY(), _data->getSizeZ() );
  }


  //---------------------------------------------------------------------------
  template <class T>
  std::string MaskIteratorOf< carto::VolumeRef<T> >::regionName() const
  {
    if ( _useLabel ) return carto::toString( _label );
    return "unknown";
  }


  //---------------------------------------------------------------------------
  // maskIteratorFactories (to be continued)

  //---------------------------------------------------------------------------
  carto::rc_ptr< MaskIterator > getMaskIterator( const std::string &fileName );

  //---------------------------------------------------------------------------
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const std::string &fileName,
                   carto::rc_ptr< VoxelSampler > voxelSampler );

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator > getMaskIterator(
    const carto::rc_ptr<carto::Volume< T > > &data )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< carto::VolumeRef<T> >( data ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator > getMaskIterator(
    const carto::VolumeRef< T > &data )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< carto::VolumeRef<T> >( data ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const carto::VolumeRef< T > &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< carto::VolumeRef<T> >( data, voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const carto::rc_ptr<carto::Volume< T > > &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MaskIteratorOf< carto::VolumeRef<T> >( data, voxelSampler ) );
  }

  //---------------------------------------------------------------------------
  carto::rc_ptr< MaskIterator > getMaskIterator(
    const std::string &fileName, const AffineTransformation3d &motion );

  //---------------------------------------------------------------------------
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const std::string &fileName,
                   carto::rc_ptr< VoxelSampler > voxelSampler,
                   const AffineTransformation3d &motion );

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator > getMaskIterator(
    const carto::VolumeRef< T > &data, const AffineTransformation3d &motion )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MotionedMaskIterator( getMaskIterator( data ), motion ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator > getMaskIterator(
    const carto::rc_ptr<carto::Volume< T > > &data,
    const AffineTransformation3d &motion )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MotionedMaskIterator( getMaskIterator( data ), motion ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const carto::VolumeRef< T > &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler,
                   const AffineTransformation3d &motion )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MotionedMaskIterator( getMaskIterator( data, voxelSampler ),
                                  motion ) );
  }

  //---------------------------------------------------------------------------
  template <class T>
  carto::rc_ptr< MaskIterator >
  getMaskIterator( const carto::rc_ptr<carto::Volume< T > > &data,
                   carto::rc_ptr< VoxelSampler > voxelSampler,
                   const AffineTransformation3d &motion )
  {
    return carto::rc_ptr< MaskIterator >
      ( new MotionedMaskIterator( getMaskIterator( data, voxelSampler ),
                                  motion ) );
  }


} // namespace aims

#endif
