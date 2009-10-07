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

#ifndef CARTODATA_VOLUME_VOLUME_H
#define CARTODATA_VOLUME_VOLUME_H

#include <cartobase/object/object.h>
#include <cartodata/volume/volumeproxy.h>
#include <cartobase/containers/allocatedvector.h>
#include <cartobase/type/types.h>
#include <cartobase/wip/io/creator.h>

#ifdef CARTO_USE_BLITZ
#ifdef _WIN32
// disable thread-safe operations in blitz++ on windows since it uses pthread
#ifdef _REENTRANT
#define CARTO_REENTRANT_MEMO
#undef _REENTRANT
#endif
#include <blitz/blitz.h>
#ifdef CARTO_REENTRANT_MEMO
#define _REENTRANT
#undef CARTO_REENTRANT_MEMO
#endif
#endif

#include <blitz/array.h>
#endif

#define CARTO_VOLUME_AUTO_DEREFERENCE

namespace carto
{

  /** 4D Volume main class

  Volume is the normal class for volumes. VolumeView is a view into a Volume 
  and allows for instance to handle borders.

  Volumes should generally be used via reference-counting pointers (rc_ptr): 
  there is a (slightly) specialized rc_ptr for Volume subclasses: VolumeRef.

  \sa \ref cartovolumes

  To use Volume classes on "standard" types (numeric types, on which the 
  Volume class has already been compiled in the libraries), just include 
  <cartodata/volume/volume.h>. If you need to use them on other non-standard 
  types (if you get a link error about missing symbols), you will need to 
  recompile the Volume template class on these types: in this case you should 
  include <cartodata/volume/volume_d.h>, and force the compilation:

  \code
  #include <cartodata/volume/volume_d.h>

  template class carto::Volume<MyType>;
  \endcode
  This should be done only once (in one source file) for each type.
  */
  template < typename T >
  class Volume : public VolumeProxy< T >
  {

  public:

#ifdef CARTO_USE_BLITZ
    typedef typename blitz::Array<T,4>::iterator iterator;
    typedef typename blitz::Array<T,4>::const_iterator const_iterator;
#else
    typedef typename AllocatedVector<T>::iterator iterator;
    typedef typename AllocatedVector<T>::const_iterator const_iterator;
#endif

    /** Volume construction and allocation
	\param sizeX number of voxels
	\param sizeY number of voxels
	\param sizeZ number of voxels
	\param sizeT number of voxels
	\param allocatorContext information about how to allocate the volume: 
	it can be a bit complex to do really optimal things, but the default 
	value (default constructor of AllocatorContext) is OK in most cases.
	\param allocated normally left to \c true, it can exceptionnally be 
	set to \c false for "virtual" volumes that must not be actually 
	allocated but are only sources for a VolumeView.
     */
    Volume( int sizeX = 1, int sizeY = 1, int sizeZ = 1, int sizeT = 1,
            const AllocatorContext& allocatorContext = AllocatorContext(), 
	    bool allocated = true );
    /** This constructor builds a Volume on an already allocated buffer. 
        The Volume is not owner of the underlying data.
    */
    Volume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer );
    Volume( const Volume< T >& other );
    virtual ~Volume();

    Volume< T >& operator=( const Volume< T >& other );

    void fill( const T& value );

    /** Iterators returned here are the most "basic" (and fastest) iterators: 
	they go from the first voxel linerarly in memory, not taking care of 
	offsets when in a VolumeView. Taking care of splitting loops 
	line-by-line is the responsability of programmers using such iterators.
     */
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    /** Warning: this operator is not virtual, so may not have the expected 
	result on inherited classes (see VolumeView)
    */
    const T& operator()( int x, int y = 0, int z = 0, int t = 0 ) const;
    T& operator() ( int x, int y = 0, int z = 0, int t = 0 );
    /// same as \c operator()
    const T& at( int x, int y = 0, int z = 0, int t = 0 ) const;
    /// same as \c operator()
    T& at( int x, int y = 0, int z = 0, int t = 0 );
#ifdef CARTO_USE_BLITZ
    const T & at( const blitz::TinyVector<int,1> & ) const;
    T & at( const blitz::TinyVector<int,1> & );
    const T & at( const blitz::TinyVector<int,2> & ) const;
    T & at( const blitz::TinyVector<int,2> & );
    const T & at( const blitz::TinyVector<int,3> & ) const;
    T & at( const blitz::TinyVector<int,3> & );
    const T & at( const blitz::TinyVector<int,4> & ) const;
    T & at( const blitz::TinyVector<int,4> & );
    blitz::Array<T,4> at( const blitz::RectDomain<4> & subdomain ) const;
    blitz::Array<T,4> at( const blitz::StridedDomain<4> & subdomain ) const;
    blitz::Array<T,4> at( const blitz::Range & r0 ) const;
    blitz::Array<T,4> at( const blitz::Range & r0, 
                          const blitz::Range & r1 ) const;
    blitz::Array<T,4> at( const blitz::Range & r0, const blitz::Range & r1, 
                          const blitz::Range & r2 ) const;
    blitz::Array<T,4> at( const blitz::Range & r0, const blitz::Range & r1, 
                          const blitz::Range & r2, 
                          const blitz::Range & r3 ) const;
#endif

    virtual void initialize();
    const AllocatorContext & allocatorContext() const;
    /** This function is only useful in the particular context of an 
	unallocated Volume, when the constructor has been used with the 
	\c allocated flag to \c false.
	Calling allocate() afterwards will actually allocate the memory. 
	Otherwise it will do nothing.
    */
    void allocate();
    /// allows resizing and changing allocator
    virtual void reallocate( int sizeX = 1, int sizeY = 1, int sizeZ = 1, 
                             int sizeT = 1, bool keepcontents = false, 
                             const AllocatorContext& allocatorContext 
                             = AllocatorContext(), bool allocate = true );

  protected:

    void allocate( int oldSizeX, int oldSizeY, int oldSizeZ, int oldSizeT, 
		   bool allocate, const AllocatorContext& allocatorContext );
    void slotSizeChanged( const PropertyFilter& propertyFilter );

    AllocatedVector<T> _items;
#ifdef CARTO_USE_BLITZ
    blitz::Array<T, 4>	_blitz;
#else
    size_t	_lineoffset;
    size_t	_sliceoffset;
    size_t	_volumeoffset;
#endif
  };


  /** Convenient handle for a Volume - this is normally the entry point 
      for all volumes handling
  */
  template<typename T>
  class VolumeRef : public rc_ptr<Volume<T> >
  {
  public:
    VolumeRef();
    VolumeRef( Volume<T>* x );
    VolumeRef( const rc_ptr<Volume<T> > & x );

#ifdef CARTO_VOLUME_AUTO_DEREFERENCE
    typedef typename Volume<T>::iterator iterator;
    typedef typename Volume<T>::const_iterator const_iterator;

    VolumeRef( int sizeX, int sizeY = 1, int sizeZ = 1, int sizeT = 1,
               const AllocatorContext& allocatorContext = AllocatorContext(), 
               bool allocated = true )
      : rc_ptr<Volume<T> >( new Volume<T>( sizeX, sizeY, sizeZ, sizeT, 
                                           allocatorContext, allocated ) )
    {}

    int getSizeX() const { return (*this)->getSizeX(); }
    int getSizeY() const { return (*this)->getSizeY(); }
    int getSizeZ() const { return (*this)->getSizeZ(); }
    int getSizeT() const { return (*this)->getSizeT(); }
    const PropertySet& header() const
    { return (*this)->header(); }
    PropertySet& header() { return (*this)->header(); }
    /// Obsolete. still here for compatibility purpose. Use header() instead.
    const PropertySet& getPropertySet() const
    { return (*this)->header(); }
    /// Obsolete. still here for compatibility purpose. Use header() instead.
    PropertySet& getPropertySet() { return (*this)->header(); }

    void fill( const T& value ) { (*this)->fill( value ); }
    iterator begin() { return (*this)->begin(); }
    iterator end() { return (*this)->end(); }
    const_iterator begin() const { return (*this)->begin(); }
    const_iterator end() const { return (*this)->end(); }
    const T& operator()( int x, int y = 0, int z = 0, int t = 0 ) const
    { return (**this)( x, y, z, t ); }
    T& operator() ( int x, int y = 0, int z = 0, int t = 0 )
    { return (**this)( x, y, z, t ); }
    const T& at( int x, int y = 0, int z = 0, int t = 0 ) const
    { return (*this)->at( x, y, z, t ); }
    T& at( int x, int y = 0, int z = 0, int t = 0 )
    { return (*this)->at( x, y, z, t ); }
#endif
  };


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <typename T>
  class DataTypeCode<Volume<T> >
  {
  public:
    static std::string objectType()
    { return "CartoVolume"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return std::string("carto_volume of ") + DataTypeCode< T >::name(); 
    }
  };


  template <typename T>
  class DataTypeCode<VolumeRef<T> >
  {
  public:
    static std::string objectType()
    { return "VolumeRef"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return std::string("VolumeRef of ") + DataTypeCode< T >::name(); 
    }
  };


  template <typename T>
  class Creator<Volume<T> >
  {
  public:
    static Volume<T>* create( Object, const AllocatorContext &, Object );
    static void setup( Volume<T> &, Object, const AllocatorContext &, Object );
  };

#endif


  template < typename T >
  inline 
  const T& Volume< T >::at( int x, int y, int z, int t ) const
  {
#ifdef CARTO_USE_BLITZ
    return _blitz( x, y, z, t );
#else
    return _items[ x + y * _lineoffset + z * _sliceoffset 
                   + t * _volumeoffset ];
#endif
  }


  template < typename T >
  inline 
  const T& Volume< T >::operator()( int x, int y, int z, int t ) const
  {
    return at( x, y, z, t );
  }


  template < typename T >
  inline 
  T& Volume< T >::at( int x, int y, int z, int t )
  {
#ifdef CARTO_USE_BLITZ
    return _blitz( x, y, z, t);
#else
    return _items[ x + y * _lineoffset + z * _sliceoffset 
                   + t * _volumeoffset ];
#endif
  }


  template < typename T >
  inline 
  T& Volume< T >::operator()( int x, int y, int z, int t )
  {
    return at( x, y, z, t );
  }


#ifdef CARTO_USE_BLITZ
  template < typename T > inline 
  const T & Volume< T >::at( const blitz::TinyVector<int,1> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline 
  T & Volume< T >::at( const blitz::TinyVector<int,1> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline 
  const T & Volume< T >::at( const blitz::TinyVector<int,2> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline 
  T & Volume< T >::at( const blitz::TinyVector<int,2> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline 
  const T & Volume< T >::at( const blitz::TinyVector<int,3> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline 
  T & Volume< T >::at( const blitz::TinyVector<int,3> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline 
  const T & Volume< T >::at( const blitz::TinyVector<int,4> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline 
  T & Volume< T >::at( const blitz::TinyVector<int,4> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::RectDomain<4> & subdomain ) const
  {
    return _blitz( subdomain );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::StridedDomain<4> & subdomain ) const
  {
    return _blitz( subdomain );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::Range & r0 ) const
  {
    return _blitz( r0 );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1 ) const
  {
    return _blitz( r0, r1 );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1, 
                   const blitz::Range & r2 ) const
  {
    return _blitz( r0, r1, r2 );
  }

  template < typename T > inline 
  blitz::Array<T,4> 
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1, 
                   const blitz::Range & r2, const blitz::Range & r3 ) const
  {
    return _blitz( r0, r1, r2, r3 );
  }
#endif


  template <typename T>
  inline 
  VolumeRef<T>::VolumeRef()
    : rc_ptr<Volume<T> >( new Volume<T> )
  {
  }


  template <typename T>
  inline 
  VolumeRef<T>::VolumeRef( Volume<T>* x )
    : rc_ptr<Volume<T> >( x )
  {
  }


  template <typename T>
  inline 
  VolumeRef<T>::VolumeRef( const rc_ptr<Volume<T> > & x )
    : rc_ptr<Volume<T> >( x )
  {
  }


  template <typename T>
  carto::Object getObjectHeader( Volume<T> & obj )
  {
    return Object::reference( obj.header() );
  }

  template <typename T>
  carto::Object getObjectHeader( VolumeRef<T> & obj )
  {
    return Object::reference( obj->header() );
  }

 //  instanciations

  extern template class Volume<int8_t>;
  extern template class Volume<uint8_t>;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class Volume<char>;
#endif
  extern template class Volume<int16_t>;
  extern template class Volume<uint16_t>;
  extern template class Volume<int32_t>;
  extern template class Volume<uint32_t>;
  extern template class Volume<long>;
  extern template class Volume<unsigned long>;
  extern template class Volume<float>;
  extern template class Volume<double>;
  extern template class Volume<cfloat>;
  extern template class Volume<cdouble>;
  extern template class Volume< std::map<int, float> >;

  extern template class Creator<Volume<int8_t> >;
  extern template class Creator<Volume<uint8_t> >;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class Creator<Volume<char> >;
#endif
  extern template class Creator<Volume<int16_t> >;
  extern template class Creator<Volume<uint16_t> >;
  extern template class Creator<Volume<int32_t> >;
  extern template class Creator<Volume<uint32_t> >;
  extern template class Creator<Volume<long> >;
  extern template class Creator<Volume<unsigned long> >;
  extern template class Creator<Volume<float> >;
  extern template class Creator<Volume<double> >;
  extern template class Creator<Volume<cfloat> >;
  extern template class Creator<Volume<cdouble> >;
  extern template class Creator<Volume< std::map<int, float> > >;

  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int8_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint8_t > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< char > )
#endif
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int16_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint16_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int32_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint32_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< long > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< unsigned long > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< float > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< double > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< cfloat > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< cdouble > )

  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int8_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint8_t > > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< char > > )
#endif
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int16_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint16_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int32_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint32_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< long > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< unsigned long > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< float > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< double > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cfloat > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cdouble > > )


}


#endif
