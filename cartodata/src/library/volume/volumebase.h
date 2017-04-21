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

#ifndef CARTODATA_VOLUME_VOLUMEBASE_H
#define CARTODATA_VOLUME_VOLUMEBASE_H
//--- cartodata --------------------------------------------------------------
#include <cartodata/config/config.h>
#include <cartodata/volume/volumeproxy.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/utilities/allocatedvector.h>
#include <soma-io/utilities/creator.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/allocator/allocator.h>
#include <cartobase/smart/rcptr.h>
//--- blitz ------------------------------------------------------------------
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

//--- std --------------------------------------------------------------------
#include <vector>
#include <iostream>
//--- forward declarations ---------------------------------------------------
namespace carto {
  class AllocatorContext;
  class PropertyFilter;
}
//----------------------------------------------------------------------------

namespace carto
{
//============================================================================
//   V O L U M E
//============================================================================
  /// 4D Volume main class
  ///
  /// Since 2013 release (version 4.4), Volume and VolumeView are merged into
  /// a singleclass. Every Volume can be a view into an other volume. If it is
  /// not, its parent volume is NULL. This allows to deal the same way with
  /// classic volumes, volumes with borders or partially read volumes.
  ///
  /// Volumes should generally be used via reference-counting pointers
  /// (rc_ptr): there is a (slightly) specialized rc_ptr for Volume
  /// subclasses: VolumeRef.
  ///
  /// Since 2016 release (version 4.5), comparison and arithmetic operators
  /// are specialized and shipped in volume.h. They can be used as long as
  /// the same operation between contained types is well defined.
  /// Several helper method (member and non member) are also included.
  ///
  /// \sa \ref cartovolumes
  ///
  /// To use Volume classes on "standard" types (numeric types, on which the
  /// Volume class has already been compiled in the libraries), just include
  /// <cartodata/volume/volume.h>. If you need to use them on other
  /// non-standard types (if you get a link error about missing symbols), you
  /// will need to recompile the Volume template class on these types: in this
  /// case you should include <cartodata/volume/volume_d.h> and force the
  /// compilation:
  /// \code
  /// #include <cartodata/volume/volume_d.h>
  ///
  /// template class carto::Volume<MyType>;
  /// \endcode
  /// This should be done only once (in one source file) for each type.
  template < typename T >
  class Volume : public VolumeProxy< T >
  {
  public:
    //========================================================================
    //   TYPES
    //========================================================================
    /// 4D int position / size.
    /// It contains a constructor enabling implicit conversion from any
    /// "vector-like" object, i.e. possessing a size() method and the []
    /// operator
    class Position4Di;

    /// Access to T type
    typedef T datatype;

    /// The most "basic" (and fastest) iterators:
    /// they go from the first voxel linerarly in memory, not taking care of
    /// offsets when in a Volume view. Taking care of splitting loops
    /// line-by-line is the responsability of programmers using such
#ifdef CARTO_USE_BLITZ
    typedef typename blitz::Array<T,4>::iterator iterator;
    typedef typename blitz::Array<T,4>::const_iterator const_iterator;
#else
    typedef typename AllocatedVector<T>::iterator iterator;
    typedef typename AllocatedVector<T>::const_iterator const_iterator;
#endif

    //========================================================================
    //   CONSTRUCTORS
    //========================================================================
    /// Volume construction and allocation
    /// \param sizeX number of voxels
    /// \param sizeY number of voxels
    /// \param sizeZ number of voxels
    /// \param sizeT number of voxels
    /// \param allocatorContext information about how to allocate the volume:
    /// it can be a bit complex to do really optimal things, but the default
    /// value (default constructor of AllocatorContext) is OK in most cases.
    /// \param allocated normally left to \c true, it can exceptionnally be
    /// set to \c false for "virtual" volumes that must not be actually
    /// allocated but are only sources for a view Volume.
    explicit Volume( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                     int sizeT = 1,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// \param size number of voxels in each dimension
    /// \param allocatorContext information about how to allocate the volume:
    /// it can be a bit complex to do really optimal things, but the default
    /// value (default constructor of AllocatorContext) is OK in most cases.
    /// \param allocated normally left to \c true, it can exceptionnally be
    /// set to \c false for "virtual" volumes that must not be actually
    /// allocated but are only sources for a view Volume.
    explicit Volume( const Position4Di & size,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation
    /// Same as the above constructor, but allows to specify a border size.
    /// Border is handled as a larger volume, with a view in it. The current
    /// volume is this view.
    explicit Volume( int sizeX, int sizeY, int sizeZ,
                     int sizeT, int bordersize,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// Same as the above constructor, but allows to specify a border size.
    /// Border is handled as a larger volume, with a view in it. The current
    /// volume is this view.
    explicit Volume( const Position4Di & size, int bordersize,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( int sizeX, int sizeY, int sizeZ, int sizeT,
                     const Position4Di & border,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( const Position4Di & size,
                     const Position4Di & border,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// This constructor builds a Volume on an already allocated buffer.
    /// The Volume is not owner of the underlying data.
    Volume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer );
    /// Position4Di version
    /// This constructor builds a Volume on an already allocated buffer.
    /// The Volume is not owner of the underlying data.
    Volume( const Position4Di & size, T* buffer );
    /// This is the volume view constructor.
    /// Beware not to mix it up with the copy constructor ( it takes a pointer
    /// to volume instead of a volume )
    /// If parent volume is allocated, view points to its data and doesn't own
    /// it. Else, it allocates \c size and owns it.
    Volume( rc_ptr<Volume<T> > other,
            const Position4Di & pos = Position4Di( 0, 0, 0, 0 ),
            const Position4Di & size = Position4Di( -1, -1, -1, -1 ),
            const AllocatorContext & allocContext = AllocatorContext() );
    /// Copy constructor
    /// The copy constructors actually duplicates data buffers. In the case
    /// of a volume view, the underlying volume is also duplicated, so the
    /// new volume will be a view in a new duplicate bigger volume.
    Volume( const Volume< T >& other );
    virtual ~Volume();

    Volume< T >& operator=( const Volume< T >& other );

    //========================================================================
    //   ITERATORS
    //========================================================================

    /// Iterators returned here are the most "basic" (and fastest) iterators:
    /// they go from the first voxel linerarly in memory, not taking care of
    /// offsets when in a Volume view. Taking care of splitting loops
    /// line-by-line is the responsability of programmers using such
    /// iterators.
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    //========================================================================
    //   ACCESSORS
    //========================================================================

    /// Warning: this operator is not virtual, so may not have the expected
    /// result on inherited classes (see old VolumeView)
    const T& operator()( long x, long y = 0, long z = 0, long t = 0 ) const;
    T& operator() ( long x, long y = 0, long z = 0, long t = 0 );
    const T& at( long x, long y = 0, long z = 0, long t = 0 ) const;
    T& at( long x, long y = 0, long z = 0, long t = 0 );
    const T& operator() ( const Position4Di & position ) const;
    T& operator() ( const Position4Di & position );
    const T& at( const Position4Di & position ) const;
    T& at( const Position4Di & position );

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

    //========================================================================
    //   INIT / ALLOCATION
    //========================================================================

    /// Initializes header info.
    virtual void initialize();
    /// returns volume's AllocatorContext
    const AllocatorContext & allocatorContext() const;
    /// This function is only useful in the particular context of an
    /// unallocated Volume, when the constructor has been used with the
    /// \c allocated flag to \c false.
    /// Calling allocate() afterwards will actually allocate the memory.
    /// Otherwise it will do nothing.
    void allocate();
    /// allows resizing and changing allocator
    virtual void reallocate( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                             int sizeT = 1, bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true );
    virtual void reallocate( const Position4Di & size,
                             bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true );

    //========================================================================
    //   COPY / VIEW
    //========================================================================

    /// Create a volume of same dimension and copy the data. This method
    /// does not deal with the underlying view structures, so borders or
    /// parent volumes are not copied or transfered.
    Volume<T> copy() const;
    template <typename OUTP>
    Volume<OUTP> copy() const;
    /// Copy the full data structure. This is similar to what the copy
    /// constructor does.
    Volume<T> deepcopy() const;
    template <typename OUTP>
    Volume<OUTP> deepcopy() const;

    /// Copy the full data structure without copying the actual data.
    Volume<T> copyStructure() const;
    /// Copy the full data structure without copying the actual data.
    template <typename OUTP>
    Volume<OUTP> copyStructure() const;

    /// Cast to Volume of different datatype
    template <typename OUTP>
    operator Volume<OUTP>() const;

    /// Get parent volume
    ///
    /// \note refVolume() returns a rc_ptr<Volume> and not a VolumeRef.
    /// Because arithmetic operators are only defined for Volume and VolumeRef
    /// objects, this result must be converted to a VolumeRef to be used in
    /// arithmetic operations.
    rc_ptr<Volume<T> > refVolume() const;
    /// Set parent volume
    void setRefVolume(const rc_ptr<Volume<T> > & refvol);
    /// Get position in parent volume
    const Position4Di posInRefVolume() const;
    /// Set position in parent volume
    void setPosInRefVolume(const Position4Di & pos);

    /// Get borders for the volume. A volume that can have borders is a volume \n
    /// that references another volume. It can be understood as a view in the reference volume.
    /// \return std::vector<uint16_t> that contains the borders availables for the volume.
    ///         vector[0]: low border in x direction
    ///         vector[1]: high border in x direction
    ///         vector[2]: low border in y direction
    ///         vector[3]: high border in y direction
    ///         vector[4]: low border in z direction
    ///         vector[5]: high border in z direction
    ///         vector[6]: low border in t direction
    ///         vector[7]: high border in t direction
    std::vector<int> getBorders() const;

    /// Get strides for the volume. Strides contain the number of voxels for \n
    /// each dimension including.
    /// \return std::vector<uint16_t> that contains the borders availables for
    ///                               the volume.
    ///         vector[0]: number of voxels for the 0 dimension. Value is 
    ///                    always 1 as no border is defined around each voxel.
    ///         vector[1]: number of voxels for the 1st dimension, i.e. in a 
    ///                    line including its borders.
    ///         vector[2]: number of voxels for the 2nde dimension, i.e. in a 
    ///                    slice including its borders.
    ///         vector[3]: number of voxels for the 3rd dimension, i.e. in a 
    ///                    volume including its borders.
    std::vector<size_t> getStrides() const;
    
    //========================================================================
    //   BOOLEANS / ACCUMULATED VALUES
    //========================================================================
    /// Implicit cast to bool
    /// returns all()
    operator bool() const;
    /// True if all values compare to true
    bool all() const;
    /// True if at least one value compares to true
    bool any() const;
    T min() const;
    T max() const;
    T sum() const;
    template <typename OUTP>
    OUTP sum() const;

    //========================================================================
    //   FILL / REPLACE
    //========================================================================
    /// Fills the volume with a given value.
    void fill( const T & value );
    Volume<T> & operator= ( const T & value );

  protected:
    //========================================================================
    //   PRIVATE UTILS
    //========================================================================
    void allocate( int oldSizeX, int oldSizeY, int oldSizeZ, int oldSizeT,
                   bool allocate, const AllocatorContext& allocatorContext );
    void slotSizeChanged( const PropertyFilter& propertyFilter );
    void updateItemsBuffer();

    void constructBorders( const Position4Di & bordersize,
                           const AllocatorContext& allocatorContext,
                           bool allocated );

    AllocatedVector<T> _items;
#ifdef CARTO_USE_BLITZ
    blitz::Array<T, 4>  _blitz;
#else
    size_t  _lineoffset;
    size_t  _sliceoffset;
    size_t  _volumeoffset;
#endif
    rc_ptr<Volume<T> >  _refvol;
    Position4Di         _pos;
  };

//============================================================================
//   POSITION VECTOR
//============================================================================
  template <typename T>
  class Volume<T>::Position4Di
  {
  public:
    explicit Position4Di( int x = 0, int y = 0, int z = 0, int t = 0 )
      : _coords( 4 )
    {
      _coords[0] = x;
      _coords[1] = y;
      _coords[2] = z;
      _coords[3] = t;
    }

    /// Generic constructor from any "vector-like" object, i.e. that has
    /// a size() method and the [] accessor.
    ///
    /// It is not explicit to allow implicit conversion.
    template <typename U>
    Position4Di( const U & other ):
      _coords(4)
    {
      for( int i = 0; i < other.size() && i < 4; ++i )
        _coords[i] = other[i];
      for( int i = other.size(); i < 4; ++i )
        _coords[i] = 0;
    }

    Position4Di( const Position4Di & pos ) : _coords( pos._coords )
    {
    }

    ~Position4Di() {}

          int & operator [] ( int coord )       { return _coords[ coord ]; }
    const int & operator [] ( int coord ) const { return _coords[ coord ]; }

    bool operator==(const Position4Di& p) const { return ( _coords[0] == p._coords[0]
                                                           && _coords[1] == p._coords[1]
                                                           && _coords[2] == p._coords[2]
                                                           && _coords[3] == p._coords[3] ); }
    bool operator!=(const Position4Di& p) const { return !(this->operator ==(p)); }

    unsigned size() const { return 4; }

  private:
    std::vector<int>  _coords;
  };

//============================================================================
//   UTILITIES
//============================================================================
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
  class Creator<Volume<T> >
  {
  public:
    static Volume<T>* create( Object, const AllocatorContext &, Object );
    static void setup( Volume<T> &, Object, const AllocatorContext &, Object );
  };

#endif

} // namespace carto

//============================================================================
// STREAM
//============================================================================

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto {

  // Warper for output streams that integrates parameters for volume
  // printing
  class VolumeOStream // : public std::ostream // is not really a stream
  {
  public:
    VolumeOStream( std::ostream & ostream );
    VolumeOStream( const VolumeOStream & other );
    ~VolumeOStream();

    std::ostream & ostream() const;

    const size_t & maxT() const;
    const size_t & maxZ() const;
    const size_t & maxY() const;
    const size_t & maxX() const;

    size_t & maxT();
    size_t & maxZ();
    size_t & maxY();
    size_t & maxX();

  private:
    size_t _maxT;
    size_t _maxZ;
    size_t _maxY;
    size_t _maxX;
    std::ostream & _ostream;
  };

  // Object returned by setMaxDim used to set Volume printing parameters
  class VolumeOStreamSetter
  {
  public:
    VolumeOStreamSetter();

    const size_t & maxT() const;
    const size_t & maxZ() const;
    const size_t & maxY() const;
    const size_t & maxX() const;

    size_t & maxT();
    size_t & maxZ();
    size_t & maxY();
    size_t & maxX();

  private:
    size_t _maxT;
    size_t _maxZ;
    size_t _maxY;
    size_t _maxX;
  };


} // namespace carto:

carto::VolumeOStream operator<< ( std::ostream & out,
                                  const carto::VolumeOStreamSetter & setter );

template <typename T>
std::ostream & operator<< ( const carto::VolumeOStream & out,
                            const carto::Volume<T> & volume );

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto {
  /// Method used to set Volume printing parameters
  VolumeOStreamSetter setMaxDim( size_t m );
}

/// Volumes are printable to standard output streams.
/// They are shown as an array, eventually cropped at a given size.
/// The default crop value is 5 in each dimension.
/// The crop value can be set with setMaxDim() the following way:
/// \code
/// #include <cartodata/volume/volume.h>
/// #include <iostream>
/// using namespace carto;
/// using namespace std;
///
/// carto::Volume<int16_t> vol( 10, 10, 10 );
/// cout << setMaxDim( 10 ) << vol << endl;
/// \endcode
template <typename T>
std::ostream & operator<< ( std::ostream & out,
                            const carto::Volume<T> & volume );


#endif // CARTODATA_VOLUME_VOLUMEBASE_H
